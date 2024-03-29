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
u8 clockDivider = 2;
XScuGic InterruptController;

volatile u16 xadcBuffer[RX_BUFFER_SIZE];
volatile u32 xadcSampleCount = 0;
volatile float voltage;

u8 xadcInit(){
	u32 address, status, intrStatusValue;
	XSysMon_Config *SYSConfigPtr;
	XAdcPs_Config *XADCConfigPtr;

	SYSConfigPtr = XSysMon_LookupConfig(SYSMON_DEVICE_ID);
	if (SYSConfigPtr == NULL) {
		return XST_FAILURE;
	}

	XSysMon_CfgInitialize(SysMonInstPtr, SYSConfigPtr, SYSConfigPtr->BaseAddress);

	address = SYSConfigPtr->BaseAddress;
	printf("XADC using Sysmon -> base address %lx \n\r", address);


//	XADCConfigPtr = XAdcPs_LookupConfig(XPAR_XADC_WIZ_0_DEVICE_ID);
//	if (XADCConfigPtr == NULL) {
//			return XST_FAILURE;
//	}
//
//	XAdcPs_CfgInitialize(XADCInstPtr,XADCConfigPtr,XADCConfigPtr->BaseAddress);

	status = xadcSetupInterruptSystem(&InterruptController, &SysMonInst, INTR_ID);
		if (status != XST_SUCCESS) {
			return XST_FAILURE;
		}
	XSysMon_SetAlarmEnables(SysMonInstPtr, 0x0);
//	status = XSysMon_SelfTest(SysMonInstPtr);
//	if ( XST_SUCCESS != status ) {
//		printf("ADC self test failed\n\r");
//	}else{
//		printf("ADC self test pass");
//	}
    //address = XADCConfigPtr->BaseAddress;
    u8 mode = XSysMon_GetSequencerMode(SysMonInstPtr);
    status =  XSysMon_GetSeqInputMode(SysMonInstPtr);
    XSysMon_SetSeqAvgEnables(SysMonInstPtr, 0);

    XSysMon_GetStatus(SysMonInstPtr); /* Clear the old status */

    XSysMon_SetCalibEnables(SysMonInstPtr,XSM_CFR1_CAL_VALID_MASK );
    intrStatusValue = Xil_In16(SYSConfigPtr->BaseAddress+XSM_CFR0_OFFSET);
	printf("CFG0 %x \n",(unsigned int) intrStatusValue);
	intrStatusValue = Xil_In16(SYSConfigPtr->BaseAddress+XSM_CFR1_OFFSET);
	printf("CFG1 %x \n",(unsigned int)intrStatusValue);
	intrStatusValue = Xil_In16(SYSConfigPtr->BaseAddress+XSM_CFR2_OFFSET);
	printf("CFG2 %x \n",(unsigned int)intrStatusValue);
	intrStatusValue = Xil_In16(SYSConfigPtr->BaseAddress+XSM_IPIER_OFFSET);
	printf("Interrupt Enable register %x \n",(unsigned int)intrStatusValue);
	intrStatusValue = Xil_In16(SYSConfigPtr->BaseAddress+XSM_GIER_OFFSET);
	printf("Global Int Enable %x \n",(unsigned int)intrStatusValue);
	xadcSampleCount = 0;

	xadcSetSampleRate(100000);
	//clockDivider = XSysMon_GetAdcClkDivisor(SysMonInstPtr);

	xadcCheckAuxSettings();

	XSysMon_IntrEnable(SysMonInstPtr, XSM_IPIXR_EOC_MASK);
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

void checkForSamples(){
	if(xadcSampleCount > 0){

	}
}

void xadcSetPolarity(u8 setting){
	int status;
	//1 for bipolar, 0 for unipolar
	if(setting){
		status =  XSysMon_SetSeqInputMode(SysMonInstPtr, XSM_SEQ_CH_AUX14);
	}else{
		status =  XSysMon_SetSeqInputMode(SysMonInstPtr, 0);
	}
	if(status != XST_SUCCESS){
		xil_printf("Error changing polarity");
	}
	return;
}

u32 xadcGetSampleCount(){
	return xadcSampleCount;
}
u32* getBuffer(){
return xadcBuffer;
}

void xadcDisableSampling(){
	XSysMon_IntrGlobalDisable(&SysMonInst);
}

void xadcEnableSampling(){
	xil_printf("Starting sampling\n");
	XSysMon_IntrGlobalEnable(SysMonInstPtr);
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
/**
 * Rate should be given in SPS
 */
int xadcSetSampleRate(u32 rate){

	if(rate > 1000000){
		xil_printf("Invalid sample rate given");
		return -1;
	}
	u8 val = CLOCK_FREQ / (26.0 * rate);
	XSysMon_SetAdcClkDivisor(SysMonInstPtr, val);
    printf("New Sample Rate: %lf SPS\n", CLOCK_FREQ / (float) val / 26.0);
	return 0;
}

static int xadcSetupInterruptSystem(XScuGic *IntcInstancePtr,XSysMon *XAdcPtr, u16 IntrId ){

	XScuGic_Config *IntcConfig; /* Instance of the interrupt controller */
	int Status;

	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}
	Status = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig, IntcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	Status = XScuGic_Connect(IntcInstancePtr, IntrId,(Xil_InterruptHandler)XAdcInterruptHandler,(void *)XAdcPtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	XScuGic_Enable(IntcInstancePtr, IntrId);

	Xil_ExceptionInit();

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_FIQ_INT,
					(Xil_ExceptionHandler)
					XScuGic_InterruptHandler,
					IntcInstancePtr);

	Xil_ExceptionEnableMask(XIL_EXCEPTION_ALL);

	Xil_Out32(XPAR_PS7_SCUGIC_0_BASEADDR,0xF);

	return XST_SUCCESS;
}
void XAdcInterruptHandler(void *CallBackRef){


	if (xadcSampleCount < RX_BUFFER_SIZE ){
		xadcBuffer[xadcSampleCount] = (u16) XSysMon_GetAdcData(&SysMonInst, AUX_14_INPUT) >> 4;
		xadcSampleCount++;
	}else{
		printf("Done\n");
		XSysMon_IntrGlobalDisable(SysMonInstPtr);
	}

	XSysMon_IntrClear(&SysMonInst, XSM_IPIXR_EOC_MASK);
}
void xadcProcessSamples(){
	u32 i = 0;
	printf("Beginning sample playback..\n\n");
	sleep(2);
	while(i < xadcSampleCount){

		voltage = RawToExtVoltage(xadcBuffer[i]);
		if(xadcBuffer[i] > 0x800) { voltage -= 1.0; }
		 printf("%f %x\n", voltage, xadcBuffer[i] );
		i++;
	}
	xadcSampleCount = 0;

	printf("Finished processing samples\n\n");
}
