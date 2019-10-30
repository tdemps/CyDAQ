/**
* @file filter_driver.h
* @brief Header file for the filter driver
*
* @author SDMAY18-31
*/

#ifndef __FILTER_DRIVER_H__
#define __FILTER_DRIVER_H__

/*-----------------------------------------------------------------
-                           Includes
-----------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

#include "drivers/pot_driver.h"
#include "drivers/shared_defs.h"
#include "drivers/filters.h"

/*-----------------------------------------------------------------
-                       Literal Constants
-----------------------------------------------------------------*/
#define FILTER_TUNE_SUCCESS 0x00
#define FILTER_TUNE_FAILED  0x01

/*-----------------------------------------------------------------
-                       Memory Constants
-----------------------------------------------------------------*/

static const wiper_location_t firstOrderLPF[] = {
    //filter tuning wipers
    //ic    wiper
    {0x00,  0x00}
};

static const wiper_location_t fifthOrderLPF[] = {
    //filter tuning wipers
    //ic    wiper
    {0x00,  0x01},
    {0x00,  0x02},
    {0x00,  0x03},
    {0x01,  0x00},
    {0x01,  0x01}
};

static const wiper_location_t firstOrderHPF[] = {
    //filter tuning wipers
    //ic    wiper
    {0x01,  0x02},
    //dc offset tuning wipers
    //ic    wiper
    {0x01,  0x03}
};

static const wiper_location_t fifthOrderHPF[] = {
    //filter tuning wipers
    //ic    wiper
    {0x02,  0x00},
    {0x02,  0x01},
    {0x02,  0x02},
    {0x02,  0x03},
    {0x03,  0x00},
    //dc offset tuning wipers
    //ic    wiper
    {0x03,  0x01}
};

static const wiper_location_t secondOrderBPF[] = {
    //filter tuning wipers
    //ic    wiper
    {0x03,  0x02},
    {0x03,  0x03},
    //dc offset tuning wipers
    //ic    wiper
    {0x06,  0x00}
};

static const wiper_location_t sixthOrderBPF[] = {
    //filter tuning wipers
    //ic    wiper
    {0x04,  0x01},
    {0x04,  0x02},
    {0x05,  0x00},
    {0x05,  0x01},
    {0x05,  0x02},
    {0x05,  0x03},
    //dc offset tuning wipers
    //ic    wiper
    {0x06,  0x01},
    {0x06,  0x03},
    {0x06,  0x02}
};

/*----------------------------------------------------------------
-                          Prototypes
-----------------------------------------------------------------*/
uint32_t filter_tune_first_order_LPF(uint16_t cutoff);
uint32_t filter_tune_fifth_order_LPF(uint16_t cutoff);
uint32_t filter_tune_first_order_HPF(uint16_t cutoff);
uint32_t filter_tune_fifth_order_HPF(uint16_t cutoff);
uint32_t filter_tune_second_order_BPF(uint16_t cutoff);
uint32_t filter_tune_sixth_order_BPF(uint16_t cutoff);
uint8_t findFilterIndex(uint16_t cutoff, const filter_t* filterVals, uint8_t numFreq);
uint16_t abs_uint_diff(uint16_t a, uint16_t b);


#endif // __FILTER_DRIVER_H__
