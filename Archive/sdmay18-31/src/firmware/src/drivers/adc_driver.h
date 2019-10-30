/**
* @file adc_driver.h
* @brief Header file for the ADC driver
*
* @author SDMAY18-31
*/

#ifndef __ADC_DRIVER_H__
#define __ADC_DRIVER_H__

/*-----------------------------------------------------------------
-                           Includes
-----------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "drivers/shared_defs.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"

/*-----------------------------------------------------------------
-                       Literal Constants
-----------------------------------------------------------------*/

/*-----------------------------------------------------------------
-                         Structures
-----------------------------------------------------------------*/
typedef struct adc_config_data_struct
{
    uint32_t        sampling_rate;
    adc_channel_e   adc_num;
    filters_e       filter_route;
} adc_config_data_t;

/*----------------------------------------------------------------
-                          Prototypes
-----------------------------------------------------------------*/
void adc_disable_dev( uint32_t adc_num );
void adc_enable_dev( uint32_t adc_num );
void adc_config( adc_config_data_t* config );
void adc_init();

#endif // __ADC_DRIVER_H__
