/**
* @file digital_sensor_task.h
* @brief Header file for the digital sensor task
*
* @author SDMAY18-31
*/

#ifndef __DIGITAL_SENSOR_TASK__
#define __DIGITAL_SENSOR_TASK__

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
#include "uart_rx_task.h"

#include "drivers/imu_driver.h"
#include "drivers/shared_defs.h"

/*----------------------------------------------------------------
-                          Gloabals
-----------------------------------------------------------------*/

/*----------------------------------------------------------------
-                          Prototypes
-----------------------------------------------------------------*/
uint32_t digital_sensor_task_init(void);
void digital_sensor_task(void* params);

#endif // __DIGITAL_SENSOR_TASK__
