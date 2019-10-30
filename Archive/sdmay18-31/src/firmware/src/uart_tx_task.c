/**
* @file uart_tx_task.c
* @brief This file contains the task responsible for sending
* sensor data to the front end
*
* @author SDMAY18-31
*/

/*-----------------------------------------------------------------
-                           Includes
-----------------------------------------------------------------*/
#include "uart_tx_task.h"

/*-----------------------------------------------------------------
-                       Literal Constants
-----------------------------------------------------------------*/
#define UARTTXTASKSTACKSIZE 128         // Stack size in words

/*-----------------------------------------------------------------
-                       Global Variables
-----------------------------------------------------------------*/
extern QueueHandle_t    data_queue;      // Queue for loading packets from ADC isrs
extern xSemaphoreHandle mutex_uart_tx;  // Mutex protecting UART TX
extern bool             collecting;     // Condition variable for if device is collecting

/*-----------------------------------------------------------------
-                          Procedures
-----------------------------------------------------------------*/

/**
* This task is responsible for sending sensor data to the front end
*
* @param pvParameters unused
*/
static void uart_tx_task( void *pvParameters )
{
    rx_data_pkt_t data_pkt;

    while(1)
    {
        // Wait until at least 5 packets are ready
        if( ( true == collecting ) &&
            ( xQueuePeek( data_queue, &( data_pkt.data[0] ), portMAX_DELAY ) ) &&
            ( uxQueueMessagesWaiting( data_queue ) >= 5 ) )
        {
            // Stuff samples into packet
            xQueueReceive( data_queue, &( data_pkt.data[0] ), portMAX_DELAY );
            xQueueReceive( data_queue, &( data_pkt.data[1] ), portMAX_DELAY );
            xQueueReceive( data_queue, &( data_pkt.data[2] ), portMAX_DELAY );
            xQueueReceive( data_queue, &( data_pkt.data[3] ), portMAX_DELAY );
            xQueueReceive( data_queue, &( data_pkt.data[4] ), portMAX_DELAY );

            data_pkt.crc = uart_calc_crc( (uint8_t*)&data_pkt, ( sizeof( rx_data_pkt_t ) - CRC_SIZE ) );

            // Send off packet
            xSemaphoreTake( mutex_uart_tx, portMAX_DELAY );
            uart_send_string( ( uint8_t* )&data_pkt, sizeof( rx_data_pkt_t ) );
            watchdog_reset_countdown();
            xSemaphoreGive( mutex_uart_tx );
        }
        taskYIELD();
    }
}

/**
* This function is used to create the uart tx task
*
* @return 0 on success and 1 on failure
*/
uint32_t uart_tx_task_init()
{
    if( xTaskCreate( uart_tx_task,
        (const portCHAR *)"UART_TX",
        UARTTXTASKSTACKSIZE, NULL,
        tskIDLE_PRIORITY + PRIORITY_UART_TX_TASK, NULL ) != pdTRUE )
    {
        return 1;
    }


    return 0;
}

