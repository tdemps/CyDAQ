

/*
 * Main function for program! Also contains multiple tests for various functionalities of program.
 *
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include <xparameters.h>
#include "x9258.h"
#include <sleep.h>
#include "filter.h"
#include "mux_driver.h"
#include "comm.h"
#include "stdlib.h"
#include "xadc.h"
#include "shared_definitions.h"

#define BUTTON_BASE_ADDR	XPAR_GPIO_0_BASEADDR
void xadcTest();
void commTest();
void filterTest();
void initButtons();
u8 getButtons();
u8 getButtonChangeBlocking();

bool samplingEnabled;
filters_e activeFilter = FILTER_PASSTHROUGH;
int main()
{
	//initialization functions for all libraries
	init_platform();
    commInit();
    muxInit();
    xadcInit();
    init_x9258_i2c();

    if(DEBUG){
    	xil_printf("\n**********CyDAQ Test Program***********\n");
    	u8 btns = 0;
		while(1){
			xil_printf("Select Test:\n0. XADC\n1. UART Test\n2. Filter Test\n3. Firmware Start\n");
			btns = getButtonChangeBlocking();
			usleep(300000);
			switch(btns){
				case 1: xadcTest(); break;
				case 2: commTest(); break;
				case 4: filterTest(); break;
				case 8:
						xil_printf("Started main firmware process\n");
						commRXTask();
						break;
			}
			sleep(2);
		}

    }
    commRXTask();
	if(DEBUG)
		xil_printf("Exiting Test Program\n");
    cleanup_platform();
    return 0;
}
/**
 * Test for XADC
 */
void xadcTest(){
	xil_printf("*****XADC Test*****\nPress BTN1 for data streaming, BTN0 else\n");
	xil_printf("Note faster sample rates may be affected by real-time streaming\n");
	xil_printf("if streaming is enabled, type ! to stop program\n");
	u8 btns = getButtonChangeBlocking();
	xil_printf("Enter sample rate in samples/second and hit enter\n");
	u8 buffer[10];
	u8 numBytes = 0;
	while(1){
		numBytes += comUartRecv(&buffer[numBytes], 2);
		if(buffer[numBytes-1] == '\r' || buffer[numBytes-1] == '\n'){
			buffer[numBytes-1] = '\0';
			u32 rate = atoi(buffer);
			if(rate > 0 && rate < XADC_MAX_SAMPLE_RATE){
				xadcSetSampleRate(rate);
				break;
			}else{
				xil_printf("Error, invalid input, using default (10,000)\n");
				xadcSetSampleRate(10000);
				break;
			}
		}
	}
    xadcEnableSampling(btns-1);
    if(btns < 2){
    	xadcProcessSamples();
    }
    sleep(1);
	xil_printf("Exiting XADC Test\n");
	return;
}
/**
 * Test for receiving/transmitting over UART
 */
void commTest(){
	if(DEBUG)
		xil_printf("Comm Test Ready, Press btn0 for enable echo, btns 1-3 otherwise\n");
	u8 echo = getButtonChangeBlocking();
	if(DEBUG)
		xil_printf("Press btns 2 and 3 simultaneously to exit test\n");
	usleep(300000);
	u8 buffer[100];
	u8 buttons = 0;
	u32 numBytes = 0, i = 0;
	while( (buttons = getButtons()) != 12){
		numBytes += comUartRecv(&buffer[numBytes], 8);
		if(numBytes > 0 && echo){
			for(i = 0; i < numBytes; i++){
				xil_printf("%c", buffer[i]);
		}
			numBytes = 0;
		}else if(buttons != 0){
			switch(buttons){
				case 1: xil_printf("@TEST123!\n"); break;
				case 2: xil_printf("@FA53!\n"); break;
			}
			usleep(500000);
		}else if(buffer[0] == '@' && buffer[numBytes-1] == '!'){
			xil_printf("@ACK!");
			for(i = 0; i < numBytes; i++){
				buffer[i] = 0;
			}
			//commProcessPacket(buffer, numBytes);
			numBytes = 0;
		}
	}

	return;
}

void filterTest(){
	xil_printf("*****Filter Test*****\nDefault filter: 1st Order LP, corner=10k\n");
	xil_printf("Use btns to select filters (follows enum in shared_defs.h)\nBtn 3 increments corners\n");
	xil_printf("Input is set to analog\n");
	xil_printf("Press btns 2 and 3 simultaneously to exit test\n");
    FILTER_FREQ_TYPE highpass = 10000, lowpass = 20000;	  //1550hz -> 10k ohm, 15000hz -> 1k ohm
	XUartPs* ptr = commGetUartPtr();
	u8 selectedFilter = FILTER_1ST_ORDER_LP, buttons = 0, numBytes = 0, status = 0;
	u8 buf[20];
	muxSetActiveFilter(selectedFilter);	//enums defined in shared_definitions.h for filters and inputs
	muxSetInputPins(ANALOG);
	status = tuneFilter(selectedFilter, highpass, lowpass);

	 while((buttons = getButtons()) != 12){	//hit buttons 3 and 4 together to end test
			numBytes += XUartPs_Recv(ptr, &buf[numBytes], 1);
			if(buf[numBytes-1] == '\r'){
				buf[numBytes-1] = '\0';
				printf("%s\n", buf);
				numBytes = 0;
			}
			if(buttons > 0 && buttons < NUM_FILTERS){
				selectedFilter = buttons-1;
				xil_printf("Changing filter to %d\n", selectedFilter);
				muxSetActiveFilter(selectedFilter);
				highpass = 10000;
				lowpass = 20000;
				tuneFilter(selectedFilter, highpass, lowpass);
			}else if(buttons == 0x8){
				highpass += 1000;
				lowpass += 1000;
				status = tuneFilter(selectedFilter, highpass, lowpass);
				if(status == 1){
					highpass = 10000;
					lowpass = 20000;
				}else{
					xil_printf("Increasing corners to: %d, %d\n", highpass, lowpass);
				}
			}
			usleep(500000);
	}
	xil_printf("Exiting Filter Test\n");
}

u8 getButtons(){
	return (u8) Xil_In32(BUTTON_BASE_ADDR);
}

u8 getButtonChangeBlocking(){
	u8 buttons = 0;
	while( (buttons = (u8) Xil_In32(BUTTON_BASE_ADDR)) == 0x0){
		usleep(50000);
	}
	return buttons;
}
