/*
 * comm.c
 *
 *Library for configuring and using the UART to communicate
 *with the frontend.
 *  Created on: Oct 16, 2019
 *      Author: tdempsay
 */

#include "comm.h"


static XUartPs UART1;
static INTC INTERRUPTCONTROLLER;

static u8 receiveBuffer[TEST_BUFFER_SIZE];	/* Buffer for Receiving Data */

/*
 * The following counters are used to determine when the entire buffer has
 * been sent and received.
 */
volatile int TotalReceivedCount;
volatile int TotalSentCount;
volatile bool fifoFlag;
extern bool samplingEnabled;
extern filters_e activeFilter;
int TotalErrorCount;

int commInit(){

	int status = 0;
	XUartPs_Config *Config;
	u32 IntrMask;
	fifoFlag = false;

	Config = XUartPs_LookupConfig(UART_DEVICE_ID);

	if (NULL == Config) {
		return XST_FAILURE;
	}

	status = XUartPs_CfgInitialize(&UART1, Config, Config->BaseAddress);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Connect the UART to the interrupt subsystem such that interrupts
	 * can occur. This function is application specific.
	 */
	//status = commSetupInterruptSystem(&INTERRUPTCONTROLLER, &UART1, UART_INT_IRQ_ID);
//	if (status != XST_SUCCESS) {
//		return XST_FAILURE;
//	}
	/*
	 * Setup the handlers for the UART that will be called from the
	 * interrupt context when data has been sent and received, specify
	 * a pointer to the UART driver instance as the callback reference
	 * so the handlers are able to access the instance data
	 */
	//XUartPs_SetHandler(&UART1, (XUartPs_Handler)Handler, &UART1);

	/*
	 * Enable the interrupt of the UART so interrupts will occur, setup
	 * a local loopback so data that is sent will be received.
	 */
//	IntrMask =
//		XUARTPS_IXR_TOUT | XUARTPS_IXR_PARITY | XUARTPS_IXR_FRAMING |
//		XUARTPS_IXR_OVER | XUARTPS_IXR_RXFULL |
//		XUARTPS_IXR_RXOVR;
//
//	if (UART1.Platform == XPLAT_ZYNQ_ULTRA_MP) {
//		IntrMask |= XUARTPS_IXR_RBRK;
//	}
//
//	XUartPs_SetInterruptMask(&UART1, IntrMask);

	/*
	 * Set the receiver timeout. If it is not set, and the last few bytes
	 * of data do not trigger the over-water or full interrupt, the bytes
	 * will not be received. By default it is disabled.
	 *
	 * The setting of 8 will timeout after 8 x 4 = 32 character times.
	 * Increase the time out value if baud rate is high, decrease it if
	 * baud rate is low.
	 */
	//XUartPs_SetRecvTimeout(&UART1, 16);
	//void XUartPs_SetFifoThreshold(XUartPs *InstancePtr, u8 TriggerLevel)

	/* Set the UART in Normal Mode */
	XUartPs_SetOperMode(&UART1, XUARTPS_OPER_MODE_NORMAL);
	XUartPs_SetBaudRate(&UART1, COMM_BAUD_RATE);

	return XST_SUCCESS;
}


XUartPs* commGetUartPtr(){
	return &UART1;
}
/**************************************************************************/
/**
*
* This function is the handler which performs processing to handle data events
* from the device.  It is called from an interrupt context. so the amount of
* processing should be minimal.
*
* This handler provides an example of how to handle data for the device and
* is application specific.
*
* @param	CallBackRef contains a callback reference from the driver,
*		in this case it is the instance pointer for the XUartPs driver.
* @param	Event contains the specific kind of event that has occurred.
* @param	EventData contains the number of bytes sent or received for sent
*		and receive events.
*
* @return	None.
*
* @note		None.
*
***************************************************************************/
void Handler(void *CallBackRef, u32 Event, unsigned int EventData)
{
	/* All of the data has been sent */
	if (Event == XUARTPS_EVENT_SENT_DATA) {
		TotalSentCount = EventData;
	}

	/* All of the data has been received */
	if (Event == XUARTPS_EVENT_RECV_DATA) {
		TotalReceivedCount = EventData;
		xil_printf("Received\n");
	}

	/*
	 * Data was received, but not the expected number of bytes, a
	 * timeout just indicates the data stopped for 8 character times
	 */
	if (Event == XUARTPS_EVENT_RECV_TOUT) {
		TotalReceivedCount = EventData;
	}

	/*
	 * Data was received with an error, keep the data but determine
	 * what kind of errors occurred
	 */
	if (Event == XUARTPS_EVENT_RECV_ERROR) {
		TotalReceivedCount = EventData;
		TotalErrorCount++;
	}

	/*
	 * Data was received with an parity or frame or break error, keep the data
	 * but determine what kind of errors occurred. Specific to Zynq Ultrascale+
	 * MP.
	 */
	if (Event == XUARTPS_EVENT_PARE_FRAME_BRKE) {
		TotalReceivedCount = EventData;
		TotalErrorCount++;
	}

	/*
	 * Data was received with an overrun error, keep the data but determine
	 * what kind of errors occurred. Specific to Zynq Ultrascale+ MP.
	 */
	if (Event == XUARTPS_EVENT_RECV_ORERR) {
		TotalReceivedCount = EventData;
		TotalErrorCount++;
	}
}

/*****************************************************************************/
/**
*
* This function sets up the interrupt system so interrupts can occur for the
* Uart. This function is application-specific. The user should modify this
* function to fit the application.
*
* @param	IntcInstancePtr is a pointer to the instance of the INTC.
* @param	UartInstancePtr contains a pointer to the instance of the UART
*		driver which is going to be connected to the interrupt
*		controller.
* @param	UartIntrId is the interrupt Id and is typically
*		XPAR_<UARTPS_instance>_INTR value from xparameters.h.
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
* @note		None.
*
****************************************************************************/
static int commSetupInterruptSystem(INTC *IntcInstancePtr, XUartPs *UartInstancePtr, u16 UartIntrId)
{
	int Status;

#ifdef XPAR_INTC_0_DEVICE_ID
#ifndef TESTAPP_GEN
	/*
	 * Initialize the interrupt controller driver so that it's ready to
	 * use.
	 */
	Status = XIntc_Initialize(IntcInstancePtr, INTC_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
#endif
	/*
	 * Connect the handler that will be called when an interrupt
	 * for the device occurs, the handler defined above performs the
	 * specific interrupt processing for the device.
	 */
	Status = XIntc_Connect(IntcInstancePtr, UartIntrId,
		(XInterruptHandler) XUartPs_InterruptHandler, UartInstancePtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

#ifndef TESTAPP_GEN
	/*
	 * Start the interrupt controller so interrupts are enabled for all
	 * devices that cause interrupts.
	 */
	Status = XIntc_Start(IntcInstancePtr, XIN_REAL_MODE);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
#endif
	/*
	 * Enable the interrupt for uart
	 */
	XIntc_Enable(IntcInstancePtr, UartIntrId);

	#ifndef TESTAPP_GEN
	/*
	 * Initialize the exception table.
	 */
	Xil_ExceptionInit();

	/*
	 * Register the interrupt controller handler with the exception table.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
				(Xil_ExceptionHandler) XIntc_InterruptHandler,
				IntcInstancePtr);
	#endif
#else
#ifndef TESTAPP_GEN
	XScuGic_Config *IntcConfig; /* Config for interrupt controller */

	/* Initialize the interrupt controller driver */
	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig,
					IntcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Connect the interrupt controller interrupt handler to the
	 * hardware interrupt handling logic in the processor.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
				(Xil_ExceptionHandler) XScuGic_InterruptHandler,
				IntcInstancePtr);
#endif

	/*
	 * Connect a device driver handler that will be called when an
	 * interrupt for the device occurs, the device driver handler
	 * performs the specific interrupt processing for the device
	 */
	Status = XScuGic_Connect(IntcInstancePtr, UartIntrId,
				  (Xil_ExceptionHandler) XUartPs_InterruptHandler,
				  (void *) UartInstancePtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/* Enable the interrupt for the device */
	XScuGic_Enable(IntcInstancePtr, UartIntrId);

#endif
#ifndef TESTAPP_GEN
	/* Enable interrupts */
	 Xil_ExceptionEnable();
#endif

	return XST_SUCCESS;
}
/**
 * Main task for processing commands received over UART
 */
void commRXTask(){
	//number of bytes received over UART
	u16 bytesReceived = 0;
	//for error handling
	bool err = false;
	while(1){

		while(receiveBuffer[bytesReceived-1] != COMM_STOP_CHAR && bytesReceived < 7){
			bytesReceived += XUartPs_Recv(&UART1, &receiveBuffer[bytesReceived], 1);
		}
		if(DEBUG)
			xil_printf("Processing packet\n");
		if(samplingEnabled == true){
			xadcDisableSampling();
			//stop sampling to config device
		}
		err = commProcessPacket(receiveBuffer, bytesReceived);
		receiveBuffer[0] = receiveBuffer[bytesReceived-1] = 0;
		bytesReceived = 0;
		if(err == true){
			xil_printf("%cERR%c", COMM_START_CHAR, COMM_STOP_CHAR);
		}else{
			xil_printf("%cACK%c", COMM_START_CHAR, COMM_STOP_CHAR);
		}
		err = false;


	}
}

/**
 * Processes the packet received from the UART.
 */
bool commProcessPacket(u8 *buffer, u16 bufSize){

	bool err = false;
	u8 cmd, status = 0;

	if((char) buffer[0] != COMM_START_CHAR || (char) buffer[bufSize-1] != COMM_STOP_CHAR || bufSize <= 2){
		err = true;
	}else{
		cmd = buffer[1]; ///needed for testing ======================
		//payload length is buffer size - start/stop chars - cmd_length
		u8 payloadLength = bufSize - 2 - COMM_CMD_SIZE;
		if(DEBUG)
			xil_printf("CMD: %u, payloadLen: %d\n", cmd, payloadLength);
		if(cmd == SAMPLE_RATE_SET){
			//set sample rate
			if(payloadLength < COMM_SAMPLE_RATE_SIZE){
				if(DEBUG)
					xil_printf("Error, not enough bytes to represent sample rate\n");
				err = true;
			}else{
				u32 rate = buffer[2] << 24 | buffer[3] << 16 | buffer[4] << 8 | buffer[5];
				xadcSetSampleRate(rate);
			}

		}else if(cmd == INPUT_SELECT){
			if(payloadLength == 0){
				if(DEBUG)
					xil_printf("Error, payload length too small\n");
				err = true;
			}else{
				status = muxSetActiveFilter(buffer[2]);
				if(status > 0){
					err = true;
				}
			}
			//input set
		}else if(cmd == FILTER_SELECT){
			//checks that payload contains filter number and that it is a
			if(payloadLength < 1){
				err = true;
				if(DEBUG)
					xil_printf("No filter param given to set\n");
			}else{
				err = muxSetActiveFilter((u8) buffer[COMM_CMD_SIZE+1]);
			}
			//set active filter
		}else if(cmd == CORNER_FREQ_SET){
			//u8 filter = ((buffer[COMM_CMD_SIZE+1] << 8) & 0xFF00 ) + buffer[COMM_CMD_SIZE+2];
			if(payloadLength < 4){
				if(DEBUG)
					xil_printf("err in filter tune function");
				err = true;
			}else{
				//each frequency should be sent as two bytes each
				FILTER_FREQ_TYPE lower = ((buffer[COMM_CMD_SIZE+1] << 8) & 0xFF00 ) + buffer[COMM_CMD_SIZE+2];
				FILTER_FREQ_TYPE upper = ((buffer[COMM_CMD_SIZE+3] << 8) & 0xFF00 ) + buffer[COMM_CMD_SIZE+4];
				//tune filter
				err = tuneFilter(50, lower, upper);
			}
		}else if(cmd == PING){
			//sets err to false so zybo sents the ACK back to GUI, confirming operation
			err = false;
		}else if(cmd == FETCH_SAMPLES){
			xadcProcessSamples();
		}else if(cmd == START_SAMPLING){
			xil_printf("%cACK%c", COMM_START_CHAR, COMM_STOP_CHAR);
			err = false;
			if(payloadLength == 1)
				xadcEnableSampling(1);
			else
				xadcEnableSampling(0);
		}else if(cmd == STOP_SAMPLING){
			xadcDisableSampling();
			//stop sampling
		}else{
			err = true;
		}
	}


	return err;

}

/**
 * Exposes Uart Receive to libraries without them needing UART ptr.
 */
u32 comUartRecv(u8 *bufferPtr, u32 numBytes)
{
	return XUartPs_Recv(&UART1, bufferPtr, numBytes);
}

u32 commUartSend(u8 *bufferPtr, u32 numBytes)
{
	return XUartPs_Send(&UART1, bufferPtr, numBytes);
}
