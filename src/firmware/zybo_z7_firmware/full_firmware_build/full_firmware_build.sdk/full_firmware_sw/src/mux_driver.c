#include "mux_driver.h"


static XGpioPs GPIO_MUX;
static u8 initialized = 0;

static const MUX_SELECT_TYPE inputMuxPins[] = {};
static const MUX_SELECT_TYPE outputMuxPins[] = {0U,9U,14U}; //S0 through S3: JF7,JF8,JF9,JF10

//holds output mux configuration settings for each filter. Indexed by filters enum in shared_definitions.h
static mux_config_data_t outputConfigs[NUM_FILTERS] = {
		{0, 0b010, 0}, //1ST ORDER LP
		{1, 0b011, 0}, //1ST ORDER HP
		{1, 0b101, 0}, //2ND ORDER BP
		{1, 0b100, 0}, //6TH ORDER BP
		{1, 0b100, 0}, //6TH ORDER HP
		{0, 0b100, 0}, //6TH ORDER LP
		{1, 0b001, 0}, //60HZ NOTCH
		{0, 0b0000, 0}, //PASSTHROUGH
};
//holds input mux configuration settings for each input. Indexed by inputs_e in shared_definitions.h
static mux_config_data_t inputConfigs[NUM_INPUTS] = {
		{0, 0b100, 1}, //AUXILARY35MM
		{0, 0b011, 1}, //ANALOG
		{0, 0b000, 1}, //SENSOR1V8
		{0, 0b001, 1}, //SENSOR3V3
		{0, 0b010, 1}, //SENSOR5V0
};

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
	muxSetActiveFilter(FILTER_PASSTHROUGH);
	//muxSetInputPins(AUXILARY35MM);

	return status;
}

/**
 * Sets active filter to connect to output.
 */
u8 muxSetActiveFilter( filters_e filterSelect){

	u8 i;

	if(initialized == 0){
		muxInit();
	}
	if(filterSelect >= NUM_FILTERS){
		xil_printf("Error, %d is not a valid filter enum #\n", filterSelect);
		return MUX_SET_FAILED;
	}

	mux_config_data_t *config = &outputConfigs[filterSelect];

	for(i = 0; i < NUM_SELECT_PINS; i++){
		XGpioPs_WritePin(&GPIO_MUX, outputMuxPins[i], (config->muxSelectPins >> i) & 0b1); //S0 (JF7)
	}
	XGpioPs_WritePin(&GPIO_MUX, ENABLE_PIN, config->enablePin); //Enable Pin (JF4)
	XGpioPs_WritePin(&GPIO_MUX, HL_TOGGLE_PIN, config->hlToggle); //H/L Toggle (JF1)

	return 0;
}

u8 muxSetInputPins( inputs_e inputSelect){

	u8 i;

	if(initialized == 0){
		muxInit();
	}
	if(inputSelect >= NUM_INPUTS){
		xil_printf("input requested %d not in valid range\n", inputSelect);
		return MUX_SET_FAILED;
	}

	mux_config_data_t *config = &inputConfigs[inputSelect];

	for(i = 0; i <= NUM_SELECT_PINS; i++){
		XGpioPs_WritePin(&GPIO_MUX, inputMuxPins[i], (config->muxSelectPins >> i) & 0b1); //S0 (JF7)
	}
	XGpioPs_WritePin(&GPIO_MUX, ENABLE_PIN, config->enablePin); //Enable Pin (JF4)

	return 0;
}
