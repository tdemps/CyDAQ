/*
 * xadc.h
 *
 *  Created on: Oct 29, 2019
 *      Author: tdempsay
 */

#ifndef SRC_XADC_H_
#define SRC_XADC_H_

#include <xadcps_hw.h>
#include <xparameters.h>
#include <sleep.h>
#include "xadcps.h"
#include "xsysmon.h"
#include <stdio.h>
/* Definitions for peripheral XADC_WIZ_0 */
#define XPAR_XADC_WIZ_0_IP_TYPE 0U
#define XPAR_XADC_WIZ_0_DEVICE_ID 0U
#define BASEADDR 0x43C00000U
#define XPAR_XADC_WIZ_0_HIGHADDR 0x43C0FFFFU
#define XPAR_XADC_WIZ_0_INCLUDE_INTR 1U
#define SYSMON_DEVICE_ID	XPAR_SYSMON_0_DEVICE_ID

#define RawToExtVoltage(AdcData) 					\
	(((float)(AdcData))/65536.0f)

u8 xadcInit();
int xadcFractionToInt(float FloatNum);
XSysMon* xadcGetSysMonPtr();
XAdcPs* xadcGetXADCPtr();
u8 xadcCheckAuxSettings();
#endif /* SRC_XADC_H_ */
