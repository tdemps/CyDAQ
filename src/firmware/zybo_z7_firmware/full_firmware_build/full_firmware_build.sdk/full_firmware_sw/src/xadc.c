/*
 * xadc.c
 *
 *  Created on: Oct 29, 2019
 *      Author: tdempsay
 */
#include "xadc.h"

static XSysMon SysMonInst;
static u32 xadcSampleRate = 0, resetValue = 0xFD050E58; //current sample rate (samples/s) and reset value for timer
static XTmrCtr TimerCounterInst;
XSysMon* SysMonInstPtr = &SysMonInst;
u8 initialized = 0; //whether XADC has been initialized
static XScuGic InterruptController;
volatile SAMPLE_TYPE xadcBuffer[RX_BUFFER_SIZE];
volatile u32 xadcSampleCount = 0;
volatile float voltage;

extern volatile bool samplingEnabled, streamingEnabled = false;

//#define INTC_DEVICE_ID		 	XPAR_INTC_0_DEVICE_ID
#define INTC_DEVICE_INT_ID	 	XPAR_INTC_0_TMRCTR_0_VEC_ID
#define TMRCTR_DEVICE_ID		XPAR_TMRCTR_0_DEVICE_ID
#define TIMER_CNTR_0	 		0


/**
 * Initializes XADC
 */
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
	status = XSysMon_SelfTest(SysMonInstPtr);
	if ( XST_SUCCESS != status ) {
		printf("ADC self test failed\n\r");
	}else{
		printf("ADC self test pass");
	}
	//disables sequencer so we can configure it
    XSysMon_SetSequencerMode(SysMonInstPtr, XSM_SEQ_MODE_SAFE);
    status =  XSysMon_GetSeqInputMode(SysMonInstPtr);
    //disables averaging on all inputs
    XSysMon_SetSeqAvgEnables(SysMonInstPtr, 0);
    //clear the old status
    XSysMon_GetStatus(SysMonInstPtr);
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


	//set sequencer to continually sample enabled inputs
	XSysMon_SetSequencerMode(SysMonInstPtr, XSM_SEQ_MODE_CONTINPASS);
	//status = XSysMon_GetSeqInputMode(SysMonInstPtr);
    //disables calibration
	XSysMon_SetCalibEnables(SysMonInstPtr, XSM_CFR1_CAL_DISABLE_MASK);
	//disables averaging
	XSysMon_SetAvg(SysMonInstPtr, XSM_AVG_0_SAMPLES);
	if(DEBUG)
		xadcCheckAuxSettings();


	//these initializes timer which controls sample rate
	status = XTmrCtr_Initialize(&TimerCounterInst, TMRCTR_DEVICE_ID);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	status = XTmrCtr_SelfTest(&TimerCounterInst,TIMER_CNTR_0);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XTmrCtr_SetOptions(&TimerCounterInst, TIMER_CNTR_0, XTC_INT_MODE_OPTION);
	//set to default sample rate
	xadcSetSampleRate(XADC_DEFAULT_RATE);
	xadcSampleCount = 0;
	samplingEnabled = false;
	streamingEnabled = false;
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
	 xil_printf("AUX14 ");
	}
	if(XSM_SEQ_CH_AUX15 & channelConfig){
	 xil_printf("AUX15 ");
	}
	if(XSM_SEQ_CH_AUX07 & channelConfig){
	 xil_printf("AUX7 ");
	}
	if(XSM_SEQ_CH_AUX06 & channelConfig){
	 xil_printf("AUX6");
	}
	xil_printf("\n\r");

	return 0;
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
/**
 * Disables sampling.
 */
void xadcDisableSampling(){
	XTmrCtr_Stop(&TimerCounterInst, TIMER_CNTR_0);
	//XSysMon_IntrGlobalDisable(&SysMonInst);
	samplingEnabled = false;
	xadcSampleCount = 0;
}
/**
 * Enables storing/printing of samples from XADC
 */
void xadcEnableSampling(u8 streamSetting){
	//checks that XADC has been initialized
	if(initialized == 0){
		xadcInit();
	}
	if(DEBUG)
		xil_printf("Starting sampling, Streaming: %s\n", (streamSetting) ? "On" : "Off");
	samplingEnabled = true;
	//number of bytes received over UART
	u32 numBytes = 0;
	//holds bytes received over UART
	u8 buf[50];
	//Starts timer to begin sampling
	XTmrCtr_Start(&TimerCounterInst, TIMER_CNTR_0);
	//XSysMon_IntrGlobalEnable(SysMonInstPtr);
	if(streamSetting == 1){
		streamingEnabled = true;
	}
		while(samplingEnabled){
			//checks for bytes received on uart
			numBytes += comUartRecv(&buf[numBytes], 6);
			//waits for xadc to finish conversion of sample
			while ((XSysMon_GetStatus(&SysMonInst) & XSM_SR_EOS_MASK) != XSM_SR_EOS_MASK);
			//restarts timer for next sample.
			XTmrCtr_Start(&TimerCounterInst, TIMER_CNTR_0);
			//stores raw 12bit sample (shifted right 4).
			xadcBuffer[xadcSampleCount] = (SAMPLE_TYPE) XSysMon_GetAdcData(&SysMonInst, AUX_14_INPUT) >> 4;
			voltage = RawToExtVoltage(xadcBuffer[xadcSampleCount]);

			//xil_printf("@%x, %d!", xadcBuffer[cursor], xadcSampleCount);
			if(streamingEnabled)
				xil_printf("%d.%d, %u\n", (int)voltage, xadcFractionToInt(voltage), xadcBuffer[xadcSampleCount]);
			if(numBytes > 2 && buf[numBytes-1] == '!' && buf[0] == '@'){
				if(DEBUG)
					xil_printf("Stopping, %d\n", xadcSampleCount);
				streamingEnabled = samplingEnabled = false;
				buf[0] = buf[numBytes-1] = '\0';
			}//else if(numBytes > 2){
			//	numBytes = 0;
			//}
			if(xadcSampleCount == RX_BUFFER_SIZE-1){
					xadcSampleCount = 0;
			}else{
					xadcSampleCount++;
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
	float Temp = FloatNum;
	if (FloatNum < 0) {
		Temp = -(FloatNum);
	}

	return( ((int)((Temp -(float)((int)Temp)) * (1000.0f))));
}
/**
 * Rate should be given in SPS. Configures timer to count at rate that matches sample rate.
 */
int xadcSetSampleRate(u32 rate){


	if(rate > XADC_MAX_SAMPLE_RATE){
		if(DEBUG)
			xil_printf("Invalid sample rate given");
		return 1;
	}
	xadcSampleRate = rate;
	double period = (double) 1 / rate;
	resetValue = ((period * CLOCK_FREQ) - pow(2, 32) - 1) * -1.0;

	XTmrCtr_Stop(&TimerCounterInst, TIMER_CNTR_0);
	XTmrCtr_SetResetValue(&TimerCounterInst, TIMER_CNTR_0, (u32) resetValue);

	//u8 val = CLOCK_FREQ / (26.0 * rate);
	//XSysMon_SetAdcClkDivisor(SysMonInstPtr, val);
	if(DEBUG)
		xil_printf("New Sample Rate: %d SPS\n", rate);
	return 0;
}
/**
 * Returns current sample rate
 */
u32 xadcGetSampleRate(){
	return xadcSampleRate;
}
/**
 * Sets up interrupt for XADC to handler function. (NOT USED CURRENTLY, POLLED INSTEAD)
 */
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

	XScuGic_Enable(IntcInstancePtr, IntrId);
	Xil_ExceptionInit();

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_FIQ_INT,
					(Xil_ExceptionHandler)
					XScuGic_InterruptHandler,
					IntcInstancePtr);

	Xil_ExceptionEnableMask(XIL_EXCEPTION_ALL);

	Xil_Out32(XPAR_PS7_SCUGIC_0_BASEADDR,0xF);
	//enables interrupt for end of sequence on XADC
	XSysMon_IntrEnable(SysMonInstPtr, XSM_IPIXR_EOS_MASK);

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
