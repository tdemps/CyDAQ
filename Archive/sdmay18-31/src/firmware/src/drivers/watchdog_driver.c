/**
* @file watchdog_driver.c
* @brief This file contains the functions needed drive the watchdog
* timer
*
* @author SDMAY18-31
*/

/*-----------------------------------------------------------------
-                           Includes
-----------------------------------------------------------------*/
#include "drivers/watchdog_driver.h"

/*-----------------------------------------------------------------
-                       Literal Constants
-----------------------------------------------------------------*/
#define WATCHDOG_TIME_SECONDS       10
#define WATCHDOG_COUNTER_LOAD_VAL   ( WATCHDOG_TIME_SECONDS * SysCtlClockGet() )

/*-----------------------------------------------------------------
-                       Global Variables
-----------------------------------------------------------------*/

/*-----------------------------------------------------------------
-                          Procedures
-----------------------------------------------------------------*/

/**
 * Setup watchdog timer
 */
void watchdog_setup()
{
    SysCtlPeripheralEnable( SYSCTL_PERIPH_WDOG0 );
    WatchdogReloadSet( WATCHDOG0_BASE, WATCHDOG_COUNTER_LOAD_VAL );
    WatchdogResetEnable( WATCHDOG0_BASE );
    WatchdogEnable( WATCHDOG0_BASE );
}

/**
 * Reset watchdog countdown
 */
void watchdog_reset_countdown()
{
    WatchdogReloadSet( WATCHDOG0_BASE, WATCHDOG_COUNTER_LOAD_VAL );
}