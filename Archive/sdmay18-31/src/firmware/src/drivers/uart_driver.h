/**
* @file uart_driver.h
* @brief Header file for the uart driver
*
* @author SDMAY18-31
*/

#ifndef __UART_DRIVER_H__
#define __UART_DRIVER_H__

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
#include "driverlib/uart.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/uart.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"

#include "drivers/adc_driver.h"
#include "communications.h"

#define     CRC_SIZE 2

/*----------------------------------------------------------------
-                          Prototypes
-----------------------------------------------------------------*/
void        uart_driver_init();
void        uart_int_handler();
bool        uart_send_char( char c );
uint8_t     uart_get_char();
bool        uart_get_rx_pkt( rx_cmd_pkt_t* packet );
bool        uart_send_string( uint8_t* data, int size );
void        uart_block_for_rx();
uint16_t    uart_calc_crc( uint8_t* data, int size );

#endif // __UART_DRIVER_H__
