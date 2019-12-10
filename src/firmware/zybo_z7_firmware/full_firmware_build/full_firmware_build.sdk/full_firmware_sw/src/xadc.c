/*
 * xadc.c
 *
 *  Created on: Oct 29, 2019
 *      Author: tdempsay
 */
#include "xadc.h"

static XSysMon SysMonInst;
static u32 sampleRate = 0;
static XTmrCtr TimerCounterInst;
XSysMon* SysMonInstPtr = &SysMonInst;
u8 clockDivider = 2;
static XScuGic InterruptController;
volatile SAMPLE_TYPE xadcBuffer[RX_BUFFER_SIZE];
volatile u32 xadcSampleCount = 0;
volatile float voltage;
extern volatile bool samplingEnabled, streamingEnabled = false;

//#define INTC_DEVICE_ID		 	XPAR_INTC_0_DEVICE_ID
#define INTC_DEVICE_INT_ID	 	XPAR_INTC_0_TMRCTR_0_VEC_ID
#define TMRCTR_DEVICE_ID		XPAR_TMRCTR_0_DEVICE_ID
#define TIMER_CNTR_0	 		0
#define RESET_VALUE	 			0xF0000000



u8 xadcInit(){
	u32 address, status, intrStatusValue;
	XSysMon_Config *SYSConfigPtr;

	SYSConfigPtr = XSysMon_LookupConfig(SYSMON_DEVICE_ID);
	if (SYSConfigPtr == NULL) {
		return XST_FAILURE;
	}

	XSysMon_CfgInitialize(SysMonInstPtr, SYSConfigPtr, SYSConfigPtr->BaseAddress);

	address = SYSConfigPtr->BaseAddress;
	xil_printf("XADC using Sysmon -> base address %lx \n\r", address);

	status = xadcSetupInterruptSystem(&InterruptController, &SysMonInst, XADC_INTR_ID);
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
    XSysMon_SetSequencerMode(SysMonInstPtr, XSM_SEQ_MODE_SAFE);
    status =  XSysMon_GetSeqInputMode(SysMonInstPtr);
    XSysMon_SetSeqAvgEnables(SysMonInstPtr, 0);

    XSysMon_GetStatus(SysMonInstPtr); /* Clear the old status */

    XSysMon_SetCalibEnables(SysMonInstPtr,XSM_CFR1_CAL_VALID_MASK );
    intrStatusValue = Xil_In16(SYSConfigPtr->BaseAddress+XSM_CFR0_OFFSET);
	printf("CFG0 %x, ",(unsigned int) intrStatusValue);
	intrStatusValue = Xil_In16(SYSConfigPtr->BaseAddress+XSM_CFR1_OFFSET);
	printf("CFG1 %x, ",(unsigned int)intrStatusValue);
	intrStatusValue = Xil_In16(SYSConfigPtr->BaseAddress+XSM_CFR2_OFFSET);
	printf("CFG2 %x\n",(unsigned int)intrStatusValue);
//	intrStatusValue = XSysMon_IntrGetEnabled(SysMonInstPtr);
//	printf("Interrupt Enable register %x \n",(unsigned int)intrStatusValue);
//	intrStatusValue = Xil_In32(SYSConfigPtr->BaseAddress+XSM_GIER_OFFSET);
//	printf("Global Int Enable %x \n",(unsigned int)intrStatusValue);
	xadcSampleCount = 0;

	xadcSetSampleRate(45000);
	//clockDivider = XSysMon_GetAdcClkDivisor(SysMonInstPtr);
	XSysMon_SetSequencerMode(SysMonInstPtr, XSM_SEQ_MODE_CONTINPASS);
	xadcCheckAuxSettings();
	XSysMon_IntrEnable(SysMonInstPtr, XSM_IPIXR_EOS_MASK);

	status = XTmrCtr_Initialize(&TimerCounterInst, TMRCTR_DEVICE_ID);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	status = XTmrCtr_SelfTest(&TimerCounterInst,TIMER_CNTR_0);
		if (status != XST_SUCCESS) {
			return XST_FAILURE;
		}

	XTmrCtr_SetHandler(&TimerCounterInst,DeviceDriverHandler,&TimerCounterInst);
	XTmrCtr_SetOptions(&TimerCounterInst, TIMER_CNTR_0, XTC_INT_MODE_OPTION);
	XTmrCtr_SetResetValue(&TimerCounterInst, TIMER_CNTR_0, RESET_VALUE);
	XTmrCtr_Start(&TimerCounterInst, TIMER_CNTR_0);

	return 0;
}

XSysMon* xadcGetSysMonPtr(){
	return SysMonInstPtr;
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
	 xil_printf("AUX6");
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
		status = XSysMon_SetSeqInputMode(SysMonInstPtr, XSM_SEQ_CH_AUX14);
	}else{
		status = XSysMon_SetSeqInputMode(SysMonInstPtr, 0);
	}
	if(status != XST_SUCCESS){
		xil_printf("Error changing polarity");
	}
	return;
}

u32 xadcGetSampleCount(){
	return xadcSampleCount;
}
SAMPLE_TYPE* xadcGetBuffer(){
	return xadcBuffer;
}

void xadcDisableSampling(){
	XSysMon_IntrGlobalDisable(&SysMonInst);
	samplingEnabled = false;

}

void xadcEnableSampling(u8 streamSetting){
	xil_printf("Starting sampling, Streaming: %s\n", (streamSetting) ? "On" : "Off");
	samplingEnabled = true;
	u32 numBytes = 0;
	u8 buf[50];
	//XSysMon_IntrGlobalEnable(SysMonInstPtr);
	if(streamSetting == 1){
		u32 cursor = 0;
		while(samplingEnabled){
			streamingEnabled = true;
			numBytes += comUartRecv(&buf[numBytes], 5);
			while ((XSysMon_GetStatus(&SysMonInst) & XSM_SR_EOS_MASK) != XSM_SR_EOS_MASK);
			xadcBuffer[xadcSampleCount] = (SAMPLE_TYPE) XSysMon_GetAdcData(&SysMonInst, AUX_14_INPUT) >> 4;
			if(xadcSampleCount == RX_BUFFER_SIZE) { xadcSampleCount = 0; }
			voltage = RawToExtVoltage(xadcBuffer[xadcSampleCount]);
			xadcSampleCount++;
			//xil_printf("@%x, %d!\n", xadcBuffer[cursor], xadcSampleCount);
			xil_printf("%d.%d\n", (int)voltage, xadcFractionToInt(voltage));
			if(buf[numBytes-1] == '!'){
				xil_printf("Stopping\n");
				samplingEnabled = false;
			}
//			if(cursor+10 <= xadcSampleCount){
//				voltage = RawToExtVoltage(xadcBuffer[cursor]);
//				//xil_printf("@%x, %d!\n", xadcBuffer[cursor], xadcSampleCount);
//				xil_printf("%d.%d\n", (int)voltage, xadcFractionToInt(voltage));
//				cursor = xadcSampleCount;
//			}
		}
	}

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

	if(rate > XADC_MAX_SAMPLE_RATE){
		xil_printf("Invalid sample rate given");
		return -1;
	}
	u8 val = CLOCK_FREQ / (26.0 * rate);
	XSysMon_SetAdcClkDivisor(SysMonInstPtr, val);
    printf("New Sample Rate: %.2lf SPS\n", CLOCK_FREQ / (float) val / 26.0);
	return 0;
}

int xadcGetSampleRate(){
	return -1;
}



static int xadcSetupInterruptSystem(XScuGic *IntcInstancePtr,XSysMon *XAdcPtr, u16 IntrId ){

	XScuGic_Config *IntcConfig; /* Instance of the interrupt controller */
	int status;

	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}
	status = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig, IntcConfig->CpuBaseAddress);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	status = XScuGic_Connect(IntcInstancePtr, IntrId,(Xil_InterruptHandler)xadcInterruptHandler,(void *)XAdcPtr);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	status = XScuGic_Connect(IntcInstancePtr, XPS_FIQ_INT_ID,
					   (Xil_InterruptHandler)DeviceDriverHandler,
					   (void *)&TimerCounterInst);
	if (status != XST_SUCCESS) {
			return XST_FAILURE;
	}
	XScuGic_Enable(IntcInstancePtr, IntrId);
	XScuGic_Enable(IntcInstancePtr, XPS_FIQ_INT_ID); //timer
	Xil_ExceptionInit();

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_FIQ_INT,
					(Xil_ExceptionHandler)
					XScuGic_InterruptHandler,
					IntcInstancePtr);

	Xil_ExceptionEnableMask(XIL_EXCEPTION_ALL);

	Xil_Out32(XPAR_PS7_SCUGIC_0_BASEADDR,0xF);

	return XST_SUCCESS;
}
void xadcInterruptHandler(void *CallBackRef){

	if (xadcSampleCount < RX_BUFFER_SIZE ){
		xadcBuffer[xadcSampleCount] = (SAMPLE_TYPE) XSysMon_GetAdcData(&SysMonInst, AUX_14_INPUT) >> 4;
		xadcSampleCount++;
	}else if(streamingEnabled == true){
		xadcSampleCount = 0;
	}else{
		xil_printf("Done\n");
		samplingEnabled = false;
		XSysMon_IntrGlobalDisable(SysMonInstPtr);
	}

	XSysMon_IntrClear(&SysMonInst, XSM_IPIXR_EOS_MASK);
}
void xadcProcessSamples(){
	u32 i = 0;
	if(xadcSampleCount == 0){
		xil_printf("No new samples\n");
		return;
	}
	xil_printf("Beginning sample playback..\n\n");
	sleep(2);

	while(i < xadcSampleCount){

		voltage = RawToExtVoltage(xadcBuffer[i]);
		 xil_printf("%d: %d.%d\n", i,(int)voltage, xadcFractionToInt(voltage));
		i++;
	}
	xadcSampleCount = 0;

	xil_printf("Finished processing samples\n\n");
}

void DeviceDriverHandler(void *CallbackRef){
	print("Timer Event\n\r");
	XTmrCtr_SetResetValue(&TimerCounterInst, TIMER_CNTR_0, RESET_VALUE);
	XTmrCtr_Start(&TimerCounterInst, TIMER_CNTR_0);

}
