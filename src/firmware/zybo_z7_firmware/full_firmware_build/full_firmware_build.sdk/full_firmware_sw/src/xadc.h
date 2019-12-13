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
#include <xsysmon.h>
#include <stdio.h>
#include <xscugic.h>
#include <Xil_exception.h>
#include <xil_printf.h>
#include <stdlib.h>
#include <xtmrctr.h>
#include <math.h>
#include "comm.h"

/* Definitions for peripheral XADC_WIZ_0 */
#define XPAR_XADC_WIZ_0_DEVICE_ID 0U
#define XPAR_XADC_WIZ_0_INCLUDE_INTR 1U
#define SYSMON_DEVICE_ID	XPAR_SYSMON_0_DEVICE_ID
#define CLOCK_FREQ 100000000 //input clock to xadc wiz block (Mhz). Must be set in Vivado
#define RX_BUFFER_SIZE  3000000
#define XADC_MAX_SAMPLE_RATE 50000
#define XADC_DEFAULT_RATE 20000
#define XADC_INTR_ID				XPS_IRQ_INT_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define AUX_14_INPUT (XSM_CH_AUX_MIN + 14)
#define SAMPLE_TYPE u16

/**
 * Function that converts raw 12bit sample to voltage (float)
 */
#define RawToExtVoltage(AdcData) 					\
	(((float)(AdcData))/4096.0f)


u8 xadcInit();
int xadcFractionToInt(float FloatNum);
XSysMon* xadcGetSysMonPtr();
u8 xadcCheckAuxSettings();
int xadcSetSampleRate(u32 rate);
u32 xadcGetSampleRate();
static int xadcSetupInterruptSystem(XScuGic *IntcInstancePtr,XSysMon *XAdcPtr,u16 IntrId );
void xadcInterruptHandler(void *CallBackRef);
u32 xadcGetSampleCount();
SAMPLE_TYPE* xadcGetBuffer();
void xadcEnableSampling(u8 streamSetting);
void xadcDisableSampling();
void xadcSetPolarity(u8 setting);
void xadcProcessSamples();
#endif /* SRC_XADC_H_ */
