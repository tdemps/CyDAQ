/**
* @file mux_driver.h
* @brief Header file for the mux driver
*
* @author SDMAY18-31
*/

#ifndef __MUX_DRIVER_H__
#define __MUX_DRIVER_H__

/*-----------------------------------------------------------------
-                           Includes
-----------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"

#include "drivers/shared_defs.h"

/*-----------------------------------------------------------------
-                       Literal Constants
-----------------------------------------------------------------*/

/*-----------------------------------------------------------------
-                         Structures
-----------------------------------------------------------------*/

/*----------------------------------------------------------------
-                          Prototypes
-----------------------------------------------------------------*/
void mux_set_filter_route( adc_channel_e channel, filters_e filter );
void mux_write( muxes_e mux, uint8_t data );
void mux_init();

#endif // __MUX_DRIVER_H__
