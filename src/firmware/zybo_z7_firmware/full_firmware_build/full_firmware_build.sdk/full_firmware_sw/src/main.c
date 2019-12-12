

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
void commTest(u8 echo);
void filterTest();
void initButtons();
u8 getButtons();
u8 getButtonChangeBlocking();
bool samplingEnabled;
int main()
{
	//initialization functions for all libraries
	init_platform();
    commInit();
    if(DEBUG)
    	xil_printf("\n\n\n**********CyDAQ Test Program***********\n");
    muxInit();
    xadcInit();
    init_x9258_i2c();

//	xil_printf("Select Test:\n 1)ADC\n2)Comm3)Filter\n");
    //commRXTask();
    //xadcTest();
    //commTest(0);
	filterTest();
	if(DEBUG)
		xil_printf("Exiting..");
    cleanup_platform();
    return 0;
}
/**
 * Test for XADC
 */
void xadcTest(){

	xadcSetSampleRate(10);
	xil_printf("Press BTN1 for real-time data streaming, BTN0 else\n");
	u8 btns = getButtonChangeBlocking();
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
void commTest(u8 echo){
	if(DEBUG)
		xil_printf("Comm Test Ready, Echo Enable: %s\n", (echo) ? "On" : "Off");
	XUartPs* ptr = commGetUartPtr();
	u8 buffer[100];
	u8 buttons = 0;
	u32 numBytes = 0, i = 0;
	while( (buttons = getButtons()) != 0x3){
		numBytes += XUartPs_Recv(ptr, &buffer[numBytes], 8);
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
    FILTER_FREQ_TYPE highpass = 10000, lowpass = 20000;	  //1550hz -> 10k ohm, 15000hz -> 1k ohm
	XUartPs* ptr = commGetUartPtr();
	u8 selectedFilter = FILTER_1ST_ORDER_LP, buttons = 0, numBytes = 0, status = 0;
	u8 buf[20];
	muxSetActiveFilter(selectedFilter);	//enums defined in shared_definitions.h for filters and inputs
	muxSetInputPins(ANALOG);
	status = tuneFilter(selectedFilter, highpass, lowpass);

	 while((buttons = getButtons()) != 0xC){	//hit buttons 3 and 4 together to end test
			numBytes += XUartPs_Recv(ptr, &buf[numBytes], 2);
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
				}
				xil_printf("Increasing corners: %d, %d\n", highpass, lowpass);
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
	while( (buttons = (u8) Xil_In32(BUTTON_BASE_ADDR)) == 0x0);
	return buttons;
}
