#include "mux_driver.h"

#define GPIO_DEV_ID XPAR_XGPIOPS_0_DEVICE_ID

static XGpioPs GPIO_MUX;
static u8 initialized = 0;

static const SELECT_TYPE inputMuxPins[] = {};
static const SELECT_TYPE outputMuxPins[] = {0U,9U,14U}; //S0 through S3: JF7,JF8,JF9,JF10

int muxInit(){
	int status = 0, i = 0;
	XGpioPs_Config *Config;

	Config = XGpioPs_LookupConfig(GPIO_DEV_ID);
	status = XGpioPs_CfgInitialize(&GPIO_MUX, Config, Config->BaseAddr);
	//set pins to output direction
	for(i = 0; i < NUM_SELECT_PINS; i++){
		XGpioPs_SetDirectionPin(&GPIO_MUX, outputMuxPins[i], 1U);
		XGpioPs_SetOutputEnablePin(&GPIO_MUX, outputMuxPins[i], 1U);
//		XGpioPs_SetDirectionPin(&GPIO_MUX, inputMuxPins[i], 1U);
//		XGpioPs_SetOutputEnablePin(&GPIO_MUX, inputMuxPins[i], 1U);

	}
	XGpioPs_SetDirectionPin(&GPIO_MUX, ENABLE_PIN, 1U);
	XGpioPs_SetDirectionPin(&GPIO_MUX, HL_TOGGLE_PIN, 1U);
	//Output Enable for each pin
	XGpioPs_SetOutputEnablePin(&GPIO_MUX, ENABLE_PIN, 1U);
	XGpioPs_SetOutputEnablePin(&GPIO_MUX, HL_TOGGLE_PIN, 1U);

	initialized = 1;

  //set pins to default state
	muxSetOutputPins(0,0b000,0);

	return status;
}

void muxSetOutputPins( SELECT_TYPE hlToggle, SELECT_TYPE muxSelects, SELECT_TYPE ePin){

	u8 i;

	if(initialized == 0){
		muxInit();
	}
	for(i = 0; i < NUM_SELECT_PINS; i++){
		XGpioPs_WritePin(&GPIO_MUX, outputMuxPins[i], (muxSelects >> i) & 0b1); //S0 (JF7)
	}
	XGpioPs_WritePin(&GPIO_MUX, ENABLE_PIN, ePin); //Enable Pin (JF4)
	XGpioPs_WritePin(&GPIO_MUX, HL_TOGGLE_PIN, hlToggle); //H/L Toggle (JF1)

}

void muxSetInputPins( SELECT_TYPE hlToggle, SELECT_TYPE muxSelects, SELECT_TYPE ePin){

	u8 i;

	if(initialized == 0){
		muxInit();
	}
	for(i = 0; i <= NUM_SELECT_PINS; i++){
		XGpioPs_WritePin(&GPIO_MUX, inputMuxPins[i], (muxSelects >> i) & 0b1); //S0 (JF7)
	}
	XGpioPs_WritePin(&GPIO_MUX, ENABLE_PIN, ePin); //Enable Pin (JF4)
	XGpioPs_WritePin(&GPIO_MUX, HL_TOGGLE_PIN, hlToggle); //H/L Toggle (JF1)

}
