/**
* @file imu_driver.c
* @brief This file contains the functions needed to drive the MPU-9250
* IMU
*
* @author SDMAY18-31
*/

/*-----------------------------------------------------------------
-                           Includes
-----------------------------------------------------------------*/
#include "drivers/imu_driver.h"

/*-----------------------------------------------------------------
-                       Literal Constants
-----------------------------------------------------------------*/

/*-----------------------------------------------------------------
-                       Global Variables
-----------------------------------------------------------------*/
static uint32_t my_sqrt( uint32_t val );

/*-----------------------------------------------------------------
-                          Procedures
-----------------------------------------------------------------*/

/**
 * Get accelerometer data
 *
 * @param axis the axis requested
 * @param accel pointer to where the accelerometer data will be stored
 *
 * @return IMU_ERR_NONE on success and IMU_ERR_FAILED on failture
 */
uint32_t imu_get_accel(axis_e axis, int16_t* accel)
{
    uint8_t memAddr = 0;

    switch(axis)
    {
        case axis_x:
            memAddr = 59;
            break;

        case axis_y:
            memAddr = 61;
            break;

        case axis_z:
            memAddr = 63;
            break;
    }

    uint32_t err;
    err = i2c_send_byte(IMU_I2C_BASE, IMU_IC_ADDRESS, memAddr, I2C_MASTER_CMD_BURST_SEND_START);
    if(err)
    {
        return IMU_ERR_FAILED;
    }

    uint8_t accelRaw[2];
    err = i2c_recieve(IMU_I2C_BASE, IMU_IC_ADDRESS, accelRaw, 2);
    if(err)
    {
        return IMU_ERR_FAILED;
    }

    *accel = (int16_t)((accelRaw[0] << 8) | accelRaw[1]);
    return IMU_ERR_NONE;
}

/**
 * Get accelerometer magnetude
 *
 * @param mag pointer to where the accelerometer magnetude will be stored
 *
 * @return IMU_ERR_NONE on success and IMU_ERR_FAILED on failture
 */
uint32_t imu_get_accel_mag(uint16_t* mag)
{
    int16_t x, y, z;

    if(imu_get_accel(axis_x, &x))
    {
        return IMU_ERR_FAILED;
    }

    if(imu_get_accel(axis_y, &y))
    {
        return IMU_ERR_FAILED;
    }

    if(imu_get_accel(axis_z, &z))
    {
        return IMU_ERR_FAILED;
    }

    *mag = (uint16_t)(4095 * my_sqrt( (x * x) + (y * y) + (z * z) ) / 65535);

    return IMU_ERR_NONE;
}

/**
 * Get gyroscope data
 *
 * @param axis the axis requested
 * @param gyro pointer to where the gyroscope data will be stored
 *
 * @return IMU_ERR_NONE on success and IMU_ERR_FAILED on failture
 */
uint32_t imu_get_gyro(axis_e axis, int16_t* gyro)
{
    uint8_t memAddr = 0;

    switch(axis)
    {
        case axis_x:
            memAddr = 67;
            break;

        case axis_y:
            memAddr = 69;
            break;

        case axis_z:
            memAddr = 71;
            break;
    }

    uint32_t err;
    err = i2c_send_byte(IMU_I2C_BASE, IMU_IC_ADDRESS, memAddr, I2C_MASTER_CMD_BURST_SEND_START);
    if(err)
    {
        return IMU_ERR_FAILED;
    }

    uint8_t gyroRaw[2];
    err = i2c_recieve(IMU_I2C_BASE, IMU_IC_ADDRESS, gyroRaw, 2);
    if(err)
    {
        return IMU_ERR_FAILED;
    }

    *gyro = (int16_t)((gyroRaw[0] << 8) | gyroRaw[1]);
    return IMU_ERR_NONE;
}

/**
 * Get gyroscope magnetude
 *
 * @param mag pointer to where the gyroscope magnetude will be stored
 *
 * @return IMU_ERR_NONE on success and IMU_ERR_FAILED on failture
 */
uint32_t imu_get_gyro_mag(uint16_t* mag)
{
    int16_t x, y, z;

    if(imu_get_gyro(axis_x, &x))
    {
        return IMU_ERR_FAILED;
    }

    if(imu_get_gyro(axis_y, &y))
    {
        return IMU_ERR_FAILED;
    }

    if(imu_get_gyro(axis_z, &z))
    {
        return IMU_ERR_FAILED;
    }

    *mag = (uint16_t)(4095 * my_sqrt( (x * x) + (y * y) + (z * z) ) / 65535);

    return IMU_ERR_NONE;
}

/**
 * Get magnetometer data
 *
 * @param axis the axis requested
 * @param mag pointer to where the magnetometer data will be stored
 *
 * @return IMU_ERR_NONE on success and IMU_ERR_FAILED on failture
 */
uint32_t imu_get_mag(axis_e axis, int16_t* mag)
{
    uint8_t memAddr = 0;

    switch(axis)
    {
        case axis_x:
            memAddr = 3;
            break;

        case axis_y:
            memAddr = 5;
            break;

        case axis_z:
            memAddr = 7;
            break;
    }

    uint32_t err;
    err = i2c_send_byte(IMU_I2C_BASE, IMU_IC_ADDRESS, memAddr, I2C_MASTER_CMD_BURST_SEND_START);
    if(err)
    {
        return IMU_ERR_FAILED;
    }

    uint8_t magRaw[2];
    err = i2c_recieve(IMU_I2C_BASE, IMU_IC_ADDRESS, magRaw, 2);
    if(err)
    {
        return IMU_ERR_FAILED;
    }

    // convert from little endian to big endian
    magRaw[1] = ((magRaw[1] << 4) & 0xF0) | ((magRaw[1] >> 4) & 0x0F);
    magRaw[0] = ((magRaw[0] << 4) & 0xF0) | ((magRaw[0] >> 4) & 0x0F);

    *mag = (int16_t)((magRaw[1] << 8) | magRaw[0]);
    return IMU_ERR_NONE;
}

/**
 * Get magnetometer magnetude
 *
 * @param mag pointer to where the magnetometer magnetude will be stored
 *
 * @return IMU_ERR_NONE on success and IMU_ERR_FAILED on failture
 */
uint32_t imu_get_mag_mag(uint16_t* mag)
{
    int16_t x, y, z;

    if(imu_get_mag(axis_x, &x))
    {
        return IMU_ERR_FAILED;
    }

    if(imu_get_mag(axis_y, &y))
    {
        return IMU_ERR_FAILED;
    }

    if(imu_get_mag(axis_z, &z))
    {
        return IMU_ERR_FAILED;
    }

    *mag = (uint16_t)(4095 * my_sqrt( (x * x) + (y * y) + (z * z) ) / 65535);

    return IMU_ERR_NONE;
}

/**
 * Get temperature data
 *
 * @param temp pointer to where the temperature data will be stored
 *
 * @return IMU_ERR_NONE on success and IMU_ERR_FAILED on failture
 */
uint32_t imu_get_temp(uint16_t* temp)
{
    uint8_t memAddr = 65;

    uint32_t err;
    err = i2c_send_byte(IMU_I2C_BASE, IMU_IC_ADDRESS, memAddr, I2C_MASTER_CMD_BURST_SEND_START);
    if(err)
    {
        return IMU_ERR_FAILED;
    }

    uint8_t rawTemp[2];
    err = i2c_recieve(IMU_I2C_BASE, IMU_IC_ADDRESS, rawTemp, 2);
    if(err)
    {
        return IMU_ERR_FAILED;
    }

    *temp = 4095 * ((rawTemp[0] << 8) | rawTemp[1]) / 65535;
    return IMU_ERR_NONE;
}

/* Sqrt alg by Jack W. Crenshaw */
static uint32_t my_sqrt( uint32_t val )
{
    uint32_t root       = 0;
    uint32_t remainder  = val;
    uint32_t place      = 0x80000000;

    while( place > remainder )
    {
        place >>= 2;
    }

    while( 0 != place )
    {
        if( remainder >= ( root + place ) )
        {
            remainder = remainder - root - place;
            root = root + ( place << 1 );
        }
        root    = root >> 1;
        place   = place >> 2;
    }

    return root;
}