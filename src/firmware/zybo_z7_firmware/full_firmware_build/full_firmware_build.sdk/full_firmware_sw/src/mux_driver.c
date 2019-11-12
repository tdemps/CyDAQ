#include "mux_driver.h"

#define GPIO_DEV_ID XPAR_XGPIOPS_0_DEVICE_ID

static XGpioPs GPIO_MUX;
static u8 initialized = 0;

int muxInit(){
	int Status;
	XGpioPs_Config *Config;

	Config = XGpioPs_LookupConfig(GPIO_DEV_ID);
	Status = XGpioPs_CfgInitialize(&GPIO_MUX, Config, Config->BaseAddr);
	//set pins to OutPut
	XGpioPs_SetDirectionPin(&GPIO_MUX, 0U, 1U);
	XGpioPs_SetDirectionPin(&GPIO_MUX, 9U, 1U);
	XGpioPs_SetDirectionPin(&GPIO_MUX, 12U, 1U);
	XGpioPs_SetDirectionPin(&GPIO_MUX, 13U, 1U);
	XGpioPs_SetDirectionPin(&GPIO_MUX, 14U, 1U);
	XGpioPs_SetDirectionPin(&GPIO_MUX, 15U, 1U);
	//Output Enable for each pin
	XGpioPs_SetOutputEnablePin(&GPIO_MUX, 0U, 1U);
	XGpioPs_SetOutputEnablePin(&GPIO_MUX, 9U, 1U);
	XGpioPs_SetOutputEnablePin(&GPIO_MUX, 12U, 1U);
	XGpioPs_SetOutputEnablePin(&GPIO_MUX, 13U, 1U);
	XGpioPs_SetOutputEnablePin(&GPIO_MUX, 14U, 1U);
	XGpioPs_SetOutputEnablePin(&GPIO_MUX, 15U, 1U);

	initialized = 1;

  //set pins to default state
	muxSetPins(0,0b000,0);


	return Status;
}

void muxSetPins( SELECT_TYPE hlToggle, SELECT_TYPE muxSelects, SELECT_TYPE ePin){

	if(initialized == 0){
		muxInit();
		initialized = 1;
	}
	XGpioPs_WritePin(&GPIO_MUX, 0U, (muxSelects & 0b1)); //S0 (JF7)
	XGpioPs_WritePin(&GPIO_MUX, 9U, (muxSelects >> 1) & 0b1); //S1 (JF8)
	XGpioPs_WritePin(&GPIO_MUX, 14U, (muxSelects >> 2) & 0b1); //S2 (JF9)
	XGpioPs_WritePin(&GPIO_MUX, 15U, (muxSelects >> 3) & 0b1); //S3 (JF10)
	XGpioPs_WritePin(&GPIO_MUX, 12U, ePin); //Enable Pin (JF4)
	XGpioPs_WritePin(&GPIO_MUX, 13U, hlToggle); //H/L Toggle (JF1)


}
