/*
 * mux_driver.h
 *
 *  Created on: Oct 15, 2019
 *      Author: tdempsay
 */

#ifndef SRC_MUX_DRIVER_H_
#define SRC_MUX_DRIVER_H_

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include <xparameters.h>
#include <xgpiops.h>
#include "shared_definitions.h"

#define GPIO_DEV_ID XPAR_XGPIOPS_0_DEVICE_ID
#define MUX_SELECT_TYPE u8
#define NUM_SELECT_PINS 3
#define ENABLE_PIN 12U //JF4,     Enable Pin for Muxes
#define HL_TOGGLE_PIN 13U //JF5,  High/Low toggle pin for filter outputs
#define MUX_SET_FAILED 1
int muxInit();
u8 muxSetActiveFilter( filters_e filterSelect);
u8 muxSetInputPins( inputs_e inputSelect);

typedef struct{
	MUX_SELECT_TYPE hlToggle;
	MUX_SELECT_TYPE muxSelectPins;
	MUX_SELECT_TYPE enablePin;
} mux_config_data_t;


#endif /* SRC_MUX_DRIVER_H_ */
