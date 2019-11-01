/*
 * xadc.c
 *
 *  Created on: Oct 29, 2019
 *      Author: tdempsay
 */
#include "xadc.h"

static XAdcPs  XADCMonInst;
static XSysMon SysMonInst;


XAdcPs *XADCInstPtr = &XADCMonInst;
XSysMon* SysMonInstPtr = &SysMonInst;

u8 xadcInit(){
	u32 address, status;
	XSysMon_Config *SYSConfigPtr;
	XAdcPs_Config *XADCConfigPtr;

	SYSConfigPtr = XSysMon_LookupConfig(SYSMON_DEVICE_ID);
	if (SYSConfigPtr == NULL) {
		return XST_FAILURE;
	}

	XSysMon_CfgInitialize(SysMonInstPtr, SYSConfigPtr, SYSConfigPtr->BaseAddress);

	address = SYSConfigPtr->BaseAddress;
	printf("XADC using Sysmon -> base address %lx \n\r", address);


	XADCConfigPtr = XAdcPs_LookupConfig(XPAR_XADC_WIZ_0_DEVICE_ID);
	if (XADCConfigPtr == NULL) {
			return XST_FAILURE;
	}

	XAdcPs_CfgInitialize(XADCInstPtr,XADCConfigPtr,XADCConfigPtr->BaseAddress);

	status = XSysMon_SelfTest(SysMonInstPtr);
	if ( XST_SUCCESS != status ) {
		printf("ADC self test failed\n\r");
	}
	u16 alarms = XAdcPs_GetAlarmEnables(XADCInstPtr);
	printf("Alarms: %X\n\r", alarms);
    address = XADCConfigPtr->BaseAddress;
    u8 mode = XSysMon_GetSequencerMode(SysMonInstPtr);
    status =  XSysMon_GetSeqInputMode(SysMonInstPtr);
    u32 avgEnables = XAdcPs_GetSeqAvgEnables(XADCInstPtr);
	/* Set the Calibration enables.
	*/
	XSysMon_SetCalibEnables(SysMonInstPtr, XSM_CFR1_CAL_PS_GAIN_OFFSET_MASK | XSM_CFR1_CAL_ADC_GAIN_OFFSET_MASK);
    printf("XADC using XADPS -> base address %lx, Mode: %x\n\r", address, mode);
	xadcCheckAuxSettings();

	return 0;
}

XSysMon* xadcGetSysMonPtr(){
	return SysMonInstPtr;
}

XAdcPs* xadcGetXADCPtr(){
	return XADCInstPtr;
}

u8 xadcCheckAuxSettings(){
	u64 channelConfig = XSysMon_GetSeqChEnables(SysMonInstPtr);
	//0x040000900 AUX14 VPVN and Temp sensors
	xil_printf("AUX Inputs Active: ");
	if (XSM_SEQ_CH_AUX14 & channelConfig){
	 xil_printf("AUX14, ");
	}
	if(XSM_SEQ_CH_AUX15 & channelConfig){
	 xil_printf("AUX15, ");
	}
	if(XSM_SEQ_CH_AUX07 & channelConfig){
	 xil_printf("AUX7, ");
	}
	if(XSM_SEQ_CH_AUX06 & channelConfig){
	 xil_printf("AUX6, ");
	}
	xil_printf("\n\r");

	return 0;
}


/****************************************************************************/
/**
*
* This function converts the fraction part of the given floating point number
* (after the decimal point)to an integer.
*
* @param	FloatNum is the floating point number.
*
* @return	Integer number to a precision of 3 digits.
*
* @note
* This function is used in the printing of floating point data to a STDIO device
* using the xil_printf function. The xil_printf is a very small foot-print
* printf function and does not support the printing of floating point numbers.
*
*****************************************************************************/
int xadcFractionToInt(float FloatNum){
	float Temp;

	Temp = FloatNum;
	if (FloatNum < 0) {
		Temp = -(FloatNum);
	}

	return( ((int)((Temp -(float)((int)Temp)) * (1000.0f))));
}
