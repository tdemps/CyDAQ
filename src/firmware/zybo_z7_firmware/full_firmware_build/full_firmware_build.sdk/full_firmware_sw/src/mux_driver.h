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

#define SELECT_TYPE u8
#define NUM_SELECT_PINS 3
#define ENABLE_PIN 12U //JF4,     Enable Pin for Muxes
#define HL_TOGGLE_PIN 13U //JF5,  High/Low toggle pin for filter outputs

int muxInit();
void muxSetOutputPins( SELECT_TYPE hlToggle, SELECT_TYPE muxSelects, SELECT_TYPE ePin);
void muxSetInputPins( SELECT_TYPE hlToggle, SELECT_TYPE muxSelects, SELECT_TYPE ePin);


#endif /* SRC_MUX_DRIVER_H_ */
