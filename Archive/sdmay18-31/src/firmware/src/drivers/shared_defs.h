/**
* @file shared_defs.h
* @brief Header file that contains definitions used by all drivers
*
* @author SDMAY18-31
*/

#ifndef __SHARED_DEFS_H__
#define __SHARED_DEFS_H__

/*-----------------------------------------------------------------
-                           Includes
-----------------------------------------------------------------*/
#include <stdint.h>

/*-----------------------------------------------------------------
-                         Enumerations
-----------------------------------------------------------------*/
typedef enum filters
{
    FILTER_1ST_ORDER_LP,
    FILTER_5TH_ORDER_LP,
    FILTER_1ST_ORDER_HP,
    FILTER_5TH_ORDER_HP,
    FILTER_2ND_ORDER_BP,
    FILTER_6TH_ORDER_BP,
    FILTER_60_HZ_NOTCH,
    FILTER_PASSTHROUGH,
    NUM_FILTERS
} filters_e;

typedef enum muxes
{
    MUX_ADC0,
    MUX_ADC1,
    MUX_ADC2,
    MUX_CHIP_SEL,
    NUM_MUXES
} muxes_e;

typedef enum adc_channel
{
    ADC_SEQUENCE_0  = 0,
    ADC_SEQUENCE_1  = 1,
    ADC_SEQUENCE_2  = 2,
    NUM_ADC_CHANNELS
} adc_channel_e;

/*  List of sensors, when adding a new digital sensor, make sure
    this list matches the f_sensors enum in the frontend. The
    "NONE" value does not need to be in this list. */
typedef enum sensors
{
    ADC0 = 0,
    ADC1 = 1,
    ADC2 = 2,
    IMU_ACCEL = 3,
    IMU_GYRO = 4,
    IMU_MAG = 5,
    IMU_TEMP = 6,
    NUM_SENSORS
} sensors_e;


#endif // __SHARED_DEFS_H__
