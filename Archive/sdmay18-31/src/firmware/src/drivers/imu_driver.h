/**
* @file imu_driver.h
* @brief Header file for the IMU driver
*
* @author SDMAY18-31
*/

#ifndef __IMU_DRIVER_H__
#define __IMU_DRIVER_H__

/*-----------------------------------------------------------------
-                           Includes
-----------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

#include "drivers/i2c_driver.h"

/*-----------------------------------------------------------------
-                       Literal Constants
-----------------------------------------------------------------*/
#define IMU_I2C_BASE I2C1_BASE
#define IMU_IC_ADDRESS  0x68

// IMCU error status
#define IMU_ERR_NONE    0x00
#define IMU_ERR_FAILED  0x01

/*-----------------------------------------------------------------
-                         Enumerations
-----------------------------------------------------------------*/
typedef enum _axis_e
{
    axis_x,
    axis_y,
    axis_z
} axis_e;

/*-----------------------------------------------------------------
-                         Structures
-----------------------------------------------------------------*/

/*----------------------------------------------------------------
-                          Prototypes
-----------------------------------------------------------------*/
uint32_t imu_get_accel(axis_e axis, int16_t* accel);
uint32_t imu_get_accel_mag(uint16_t* mag);
uint32_t imu_get_gyro(axis_e axis, int16_t* gyro);
uint32_t imu_get_gyro_mag(uint16_t* mag);
uint32_t imu_get_mag(axis_e axis, int16_t* mag);
uint32_t imu_get_mag_mag(uint16_t* mag);
uint32_t imu_get_temp(uint16_t* temp);

#endif // __IMU_DRIVER_H__
