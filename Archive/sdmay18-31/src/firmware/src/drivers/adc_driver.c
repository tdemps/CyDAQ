/**
* @file adc_driver.c
* @brief This file contains the functions needed to drive the ADC
* module with FreeRTOS
*
* @author SDMAY18-31
*/

/*-----------------------------------------------------------------
-                           Includes
-----------------------------------------------------------------*/
#include "drivers/adc_driver.h"

/*-----------------------------------------------------------------
-                       Literal Constants
-----------------------------------------------------------------*/

/*-----------------------------------------------------------------
-                       Global Variables
-----------------------------------------------------------------*/
extern QueueHandle_t    data_queue;      // Queue for loading packets from ADC isrs

static uint32_t         adc_val;        // Current ADC value
static uint16_t         lower_16_bits;  // ADC value after trimming

/*-----------------------------------------------------------------
-                          Prototypes
-----------------------------------------------------------------*/
static uint32_t get_pwm_clk();

/*-----------------------------------------------------------------
-                          Procedures
-----------------------------------------------------------------*/

/**
* ISR responsible for copying data from the ADC channel 0 conversion
* to the data queue
*/
void adc_driver_isr_0()
{
    ADCIntClear( ADC0_BASE, ADC_SEQUENCE_0 );
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Fetch current adc value and cutoff the top 16 bits
    ADCSequenceDataGet( ADC0_BASE, ADC_SEQUENCE_0, &adc_val );
    lower_16_bits = ( adc_val & 0xffff );

    // Mask in ADC sequence number as a sample ID
    // Note this is not actually necessary for sequence 0, but is left here
    // for uniformity.
    lower_16_bits |= ADC_SEQUENCE_0 << 12;

    // Post ADC value to the queue
    xQueueSendToBackFromISR( data_queue, &lower_16_bits, &xHigherPriorityTaskWoken );

    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

/**
* ISR responsible for copying data from the ADC channel 1 conversion
* to the data queue
*/
void adc_driver_isr_1()
{
    ADCIntClear( ADC0_BASE, ADC_SEQUENCE_1 );
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Fetch current adc value and cutoff the top 16 bits
    ADCSequenceDataGet( ADC0_BASE, ADC_SEQUENCE_1, &adc_val );
    lower_16_bits = ( adc_val & 0xffffd );

    // Mask in ADC sequence number as a sample ID
    lower_16_bits |= ADC_SEQUENCE_1 << 12;

    // Post ADC value to the queue
    xQueueSendToBackFromISR( data_queue, &lower_16_bits, &xHigherPriorityTaskWoken );

    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

/**
* ISR responsible for copying data from the ADC channel 2 conversion
* to the data queue
*/
void adc_driver_isr_2()
{
    ADCIntClear( ADC0_BASE, ADC_SEQUENCE_2 );
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Fetch current adc value and cutoff the top 16 bits
    ADCSequenceDataGet( ADC0_BASE, ADC_SEQUENCE_2, &adc_val );
    lower_16_bits = ( adc_val & 0xffff );

    // Mask in ADC sequence number as a sample ID
    lower_16_bits |= ADC_SEQUENCE_2 << 12;

    // Post ADC value to the queue
    xQueueSendToBackFromISR( data_queue, &lower_16_bits, &xHigherPriorityTaskWoken );

    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

/**
* Disable ADC channel
*
* @param adc_num ADC channel number
*/
void adc_disable_dev( uint32_t adc_num )
{
    ADCSequenceDisable( ADC0_BASE, adc_num );
    ADCIntUnregister( ADC0_BASE, adc_num );
    ADCIntDisable( ADC0_BASE, adc_num );
    ADCIntClear( ADC0_BASE, adc_num );
}

/**
* Enable ADC channel
*
* @param adc_num ADC channel number
*/
void adc_enable_dev( uint32_t adc_num )
{

    ADCSequenceEnable( ADC0_BASE, adc_num );
}

/**
* Configure ADC channel
* Ensure sequence is disabled before configuring device
* Note: Currently this only works with 1 ADC channel enabled. We will likly need to write timer
* interrupts for each channel to trigger at different rates.
*
* @param config pointer to the ADC configuration
*/
void adc_config( adc_config_data_t* config )
{
    // Configure ADC channels
    // TODO Reduce the amount of redunant code in the cases -TVT
    switch( config->adc_num )
    {
        case ADC_SEQUENCE_0:
            ADCSequenceDisable( ADC0_BASE, ADC_SEQUENCE_0 );
            PWMGenConfigure( PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC );

            PWMGenPeriodSet( PWM0_BASE, PWM_GEN_0, get_pwm_clk() / ( config->sampling_rate ) );
            PWMGenEnable( PWM0_BASE, PWM_GEN_0 );
            PWMGenIntTrigEnable( PWM0_BASE, PWM_GEN_0, PWM_TR_CNT_ZERO );

            ADCSequenceConfigure( ADC0_BASE, ADC_SEQUENCE_0, ADC_TRIGGER_PWM0 | ADC_TRIGGER_PWM_MOD0, ADC_SEQUENCE_0 );
            ADCSequenceStepConfigure( ADC0_BASE, ADC_SEQUENCE_0, 0, ADC_CTL_CH0 | ADC_CTL_END | ADC_CTL_IE );
            ADCIntRegister( ADC0_BASE, ADC_SEQUENCE_0, adc_driver_isr_0 );
            IntPrioritySet( INT_ADC0SS0, configKERNEL_INTERRUPT_PRIORITY );
            break;

        case ADC_SEQUENCE_1:
            ADCSequenceDisable( ADC0_BASE, ADC_SEQUENCE_1 );
            PWMGenConfigure( PWM0_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC );
            PWMGenPeriodSet( PWM0_BASE, PWM_GEN_1, get_pwm_clk() / config->sampling_rate );
            PWMGenEnable( PWM0_BASE, PWM_GEN_1 );
            PWMGenIntTrigEnable( PWM0_BASE, PWM_GEN_1, PWM_TR_CNT_ZERO );

            ADCSequenceConfigure( ADC0_BASE, ADC_SEQUENCE_1, ADC_TRIGGER_PWM1 | ADC_TRIGGER_PWM_MOD0, ADC_SEQUENCE_1 );
            ADCSequenceStepConfigure( ADC0_BASE, ADC_SEQUENCE_1, 0, ADC_CTL_CH1 | ADC_CTL_END | ADC_CTL_IE );
            ADCIntRegister( ADC0_BASE, ADC_SEQUENCE_1, adc_driver_isr_1 );
            IntPrioritySet( INT_ADC0SS1, configKERNEL_INTERRUPT_PRIORITY );
            break;

        case ADC_SEQUENCE_2:
            ADCSequenceDisable( ADC0_BASE, ADC_SEQUENCE_2 );
            PWMGenConfigure( PWM0_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC );
            PWMGenPeriodSet( PWM0_BASE, PWM_GEN_2, get_pwm_clk() / config->sampling_rate );
            PWMGenEnable( PWM0_BASE, PWM_GEN_2 );
            PWMGenIntTrigEnable( PWM0_BASE, PWM_GEN_2, PWM_TR_CNT_ZERO );

            ADCSequenceConfigure( ADC0_BASE, ADC_SEQUENCE_2, ADC_TRIGGER_PWM2 | ADC_TRIGGER_PWM_MOD0, ADC_SEQUENCE_2 );
            ADCSequenceStepConfigure( ADC0_BASE, ADC_SEQUENCE_2, 0, ADC_CTL_CH2 | ADC_CTL_END | ADC_CTL_IE );
            ADCIntRegister( ADC0_BASE, ADC_SEQUENCE_2, adc_driver_isr_2 );
            IntPrioritySet( INT_ADC0SS2, configKERNEL_INTERRUPT_PRIORITY) ;
            break;

        default:
            // TODO Handle invalid sequence case -TVT
            break;

    }

    ADCIntClear( ADC0_BASE, config->adc_num );
    ADCIntEnable( ADC0_BASE, config->adc_num );
}

/**
* Initialize ADC module
*/
void adc_init()
{
    // Enable ADC and PWM
    SysCtlPeripheralEnable( SYSCTL_PERIPH_ADC0 );
    SysCtlPeripheralEnable( SYSCTL_PERIPH_PWM0 );

    // Setup GPIO
    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOE );
    GPIOPinTypeADC( GPIO_PORTE_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 );

    // Setup PWM Clock
    SysCtlPWMClockSet( SYSCTL_PWMDIV_64 );
}
/**
* Handles conversion between the PWM clock divider settings and the actual count value
*
* @return PWM clock frequency
*/
static uint32_t get_pwm_clk()
{
    uint32_t ret = 1;

    switch( SysCtlPWMClockGet() )
    {
        case SYSCTL_PWMDIV_1:
            ret = 1;

            break;

        case SYSCTL_PWMDIV_2:
            ret = 2;

            break;

        case SYSCTL_PWMDIV_4:
            ret = 4;

            break;

        case SYSCTL_PWMDIV_8:
            ret = 8;

            break;

        case SYSCTL_PWMDIV_16:
            ret = 16;

            break;

        case SYSCTL_PWMDIV_64:
            ret = 64;

            break;
    }

    return SysCtlClockGet() / ret;
}