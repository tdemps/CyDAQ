/**
* @file pot_driver.c
* @brief This file contains the functions needed to modify AD5254
* potentiometer wiper values
*
* @author SDMAY18-31
*/

/*-----------------------------------------------------------------
-                           Includes
-----------------------------------------------------------------*/
#include "drivers/pot_driver.h"

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
 * Tune potentiometer wiper value
 *
 * @param wiperLoc the wiper mux value and register address
 * @param wiperVal the value for the wiper to be set to
 *
 * @return POT_ERR_NONE on success and POT_ERR_FAILED on failure
 */
uint32_t pot_tune_wiper(wiper_location_t wiperLoc, uint8_t wiperVal)
{
    mux_write(MUX_CHIP_SEL, wiperLoc.ic);

    uint8_t data[2];
    data[0] = wiperLoc.wiper;
    data[1] = wiperVal;

    uint32_t err = i2c_send(POT_I2C_BASE, POT_IC_ADDRESS, data, 2);
    if(err)
    {
        return POT_ERR_FAILED;
    }

    err = i2c_send_byte(POT_I2C_BASE, POT_IC_ADDRESS, wiperLoc.wiper, I2C_MASTER_CMD_BURST_SEND_START);
    if(err)
    {
        return POT_ERR_FAILED;
    }

    uint8_t readBack;
    err = i2c_recieve(POT_I2C_BASE, POT_IC_ADDRESS, &readBack, 1);
    if(err || readBack != wiperVal)
    {
        return POT_ERR_FAILED;
    }


    return POT_ERR_NONE;
}