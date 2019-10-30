/**
* @file i2c_driver.h
* @brief Header file for the I2C driver
*
* @author SDMAY18-31
*/

#ifndef __I2C_DRIVER_H__
#define __I2C_DRIVER_H__

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


#include "driverlib/i2c.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/uart.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"

/*-----------------------------------------------------------------
-                       Literal Constants
-----------------------------------------------------------------*/
#define I2C_MASTER_ERR_TIMEOUT 0x00000500

/*----------------------------------------------------------------
-                          Prototypes
-----------------------------------------------------------------*/
void i2c_driver_init(void);
void i2c2_int_handler(void);
void i2c1_int_handler(void);
uint32_t i2c_send_byte(uint32_t base, uint8_t addr, uint8_t data, uint32_t cmd);
uint32_t i2c_send(uint32_t base, uint8_t addr, uint8_t* data, uint32_t dataSize);
uint32_t i2c_recieve_byte(uint32_t base, uint8_t addr, uint8_t* data, uint32_t cmd);
uint32_t i2c_recieve(uint32_t base, uint8_t addr, uint8_t* data, uint32_t dataSize);
void i2c_abort_on_err(uint32_t base);

#endif // __I2C_DRIVER_H__
