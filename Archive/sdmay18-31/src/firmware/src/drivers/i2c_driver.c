/**
* @file i2c_driver.c
* @brief This file contains the functions needed to drive the I2C
* module with FreeRTOS
*
* @author SDMAY18-31
*/

/*-----------------------------------------------------------------
-                           Includes
-----------------------------------------------------------------*/
#include "drivers/i2c_driver.h"

/*-----------------------------------------------------------------
-                       Literal Constants
-----------------------------------------------------------------*/
#define I2C_TIME_OUT 8000 // 20 ms delay

/*-----------------------------------------------------------------
-                       Global Variables
-----------------------------------------------------------------*/
static xSemaphoreHandle sem_i2c2_xfr_done;
static xSemaphoreHandle sem_i2c1_xfr_done;

static uint32_t i2c2_err;
static uint32_t i2c1_err;

/*-----------------------------------------------------------------
-                          Procedures
-----------------------------------------------------------------*/

/**
 * Initialize the I2C module
 */
void i2c_driver_init(void)
{
    //enable I2C modules
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C2);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);


    //reset modules
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C2);
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C1);


    //enable GPIO peripheral that contains I2C 2
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);


    // Configure the pin muxing for I2C2 functions on port PE4 and PE5.
    GPIOPinConfigure(GPIO_PE4_I2C2SCL);
    GPIOPinConfigure(GPIO_PE5_I2C2SDA);
    GPIOPinConfigure(GPIO_PA6_I2C1SCL);
    GPIOPinConfigure(GPIO_PA7_I2C1SDA);

    // Select the I2C function for these pins.
    GPIOPinTypeI2CSCL(GPIO_PORTE_BASE, GPIO_PIN_4);
    GPIOPinTypeI2C(GPIO_PORTE_BASE, GPIO_PIN_5);
    GPIOPinTypeI2CSCL(GPIO_PORTA_BASE, GPIO_PIN_6);
    GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_7);

    // Enable and initialize the I2C2 master module.  Use the system clock for
    // the I2C0 module.  The last parameter sets the I2C data transfer rate.
    // If false the data rate is set to 100kbps and if true the data rate will
    // be set to 400kbps.
    I2CMasterInitExpClk(I2C2_BASE, SysCtlClockGet(), true);
    I2CMasterInitExpClk(I2C1_BASE, SysCtlClockGet(), true);

    // Enable Interrupts
    IntMasterEnable();

    // Enable I2C Interrupts
    IntEnable(INT_I2C2);
    IntEnable(INT_I2C1);

    // Set I2C Timeout
    I2CMasterTimeoutSet(I2C2_BASE, I2C_TIME_OUT);
    I2CMasterTimeoutSet(I2C1_BASE, I2C_TIME_OUT);


    // Enable I2C interrupt
    I2CMasterIntEnableEx(I2C2_BASE, I2C_MASTER_INT_TIMEOUT
        | I2C_MASTER_INT_DATA);

    I2CMasterIntEnableEx(I2C1_BASE, I2C_MASTER_INT_TIMEOUT
        | I2C_MASTER_INT_DATA);

    // Clear any pendin(g interrupts
    I2CMasterIntClearEx(I2C2_BASE, I2CMasterIntStatusEx(I2C2_BASE, false));
    I2CMasterIntClearEx(I2C2_BASE, I2CMasterIntStatusEx(I2C1_BASE, false));


    // Register interrupt callback handler
    I2CIntRegister(I2C2_BASE, i2c2_int_handler);
    I2CIntRegister(I2C1_BASE, i2c1_int_handler);


    // Set interrupt priority as per free RTOS documentation
    IntPrioritySet(INT_I2C2, configMAX_SYSCALL_INTERRUPT_PRIORITY);
    IntPrioritySet(INT_I2C1, configMAX_SYSCALL_INTERRUPT_PRIORITY);

    sem_i2c2_xfr_done = xSemaphoreCreateBinary();
    sem_i2c1_xfr_done = xSemaphoreCreateBinary();
}

/**
 * ISR for I2C1
 * Check the status of the I2C module and act accordingly
 */
void i2c1_int_handler()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    i2c1_err = 0;
    // Check interrupt status
    uint32_t status = I2CMasterIntStatusEx(I2C1_BASE, true);

    if(status & I2C_MASTER_INT_TIMEOUT)
    {
        i2c1_err = I2C_MASTER_ERR_TIMEOUT;
        xSemaphoreGiveFromISR(sem_i2c1_xfr_done, &xHigherPriorityTaskWoken);
    }
    // Check for transmission finished
    else if(status & I2C_MASTER_INT_DATA)
    {
        xSemaphoreGiveFromISR(sem_i2c1_xfr_done, &xHigherPriorityTaskWoken);
    }

    // Clear interrupts
    I2CMasterIntClearEx(I2C1_BASE, status);

    // Check for errors
    i2c1_err |= I2CMasterErr(I2C1_BASE);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

}

/**
 * ISR for I2C2
 * Check the status of the I2C module and act accordingly
 */
void i2c2_int_handler()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    i2c2_err = 0;
    // Check interrupt status
    uint32_t status = I2CMasterIntStatusEx(I2C2_BASE, true);

    if(status & I2C_MASTER_INT_TIMEOUT)
    {
        i2c2_err = I2C_MASTER_ERR_TIMEOUT;
        xSemaphoreGiveFromISR(sem_i2c2_xfr_done, &xHigherPriorityTaskWoken);
    }
    // Check for transmission finished
    else if(status & I2C_MASTER_INT_DATA)
    {
        xSemaphoreGiveFromISR(sem_i2c2_xfr_done, &xHigherPriorityTaskWoken);
    }

    // Clear interrupts
    I2CMasterIntClearEx(I2C2_BASE, status);

    // Check for errors
    i2c2_err |= I2CMasterErr(I2C2_BASE);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**
 * Send a byte over I2C
 *
 * @param base the base address for the I2C module (I2C1_BASE or I2C2_BASE)
 * @param addr the address of the IC to send the data
 * @param data the data to transfer
 * @param cmd the I2C master command
 *
 * @return error definitions located in i2c.h
 */
uint32_t i2c_send_byte(uint32_t base, uint8_t addr, uint8_t data, uint32_t cmd)
{
    uint32_t* err;
    xSemaphoreHandle sem_i2c_xfr_done;

    if(base == I2C2_BASE)
    {
        err = &i2c2_err;
        sem_i2c_xfr_done = sem_i2c2_xfr_done;
    }
    else
    {
        err = &i2c1_err;
        sem_i2c_xfr_done = sem_i2c1_xfr_done;
    }

    // Set up transfer
    I2CMasterSlaveAddrSet(base, addr, false);
    I2CMasterDataPut(base, data);

    // Initiate transfer
    I2CMasterControl(base, cmd);

    // Block until transfer complete or error
    xSemaphoreTake(sem_i2c_xfr_done, portMAX_DELAY);

    return *err;
}

/**
 * Send data over I2C
 *
 * @param base the base address for the I2C module (I2C1_BASE or I2C2_BASE)
 * @param addr the address of the IC to send the data
 * @param data pointer to the data to transfer
 * @param dataSize length of the data
 *
 * @return error definitions located in i2c.h
 */
uint32_t i2c_send(uint32_t base, uint8_t addr, uint8_t* data, uint32_t dataSize)
{
    uint32_t* err;

    if(base == I2C2_BASE)
    {
        err = &i2c2_err;
    }
    else
    {
        err = &i2c1_err;
    }

    if(!(dataSize > 0))
    {
        return -1;
    }
    else if(dataSize == 1)
    {
        // Send single byte
        return i2c_send_byte(base, addr, data[0], I2C_MASTER_CMD_SINGLE_SEND);
    }

    //Send first byte
    i2c_send_byte(base, addr, data[0], I2C_MASTER_CMD_BURST_SEND_START);
    if(*err)
    {
        i2c_abort_on_err(base);
        return *err;
    }

    for(int i = 1; i < dataSize - 1; i++)
    {
        i2c_send_byte(base, addr, data[i], I2C_MASTER_CMD_BURST_SEND_CONT);
        if(*err)
        {
            i2c_abort_on_err(base);
            return *err;
        }
    }

    //Send last byte
    i2c_send_byte(base, addr, data[dataSize - 1], I2C_MASTER_CMD_BURST_SEND_FINISH);
    if(*err)
    {
        i2c_abort_on_err(base);
    }
    return *err;
}

/**
 * Recieve a byte over I2C
 *
 * @param base the base address for the I2C module (I2C1_BASE or I2C2_BASE)
 * @param addr the address of the IC to send the data
 * @param data the data to transfer
 * @param cmd the I2C master command
 *
 * @return error definitions located in i2c.h
 */
uint32_t i2c_recieve_byte(uint32_t base, uint8_t addr, uint8_t* data, uint32_t cmd)
{
    uint32_t* err;
    xSemaphoreHandle sem_i2c_xfr_done;

    if(base == I2C2_BASE)
    {
        err = &i2c2_err;
        sem_i2c_xfr_done = sem_i2c2_xfr_done;
    }
    else
    {
        err = &i2c1_err;
        sem_i2c_xfr_done = sem_i2c1_xfr_done;
    }

    // Set up transfer
    I2CMasterSlaveAddrSet(base, addr, true);
    I2CMasterControl(base, cmd);

    // Wait for transfer complete
    xSemaphoreTake(sem_i2c_xfr_done, portMAX_DELAY);

    // Get data
    *data = I2CMasterDataGet(base);
    return *err;
}

/**
 * Recieve data over I2C
 *
 * @param base the base address for the I2C module (I2C1_BASE or I2C2_BASE)
 * @param addr the address of the IC to send the data
 * @param data pointer to the data to transfer
 * @param dataSize length of the data
 *
 * @return error definitions located in i2c.h
 */
uint32_t i2c_recieve(uint32_t base, uint8_t addr, uint8_t* data, uint32_t dataSize)
{
    uint32_t* err;

    if(base == I2C2_BASE)
    {
        err = &i2c2_err;
    }
    else
    {
        err = &i2c1_err;
    }

    if(!(dataSize > 0))
    {
        return -1;
    }
    else if(dataSize == 1)
    {
        return i2c_recieve_byte(base, addr, data, I2C_MASTER_CMD_SINGLE_RECEIVE);
    }

    //Recieve first byte
    i2c_recieve_byte(base, addr, &(data[0]), I2C_MASTER_CMD_BURST_RECEIVE_START);
    if(*err)
    {
        i2c_abort_on_err(base);
        return *err;
    }

    for(int i = 1; i < dataSize - 1; i++)
    {
        i2c_recieve_byte(base, addr, &(data[i]), I2C_MASTER_CMD_BURST_RECEIVE_CONT);
        if(*err)
        {
            i2c_abort_on_err(base);
            return *err;
        }
    }

    //Recieve last byte
    i2c_recieve_byte(base, addr, &(data[dataSize - 1]), I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
    if(*err)
    {
        i2c_abort_on_err(base);
    }
    return *err;
}

/**
 * Abort the transfer upon an error in the I2C module
 *
 * @param base the base address for the I2C module (I2C1_BASE or I2C2_BASE)
 */
void i2c_abort_on_err(uint32_t base)
{
    I2CMasterControl(base, I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP);
}
