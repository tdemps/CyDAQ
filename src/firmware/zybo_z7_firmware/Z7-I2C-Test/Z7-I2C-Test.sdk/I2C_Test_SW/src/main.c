

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
#define IIC_DEVICE_ID		XPAR_XIICPS_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define IIC_INT_VEC_ID		XPAR_XIICPS_0_INTR
void xadcTest();

int main()
{
    xil_printf("\n\n\n");
	xil_printf("Beginning CyDAQ Test Program\n");
    init_platform();
    xadcTest();
    //initUartComm();
    muxInit();
    int status = init_x9258_i2c(IIC_DEVICE_ID);
    sleep(1);

    filter_t sixthBP = (filter_t) {
    	.filterEnum = FILTER_6TH_ORDER_BP,
		.currentFreq = {0,0},
		.filterOrder = 6,
		.wipers = getWiperArray(FILTER_6TH_ORDER_BP),
		.muxSetting = 0b00001010
    };

    POT_R_TYPE i = 0;

    //CHANGE CORNERS FOR FILTER HERE//////////////////////////////////////////////////////////
    FREQ_TYPE lower = 1550;	  //1550hz -> 10k
	FREQ_TYPE upper = 15000;  //15000hz -> 1K
	///////////////////////////////////////////////////////////////////////////////////////////

    //while(1){
		//inputs are: HP/LP toggle, S3S2S1S0, Enable for muxSetPins
    	muxSetPins( 1, 0b0101, 0);
    	//muxSetPins( 1, 0b0000, 0); //Passthrough
    	status = tuneFilter(&sixthBP, lower, upper);

		//status = x9258_volatile_write(wiper, i);
		//status = x9258_volatile_write(wiper2, i);

		//i = (i <= 255) ? i+10 : 0;
		//xil_printf("Status: %d, i: %d\n", status, i);
		sleep(3);
	//}

    cleanup_platform();
    return 0;
}
void xadcTest(){

    u32 status = 0;
    float voltage = 0.0;
    xadcInit();
   // xil_printf("Hello World\n\r");
    u16 i = 0, reading = 0;
    xadcEnableSampling();
	sleep(5);

	xadcProcessSamples();

    return 0;
}
