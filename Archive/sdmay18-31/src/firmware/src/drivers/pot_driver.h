/**
* @file pot_driver.h
* @brief Header file for the potentiometer driver
*
* @author SDMAY18-31
*/

#ifndef __POT_DRIVER_H__
#define __POT_DRIVER_H__

/*-----------------------------------------------------------------
-                           Includes
-----------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

#include "drivers/mux_driver.h"
#include "drivers/i2c_driver.h"

/*-----------------------------------------------------------------
-                       Literal Constants
-----------------------------------------------------------------*/
#define POT_I2C_BASE I2C2_BASE
#define POT_IC_ADDRESS  0x2C

// Potentiometer error status
#define POT_ERR_NONE    0x00
#define POT_ERR_FAILED  0x01

/*-----------------------------------------------------------------
-                         Enumerations
-----------------------------------------------------------------*/

/*-----------------------------------------------------------------
-                         Structures
-----------------------------------------------------------------*/
typedef struct _wiper_location_t
{
    uint8_t ic;         // ic number corresponds to potentiometer ic muxing select pins
    uint8_t wiper;      // wiper corresponds to the wiper on a given ic
} wiper_location_t;

/*----------------------------------------------------------------
-                          Prototypes
-----------------------------------------------------------------*/
uint32_t pot_tune_wiper(wiper_location_t wiperLoc, uint8_t wiperVal);

#endif // __POT_DRIVER_H__
