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

int muxInit();
void muxSetPins( SELECT_TYPE hlToggle, SELECT_TYPE muxSelects, SELECT_TYPE ePin);


#endif /* SRC_MUX_DRIVER_H_ */
