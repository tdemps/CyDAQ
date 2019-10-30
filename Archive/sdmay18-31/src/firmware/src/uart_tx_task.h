/**
* @file uart_tx_task.h
* @brief Header file for the UART TX task
*
* @author SDMAY18-31
*/

#ifndef __UART_TX_TASK_H__
#define __UART_TX_TASK_H__

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
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "inc/hw_memmap.h"
#include "drivers/uart_driver.h"
#include "drivers/watchdog_driver.h"

#include "communications.h"

/*----------------------------------------------------------------
-                          Prototypes
-----------------------------------------------------------------*/
uint32_t uart_tx_task_init(void);

#endif // __UART_TX_TASK_H__
