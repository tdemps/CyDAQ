/*
 * x9258.c
 *
 *  Created on: Oct 8, 2019
 *      Author: tdempsay
 */

/*
 * Allows I2C communication with the x9258 digital potentiometer.
 */
#include "x9258.h"

static XIicPs I2C0_IIC;
u8 potInit = 0;

XStatus init_x9258_i2c(){
	if(DEBUG)
		xil_printf("Initializing I2C for X9258\n");
	XStatus status;
	XIicPs_Config *Config;

/*
	 * Initialize the IIC driver so that it's ready to use
	 * Look up the configuration in the config table, then initialize it.
	 */
	Config = XIicPs_LookupConfig(IIC_DEVICE_ID);
	if (NULL == Config) {
		return XST_FAILURE;
	}

	status = XIicPs_CfgInitialize(&I2C0_IIC, Config, Config->BaseAddress);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Perform a self-test to ensure that the hardware was built correctly.
	 */
	status = XIicPs_SelfTest(&I2C0_IIC);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Set the IIC serial clock rate.
	 */
	status = XIicPs_SetSClk(&I2C0_IIC, POT_I2C_CLK_RATE);
	potInit = 1;
	return status;
}
/**
 * Function used to convert resistance into the appropriate byte value
 * send to the X9258 IC's.
 */
POT_R_TYPE pot_value_conversion(int ohmValue){
  static const POT_R_TYPE calFactor = 0;

  if(ohmValue < 0 || ohmValue > 100000){
	if(DEBUG)
		xil_printf("Invalid resistance given, choose val between 0-100,000 ohm\n");
    return 0;
  }
  return map(ohmValue,0,100000,0,255) + calFactor;
}

XStatus x9258_volatile_write(wiper_t wiper_location, POT_R_TYPE r_value){

	if(potInit == 0){
		init_x9258_i2c();
	}
  	u8 SendBuffer[POT_I2C_BUFFER_SIZE];
  	SendBuffer[0] = (u8) (0b10100000 | wiper_location.wiper);
  	SendBuffer[1] = r_value;

  	u16 address = (u16) ((0b01010000 | wiper_location.ic_addr) >> 1);
  	/*
	 * Send the buffer using the IIC and ignore the number of bytes sent
	 * as the return value since we are using it in interrupt mode.
	 */
  	XStatus status = XIicPs_MasterSendPolled(&I2C0_IIC, SendBuffer, POT_I2C_BUFFER_SIZE,  address);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	if(DEBUG)
		xil_printf("Wrote to IC_Addr: %x,Pot#: %d,Reg Val: %d \n", wiper_location.ic_addr, wiper_location.wiper, r_value);

	/*
	 * Wait until bus is idle to start another transfer.
	 */
	while (XIicPs_BusIsBusy(&I2C0_IIC)) {
		/* NOP */
	}

	return XST_SUCCESS;
}
/**
 * Generic mapping function stolen from Arduino. Used to map value x from in_range
 * to out_range.
 */
long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
/**
 * Returns IICPS instance used for I2C comms with pots.
 */
XIicPs* getIicInstance(){
	return &I2C0_IIC;
}
/**
 * Old, left for archival purposes.
 */
void setIicInstance(XIicPs *InstancePtr){
	I2C0_IIC = *InstancePtr;
}


