/*
 * comm.h
 *
 *  Created on: Oct 16, 2019
 *      Author: tdempsay
 */

#ifndef SRC_COMM_H_
#define SRC_COMM_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "xparameters.h"
#include "xplatform_info.h"
#include "xuartps.h"
#include "xil_exception.h"
#include "xil_printf.h"
#include "xadc.h"
#include "filter.h"
#ifdef XPAR_INTC_0_DEVICE_ID
#include "xintc.h"
#else
#include "xscugic.h"
#endif

#ifdef XPAR_INTC_0_DEVICE_ID
#define INTC		XIntc
#define UART_DEVICE_ID		XPAR_XUARTPS_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_INTC_0_DEVICE_ID
#define UART_INT_IRQ_ID		XPAR_INTC_0_UARTPS_0_VEC_ID
#else
#define INTC		XScuGic
#define UART_DEVICE_ID		XPAR_XUARTPS_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define UART_INT_IRQ_ID		XPAR_XUARTPS_1_INTR
#endif

#define UART_DEVICE_ID XPAR_XUARTPS_0_DEVICE_ID
#define COMM_BAUD_RATE 921600U
#define COMM_CMD_SIZE 1		//in bytes
#define COMM_SAMPLE_RATE_SIZE 8 //in bytes
#define COMM_START_CHAR '@'
#define COMM_STOP_CHAR '!'
#define TEST_BUFFER_SIZE	50

typedef enum cmds
{
    START,
    STOP,
    FBTUNE,
    INPUTSELECT,
    FILTERSELECT,
    NUMCOMMANDS
} commands;

int commInit();
static int commSetupInterruptSystem(INTC *IntcInstancePtr, XUartPs *UartInstancePtr, u16 UartIntrId);
void Handler(void *CallBackRef, u32 Event, unsigned int EventData);
XUartPs* commGetUartPtr();
void commRXTask();
bool commProcessPacket(u8 *buffer, u16 bufSize);
u32 comUartRecv(u8 *bufferPtr, u32 numBytes);
#endif /* SRC_COMM_H_ */
