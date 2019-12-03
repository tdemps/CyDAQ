/*
 * xadc.h
 *
 *  Created on: Oct 29, 2019
 *      Author: tdempsay
 */

#ifndef SRC_XADC_H_
#define SRC_XADC_H_

#include <stdbool.h>
#include <stdint.h>
#include <xadcps_hw.h>
#include <xparameters.h>
#include <sleep.h>
#include "xadcps.h"
#include "xsysmon.h"
#include <stdio.h>
#include "xscugic.h"
#include "Xil_exception.h"
#include "xil_printf.h"
#include <stdlib.h>
/* Definitions for peripheral XADC_WIZ_0 */
#define XPAR_XADC_WIZ_0_DEVICE_ID 0U
#define XPAR_XADC_WIZ_0_INCLUDE_INTR 1U
#define SYSMON_DEVICE_ID	XPAR_SYSMON_0_DEVICE_ID
#define CLOCK_FREQ 100000000 //input clock to xadc wiz block (Mhz). Must be set in Vivado
#define RX_BUFFER_SIZE  10000
#define INTR_ID				XPS_IRQ_INT_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define AUX_14_INPUT (XADCPS_CH_AUX_MIN + 14)
#define SAMPLE_TYPE u16

#define RawToExtVoltage(AdcData) 					\
	(((float)(AdcData))/4096.0f)


u8 xadcInit();
int xadcFractionToInt(float FloatNum);
XSysMon* xadcGetSysMonPtr();
XAdcPs* xadcGetXADCPtr();
u8 xadcCheckAuxSettings();
int xadcSetSampleRate(u32 rate);
int xadcGetSampleRate();
static int xadcSetupInterruptSystem(XScuGic *IntcInstancePtr,XSysMon *XAdcPtr,u16 IntrId );
void XAdcInterruptHandler(void *CallBackRef);
void checkForSamples();
u32 xadcGetSampleCount();
u32* getBuffer();
void xadcEnableSampling(u8 streamSetting);
void xadcDisableSampling();
void xadcSetPolarity(u8 setting);
void xadcProcessSamples();
#endif /* SRC_XADC_H_ */
