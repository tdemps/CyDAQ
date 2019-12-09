#include "mux_driver.h"

static XGpio GPIO_MUX;
static u8 initialized = 0;

//static const MUX_SELECT_TYPE inputMuxPins[] = {0x8, 0x4, 0x2}; //Input select lines S0-S3 on JE header.
//static const MUX_SELECT_TYPE filterMuxPins[] = {0x80, 0x40, 0x20};
/*
 * S0 through S3: JE10,JE9,JE8 Reference Zybo PMOD pinout to see numbering (filter muxes).
 * S0 through S3: JE4, JE3, JE2 (Input select mux).
 * High/Low Toggle Pin should be 0 for high pass, 1 for low pass.
 * The values in the arrays represent the bit position of the pin's relative to their position within the je bus (input to GPIO block in design).
 * See constaints file for GPIO routing to external pins.
 */

//holds output mux configuration settings for each filter. Indexed by filters enum in shared_definitions.h\
//the offsets are to align the bits with the select line place within the GPIO bus/pin numbering on the PMOD.
//Ex. filter selects are pins 8,9,10 on the PMOD, mapped to bits 5,6,7 into the axi_gpio. Therefore, the offset is 5.
//
//Select Line bits are 0bS2S1S0
static mux_config_data_t outputConfigs[NUM_FILTERS] = {
		{1, 0b010, 1}, //1ST ORDER LP
		{0, 0b011, 1}, //1ST ORDER HP
		{1, 0b101, 1}, //2ND ORDER BP
		{1, 0b100, 1}, //6TH ORDER BP
		{0, 0b110, 1}, //6TH ORDER HP
		{1, 0b110, 1}, //6TH ORDER LP
		{1, 0b001, 1}, //60HZ NOTCH
		{0, 0b000, 1}, //PASSTHROUGH
};
//holds input mux configuration settings for each input. Indexed by inputs_e in shared_definitions.h
//Select Line bits are 0bS2S1S0
static mux_config_data_t inputConfigs[NUM_INPUTS] = {
		{0, 0b100, 1}, //AUXILARY35MM
		{0, 0b011, 1}, //ANALOG
		{0, 0b000, 1}, //SENSOR1V8
		{0, 0b001, 1}, //SENSOR3V3
		{0, 0b010, 1}, //SENSOR5V0
};

int muxInit(){
	int status = 0;
	XGpio_Config *Config;

	Config = XGpio_LookupConfig(GPIO_DEV_ID);
	status = XGpio_CfgInitialize(&GPIO_MUX, Config, Config->BaseAddress);
	if(status != XST_SUCCESS){
		xil_printf("Error setting up GPIO for Mux Pins\n");
	}
	//mask used to get output direction of gpio bus
//	u8 outputMask =~HL_TOGGLE_PIN & ~ENABLE_PIN;
//	for(i = 0; i < NUM_SELECT_PINS; i++){
//		//bit mask of 0 is output direction
//		outputMask &= ~filterMuxPins[i];
//	}

	//set gpio pins to be all outputs (assuming mux pins are the only ones on this channel)
	XGpio_SetDataDirection(&GPIO_MUX, MUX_GPIO_CHANNEL, 0x00);

	initialized = 1;

	//set pins to default state
	muxSetActiveFilter(FILTER_PASSTHROUGH);
	muxSetInputPins(ANALOG);

	return status;
}

/**
 * Sets active filter to connect to output.
 */
u8 muxSetActiveFilter( filters_e filterSelect){

	if(initialized == 0){
		muxInit();
	}
	if(filterSelect >= NUM_FILTERS){
		xil_printf("Error, %d is not a valid filter enum #\n", filterSelect);
		return MUX_SET_FAILED;
	}

	mux_config_data_t *config = &outputConfigs[filterSelect];
	u8 mask =  ((u8) XGpio_DiscreteRead(&GPIO_MUX, MUX_GPIO_CHANNEL) & (0x0E)) | ((config->muxSelectPins << FLTR_PIN_OFFSET)
				| (config->enablePin << EN_PIN_OFFSET) | (config->hlToggle << HL_TGLE_PIN_OFFSET));

	XGpio_DiscreteWrite(&GPIO_MUX, MUX_GPIO_CHANNEL, mask);

	return 0;
}

u8 muxSetInputPins( inputs_e inputSelect){


	if(initialized == 0){
		muxInit();
	}
	if(inputSelect >= NUM_INPUTS){
		xil_printf("input requested %d not in valid range\n", inputSelect);
		return MUX_SET_FAILED;
	}

	mux_config_data_t *config = &inputConfigs[inputSelect];
	u8 mask =  ((u8) XGpio_DiscreteRead(&GPIO_MUX, MUX_GPIO_CHANNEL) & (0xF0)) | ((config->muxSelectPins << INPUT_PIN_OFFSET)
				| (config->enablePin << EN_PIN_OFFSET));

	XGpio_DiscreteWrite(&GPIO_MUX, MUX_GPIO_CHANNEL, mask);

	return 0;
}
