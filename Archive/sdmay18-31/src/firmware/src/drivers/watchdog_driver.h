/**
* @file watchdog_driver.h
* @brief Header file for the watchdog driver
*
* @author SDMAY18-31
*/

#ifndef __WATCHDOG_DRIVER_H__
#define __WATCHDOG_DRIVER_H__

/*-----------------------------------------------------------------
-                           Includes
-----------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/watchdog.h"

/*----------------------------------------------------------------
-                          Prototypes
-----------------------------------------------------------------*/
void watchdog_setup();

void watchdog_reset_countdown();

#endif // __WATCHDOG_DRIVER_H__
