/*-----------------------------------------------------------------
-                           Includes
-----------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

#include "driverlib/rom.h"
#include "driverlib/sysctl.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "uart_tx_task.h"
#include "uart_rx_task.h"

#include "drivers/i2c_driver.h"
#include "drivers/mux_driver.h"
#include "drivers/watchdog_driver.h"
#include "drivers/imu_driver.h"

/*----------------------------------------------------------------
-                          Prototypes
-----------------------------------------------------------------*/

/*-----------------------------------------------------------------
-                       Literal Constants
-----------------------------------------------------------------*/

/*-----------------------------------------------------------------
-                       Memory Constants
-----------------------------------------------------------------*/

/*-----------------------------------------------------------------
-                       Global Variables
-----------------------------------------------------------------*/
QueueHandle_t       data_queue;             // Queue for loading packets from ADC isrs
xSemaphoreHandle    mutex_uart_tx;          // Mutex protecting UART TX
xSemaphoreHandle    mutex_digital_state;    // Mutex to enable and disable digital sensor collection
uint32_t*           digital_state;          // State to enable and disable digital sensor collection
bool                collecting;             // Condition variable for if device is collecting
TaskHandle_t        digitalSensorTaskHandle;

/*-----------------------------------------------------------------
-                          Procedures
-----------------------------------------------------------------*/
void test_i2c_task(void* params);

int main( void )
{
    ROM_SysCtlClockSet( ( SYSCTL_SYSDIV_2_5 ) |
                        ( SYSCTL_USE_PLL    ) |
                        ( SYSCTL_XTAL_16MHZ ) |
                        ( SYSCTL_OSC_MAIN   ) );


    // Create Queue for ADC data
    data_queue       = xQueueCreate( 500, sizeof( uint16_t ) );

    // Create Mutex for protecting UART TX
    mutex_uart_tx   = xSemaphoreCreateMutex();

      // Create Mutex for protecting Digital sensor task
    mutex_digital_state   = xSemaphoreCreateMutex();

    // Create state variable for if the device is actively collecting data
    collecting      = false;

    // Non-task Relted setup
    i2c_driver_init();
    mux_init();
    adc_init();
    watchdog_setup();

    // Create Digital Sensor task
    if( 0 != digital_sensor_task_init() )
    {
        for(;;);
    }

    // Create UART RX task
    if( 0 != uart_rx_task_init() )
    {
        for(;;);
    }

    // Create UART TX task
    if( 0 != uart_tx_task_init() )
    {
        for(;;);
    }

    vTaskStartScheduler();

    for(;;);

}

#ifdef DEBUG
    void __error__( char *pcFilename, uint32_t ui32Line )
    {

    }
#endif

void vApplicationStackOverflowHook( xTaskHandle *pxTask, char *pcTaskName )
{
    while( 1 )
    {
    }
}