/**
* @file mux_driver.c
* @brief This file contains the functions needed to drive the various
* muxes on the PCB
*
* @author SDMAY18-31
*/

/*-----------------------------------------------------------------
-                           Includes
-----------------------------------------------------------------*/
#include "drivers/mux_driver.h"

/*-----------------------------------------------------------------
-                       Literal Constants
-----------------------------------------------------------------*/

/*-----------------------------------------------------------------
-                       Global Variables
-----------------------------------------------------------------*/

/*-----------------------------------------------------------------
-                          Procedures
-----------------------------------------------------------------*/

/**
 * Set filter mux
 *
 * @param channel ADC channel
 * @param filter filter
 */
void mux_set_filter_route( adc_channel_e channel, filters_e filter )
{
    /*  Filter enum maps each filter to the appropriate pins.
        Channel enum will map to the muxes enum */
    mux_write( channel, filter );
}

/**
 * Sets mux selection. 0th bit of data corresponds to S0, 2nd bit of
 * data corresponds to S2
 *
 * @param mux the mux to set
 * @param the value to set the mux to
 */
void mux_write( muxes_e mux, uint8_t data )
{
    switch( mux )
    {
        case MUX_ADC0:
            GPIOPinWrite( GPIO_PORTB_BASE,
                        ( GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 ),
                        ( data & 0x7 ) );
            break;

        case MUX_ADC1:
            GPIOPinWrite( GPIO_PORTB_BASE,
                        ( GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 ),
                        ( ( data & 0x7 ) << 3 ) );

            break;

        case MUX_ADC2:
            GPIOPinWrite( GPIO_PORTB_BASE,
                        ( GPIO_PIN_6 | GPIO_PIN_7 ),
                        ( ( data & 0x3 ) << 6 ) );
            GPIOPinWrite( GPIO_PORTD_BASE, GPIO_PIN_1, ( ( data & 0x4 ) >> 2 ) );
            break;

        case MUX_CHIP_SEL:
            GPIOPinWrite( GPIO_PORTC_BASE,
                        ( GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 ),
                        ( ( data & 0x7 ) << 4 ) );
            break;

        default:
            break;
    }
}

/**
 * Initialize Mux Driver
 */
void mux_init()
{
    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOB );
    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOD );
    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOC );

    // Enable Pins for MUX_ADC0
    GPIOPinTypeGPIOOutput( GPIO_PORTB_BASE, ( GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 ) );

    // Enable Pins for MUX_ADC1
    GPIOPinTypeGPIOOutput( GPIO_PORTB_BASE, ( GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 ) );

    // Enable Pins for MUX_ADC2
    GPIOPinTypeGPIOOutput( GPIO_PORTB_BASE, ( GPIO_PIN_6 | GPIO_PIN_7 ) );
    GPIOPinTypeGPIOOutput( GPIO_PORTD_BASE, GPIO_PIN_0 );

    // Enable Pins for MUX_CHIP_SEL
    GPIOPinTypeGPIOOutput( GPIO_PORTC_BASE, ( GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 ) );
}