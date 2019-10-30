/**
* @file uart_rx_task.h
* @brief Header file for the UART RX task
*
* @author SDMAY18-31
*/

#ifndef __UART_RX_TASK_H__
#define __UART_RX_TASK_H__

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

#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "driverlib/crc.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "inc/hw_memmap.h"

#include "communications.h"
#include "digital_sensor_task.h"
#include "drivers/uart_driver.h"
#include "drivers/adc_driver.h"
#include "drivers/mux_driver.h"
#include "drivers/filter_driver.h"
#include "drivers/watchdog_driver.h"

/*----------------------------------------------------------------
-                          Structs
-----------------------------------------------------------------*/
typedef struct _digital_config_t
{
    sensors_e sensor;
    uint32_t sampleRate;
    bool enabled;
    bool collecting;
} digital_config_t;

/*----------------------------------------------------------------
-                          Prototypes
-----------------------------------------------------------------*/
uint32_t uart_rx_task_init(void);

#endif // __UART___UART_RX_TASK_H__TX_TASK_H__
