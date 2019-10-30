/**
* @file uart_rx_task.c
* @brief This file contains the task responsible for retrieving
* and executing commands from the front end
*
* @author SDMAY18-31
*/

/*-----------------------------------------------------------------
-                           Includes
-----------------------------------------------------------------*/
#include "uart_rx_task.h"

/*-----------------------------------------------------------------
-                       Literal Constants
-----------------------------------------------------------------*/
#define UARTRXTASKSTACKSIZE 256         // Stack size in words

/*-----------------------------------------------------------------
-                       Global Variables
-----------------------------------------------------------------*/
digital_config_t*           pDigitalConfig;   // Config for digital sensors
extern xSemaphoreHandle     mutex_digital_state;    // Mutex to enable and disable digital sensor collection

extern QueueHandle_t    data_queue;      // Queue for loading packets from ADC isrs
extern xSemaphoreHandle mutex_uart_tx;  // Mutex protecting UART TX
extern bool             collecting;     // Condition variable for if device is collecting
extern TaskHandle_t     digitalSensorTaskHandle;

/*-----------------------------------------------------------------
-                          Prototypes
-----------------------------------------------------------------*/
bool process_rx_pkt( rx_cmd_pkt_t* pkt );
void adc_config_process( adc_config_data_t* config, uint32_t param1, uint32_t param2 );

/*-----------------------------------------------------------------
-                          Procedures
-----------------------------------------------------------------*/

/**
* This task is responsible for retrieving and executing commands
* from the front end
*
* @param pvParameters unused
*/
static void uart_rx_task( void *pvParameters )
{
    uart_driver_init();
    rx_cmd_pkt_t current_pkt;

    digital_config_t digitalConfig =
    {
        0,
        0,
        0,
        0
    };

    pDigitalConfig = &digitalConfig;

    while( 1 )
    {
        // Block until RX interrupt is signal is sent. Interrupt will fire when the
        // FIFO is 3/4 full, 12 bytes, the fixed size of the command packets.
        uart_block_for_rx();
        xSemaphoreTake( mutex_uart_tx, portMAX_DELAY );
        watchdog_reset_countdown();

        // If a command packet is received while the device is collecting, we need to
        // disable collections, as the device can only be configured while stopped.
        if( true == collecting )
        {
            adc_disable_dev( ADC_SEQUENCE_0 );
            adc_disable_dev( ADC_SEQUENCE_1 );
            adc_disable_dev( ADC_SEQUENCE_2 );
            collecting = false;
        }

        // Grab bytes from RX FIFO and pack them into the command packet structure.
        uart_get_rx_pkt( &current_pkt );

        // Process packet that was received
        if( true == process_rx_pkt( &current_pkt ) )
        {
            current_pkt.start_byte  = START_BYTE;
            current_pkt.cmd         = ACK;
        }
        else
        {
            current_pkt.start_byte  = START_BYTE;
            current_pkt.cmd         = NAK;
        }

        current_pkt.crc = uart_calc_crc( (uint8_t*)&current_pkt, ( sizeof( rx_cmd_pkt_t ) - CRC_SIZE ) );

        uart_send_string( (uint8_t*)&current_pkt, sizeof( rx_cmd_pkt_t ) );

        watchdog_reset_countdown();
        xSemaphoreGive( mutex_uart_tx );
        taskYIELD();
    }
}

/**
* Process data packet and execute the command
*
* @param pkt pointer to the data packet
*/
bool process_rx_pkt( rx_cmd_pkt_t* pkt )
{
    bool ret;
    /* Check packet validity */
    if( 0 == uart_calc_crc( (uint8_t*)pkt, sizeof( rx_cmd_pkt_t ) ) )
    {
        adc_config_data_t config_data;
        switch( pkt->cmd )
        {
        case START_COLLECTION:
            collecting = true;

            xSemaphoreTake(mutex_digital_state, portMAX_DELAY);
            pDigitalConfig->collecting = true;
            xSemaphoreGive(mutex_digital_state);
            break;

        case ENABLE_DEV:
            xQueueReset( data_queue );
            config_data.adc_num = pkt->param1;
            adc_enable_dev( config_data.adc_num );
            break;

        case STOP_COLLECTION:
        // TODO Add other devices as needed -TVT
            if( true == collecting )
            {
                adc_disable_dev( ADC_SEQUENCE_0 );
                adc_disable_dev( ADC_SEQUENCE_1 );
                adc_disable_dev( ADC_SEQUENCE_2 );
                    // TODO disable I2C and SPI
                if(pDigitalConfig->collecting)
                {
                    xSemaphoreTake(mutex_digital_state, portMAX_DELAY);
                    pDigitalConfig->collecting = false;
                    pDigitalConfig->enabled = false;
                    xSemaphoreGive(mutex_digital_state);
                    //decrease priority
                    vTaskPrioritySet(digitalSensorTaskHandle, PRIORITY_UART_TX_TASK - 1);
                }

                xQueueReset( data_queue );
                collecting = false;
            }

                break;

            // Consider reducing the ADC config commands if the sampling rate is the only
            // parameter that needs to be set.
            // TODO Create proper configuration for sequences 1 and 2 -TVT
            case CONFIG_ADC0:
                config_data.adc_num = ADC_SEQUENCE_0;
                adc_config_process( &config_data, pkt->param1, pkt->param2 );
                break;

            case CONFIG_ADC1:
                config_data.adc_num = ADC_SEQUENCE_1;
                adc_config_process( &config_data, pkt->param1, pkt->param2 );
                break;

            case CONFIG_ADC2:
                config_data.adc_num = ADC_SEQUENCE_2;
                adc_config_process( &config_data, pkt->param1, pkt->param2 );
                break;

            case TUNE_FILTER:
            {
                uint16_t cutoff;

                cutoff = pkt->param2;

                switch( pkt->param1 )
                {
                    case FILTER_6TH_ORDER_BP:
                        filter_tune_sixth_order_BPF( cutoff );
                        break;

                    case FILTER_1ST_ORDER_LP:
                        filter_tune_first_order_LPF( cutoff );
                        break;

                    case FILTER_5TH_ORDER_LP:
                        filter_tune_fifth_order_LPF( cutoff );
                        break;

                    case FILTER_2ND_ORDER_BP:
                        filter_tune_second_order_BPF( cutoff );
                        break;

                    case FILTER_5TH_ORDER_HP:
                        filter_tune_fifth_order_HPF( cutoff );
                        break;

                    case FILTER_1ST_ORDER_HP:
                        filter_tune_first_order_HPF( cutoff );
                        break;

                    // Passthrough filter is not tunable, NOP
                    case FILTER_PASSTHROUGH:
                    default:
                        break;
                }
                break;
            }

            case CONFIG_I2C:
            case CONFIG_SPI:
            {
                xSemaphoreTake(mutex_digital_state, portMAX_DELAY);
                pDigitalConfig->sensor = pkt->param1;
                pDigitalConfig->sampleRate = pkt->param2 & 0xffff;
                pDigitalConfig->enabled = true;
                //increase priority
                vTaskPrioritySet(digitalSensorTaskHandle, PRIORITY_UART_TX_TASK + 1);
                xSemaphoreGive(mutex_digital_state);
                break;
            }

            default:
                break;
        }

        ret = true;
    }
    else
    {
        ret = false;
    }

    return ret;
}

/**
* Configure ADC channel
* Set the sampling rate and filter route
*
* @param config pointer to ADC config data struct
* @param param1 the setting to configure
* @param param2 the setting value
*/
void adc_config_process( adc_config_data_t* config, uint32_t param1, uint32_t param2 )
{
    switch( param1 )
    {
        case SET_SAMPLING_RATE:
            config->sampling_rate = param2;
            adc_config( config );
            break;

        case SET_FILTER_ROUTE:
            config->filter_route = param2;
            mux_set_filter_route( config->adc_num, config->filter_route );
            break;

        default:
            break;
    }
}

/**
* This function is used to create the uart rx task
*
* @return 0 on success and 1 on failure
*/
uint32_t uart_rx_task_init()
{
    if( xTaskCreate( uart_rx_task,
        (const portCHAR *)"UART_RX",
        UARTRXTASKSTACKSIZE, NULL,
        tskIDLE_PRIORITY + PRIORITY_UART_RX_TASK, NULL) != pdTRUE )
    {
        return 1;
    }

    return 0;
}

