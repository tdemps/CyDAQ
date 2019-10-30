/**
* @file digital_sensor_task.c
* @brief This file contains the task responsible for retrieving
* data from a digital sensor and putting that data in the data
* queue.
*
* @author SDMAY18-31
*/

/*-----------------------------------------------------------------
-                           Includes
-----------------------------------------------------------------*/
#include "digital_sensor_task.h"

/*-----------------------------------------------------------------
-                       Literal Constants
-----------------------------------------------------------------*/
#define DIGITALSENSORTASKSTACKSIZE 256         // Stack size in words

/*-----------------------------------------------------------------
-                       Global Variables
-----------------------------------------------------------------*/
extern QueueHandle_t        data_queue;      // Queue for loading packets from ADC isrs
extern xSemaphoreHandle     mutex_digital_state;  // Mutex protecting UART TX
extern bool                 collecting;     // Condition variable for if device is collecting
extern digital_config_t*    pDigitalConfig;   // Config for digital sensors
extern TaskHandle_t         digitalSensorTaskHandle;

/*-----------------------------------------------------------------
-                          Procedures
-----------------------------------------------------------------*/

/**
* This function is used to create the digital sensor task
*
* @return 0 on success and 1 on failure
*/
uint32_t digital_sensor_task_init(void)
{
    if( xTaskCreate( digital_sensor_task,
    (const portCHAR *)"Digital Sensor",
    DIGITALSENSORTASKSTACKSIZE, NULL,
    tskIDLE_PRIORITY + PRIORITY_DIGITAL_SENSOR_TASK, &digitalSensorTaskHandle) != pdTRUE )
    {
        return 1;
    }

    return 0;
}

/**
* This task is responsible for retrieving data from a digital sensor
* and putting that data in the data queue
*
* @param params unused
*/
void digital_sensor_task(void* params)
{
    TickType_t delay = pdMS_TO_TICKS(500);
    sensors_e sensor = 0;
    bool enabled = 0;
    bool collecting = 0;

    for(;;)
    {
        vTaskDelay(delay);
        xSemaphoreTake(mutex_digital_state, portMAX_DELAY);

        sensor = pDigitalConfig->sensor;
        delay = pdMS_TO_TICKS(1000 / pDigitalConfig->sampleRate);
        enabled = pDigitalConfig->enabled;
        collecting = pDigitalConfig->collecting;

        xSemaphoreGive(mutex_digital_state);
        if(!collecting || !enabled)
        {
            continue;
        }

        switch(sensor)
        {
            case ADC0:
            case ADC1:
            case ADC2:
            {
                break;
            }
            case IMU_ACCEL:
            {
                uint16_t accel;
                if(!imu_get_accel_mag(&accel))
                {
                    accel |= (IMU_ACCEL << 12);
                    xQueueSendToBack(data_queue, &accel, 0);
                }

                //TODO abort on error?

                break;
            }
            case IMU_GYRO:
            {
                uint16_t gyro;
                if(!imu_get_gyro_mag(&gyro))
                {
                    gyro |= (IMU_GYRO << 12);
                    if(xQueueSendToBack(data_queue, &gyro, 0) != pdTRUE)
                    {
                        break;
                    }
                }

                //TODO abort on error?

                break;
            }
            case IMU_MAG:
            {
                uint16_t mag;
                if(!imu_get_mag_mag(&mag))
                {
                    mag |= (IMU_MAG << 12);
                    if(xQueueSendToBack(data_queue, &mag, 0) != pdTRUE)
                    {
                        break;
                    }
                }

                //TODO abort on error?

                break;
            }
            case IMU_TEMP:
            {
                uint16_t temp;
                if(!imu_get_temp(&temp))
                {
                    temp |= (IMU_TEMP << 12);
                    if(xQueueSendToBack(data_queue, &temp, 0) != pdTRUE)
                    {
                        break;
                    }
                }

                //TODO abort on error?

                break;
            }
            case NUM_SENSORS:
            {
                break;
            }
        }

    }
}
