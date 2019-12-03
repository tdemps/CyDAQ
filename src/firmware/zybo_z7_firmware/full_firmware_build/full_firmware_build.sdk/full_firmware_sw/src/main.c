

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
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
#include "xuartps.h"
#include "shared_definitions.h"

#define IIC_DEVICE_ID		XPAR_XIICPS_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define IIC_INT_VEC_ID		XPAR_XIICPS_0_INTR
#define BUTTON_BASE_ADDR	XPAR_GPIO_0_BASEADDR
void xadcTest();
void commTest(u8 echo);
void filterTest();
void initButtons();
u8 getButtons();

bool samplingEnabled;
int main()
{
    init_platform();
    commInit();
    muxInit();
    xadcInit();
    init_x9258_i2c(IIC_DEVICE_ID);

    samplingEnabled = false;
	xil_printf("\n\n\n**********CyDAQ Test Program***********\n");

    //xadcTest();
    //commTest(1);
	filterTest();
    char* test = "@STRT1!";
    commProcessPacket(test, 7);

    xil_printf("Exiting..");
    cleanup_platform();
    return 0;
}
void xadcTest(){

    xadcEnableSampling(0);

	sleep(3);

	xadcProcessSamples();
	return;
}

void commTest(u8 echo){

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
		}
	}

	return;
}

void filterTest(){
	u32 i = 0;
    FILTER_FREQ_TYPE lower = 1550;	  //1550hz -> 10k
	FILTER_FREQ_TYPE upper = 15000;  //15000hz -> 1K
	int status;

	 while(1){
	    	muxSetActiveFilter(FILTER_6TH_ORDER_BP);
	    	//muxSetPins(FILTER_PASSTHROUGH); //Passthrough
	    	status = tuneFilter(FILTER_6TH_ORDER_BP, lower, upper);

			//status = x9258_volatile_write(wiper, i);
			//status = x9258_volatile_write(wiper2, i);

			i = (i <= 255) ? i+10 : 0;
			//xil_printf("Status: %d, i: %d\n", status, i);
			sleep(3);
		}
}

u8 getButtons(){
	return (u8) Xil_In32(BUTTON_BASE_ADDR);
}
