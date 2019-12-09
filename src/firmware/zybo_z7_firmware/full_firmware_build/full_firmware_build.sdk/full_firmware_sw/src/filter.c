#include "filter.h"

static wiper_t sixthOrderLP[6] = { //also 6order HP
  //ic    wiper
  {DEV_ADDRESS_B,  1},
  {DEV_ADDRESS_B,  2},
  {DEV_ADDRESS_C,  3},
  {DEV_ADDRESS_C,  2},
  {DEV_ADDRESS_C,  1},
  {DEV_ADDRESS_C,  0}
};

static wiper_t secondOrderBP[2] = {
  //ic    wiper
  {DEV_ADDRESS_A,  1},
  {DEV_ADDRESS_A,  3} //upper corner pot
};

static wiper_t sixthOrderBP[6] = {
  //filter tuning wipers
  //ic    wiper
  {DEV_ADDRESS_D,  3},
  {DEV_ADDRESS_B,  0},
  {DEV_ADDRESS_B,  3},
  //lp pots (upper corner)
  {DEV_ADDRESS_D,  2},
  {DEV_ADDRESS_D,  1},
  {DEV_ADDRESS_D,  0}
};

static wiper_t firstOrderLP[1] = {
  {DEV_ADDRESS_A,2}
};

static wiper_t firstOrderHP[1] = {
  {DEV_ADDRESS_A, 0}
};


/*
 * Array of pointers to wiper_t arrays for each filter (defined above).
 * Order matches that of filter enum in shared_definitions.h
 */
static wiper_t *filterPotConfigs[6] = {firstOrderLP, firstOrderHP, secondOrderBP, sixthOrderBP, sixthOrderLP, sixthOrderLP};

static filter_t sixthBP = {
	.filterEnum = FILTER_6TH_ORDER_BP,
	.currentFreq = {0,0},
	.filterOrder = 6,
	.wipers = sixthOrderBP
};

static filter_t sixthLP = {
	.filterEnum = FILTER_6TH_ORDER_LP,
	.currentFreq = {0,0},
	.filterOrder = 6,
	.wipers = sixthOrderLP
};

static filter_t sixthHP = {
	.filterEnum = FILTER_6TH_ORDER_HP,
	.currentFreq = {0,0},
	.filterOrder = 6,
	.wipers = sixthOrderLP
};

static filter_t firstLP = {
	.filterEnum = FILTER_1ST_ORDER_LP,
	.currentFreq = {0,0},
	.filterOrder = 1,
	.wipers = firstOrderLP
};

static filter_t firstHP = {
	.filterEnum = FILTER_1ST_ORDER_HP,
	.currentFreq = {0,0},
	.filterOrder = 1,
	.wipers = firstOrderHP
};

static filter_t secondBP = {
	.filterEnum = FILTER_2ND_ORDER_BP,
	.currentFreq = {0,0},
	.filterOrder = 1,
	.wipers = secondOrderBP
};
static filter_t *filterConfigs[NUM_FILTERS-1] = {&firstLP, &firstHP, &secondBP, &sixthBP, &sixthHP, &sixthLP};
/*
 * Writes pots to get the desired frequency(ies).
 * For BP, freq1 is lower corner(HP) and freq2 is upper corner(LP).
 * If the filter only has one corner, pass in NULL or 0 as freq2
 */
u8 tuneFilter(filters_e filterSelect, FILTER_FREQ_TYPE freq1, FILTER_FREQ_TYPE freq2){

	if(freq1 > FILTER_FREQ_MAX || freq2 > FILTER_FREQ_MAX){
		  xil_printf("Invalid frequency values given\n");
		  return 1;
	}else if(filterSelect == FILTER_60_HZ_NOTCH || filterSelect == FILTER_PASSTHROUGH){ //cant be notch or pass
		xil_printf("Can't tune passthrough or notch\n");
		return 2;
	}

	POT_I2C_ERR_TYPE err = 0;
	POT_R_TYPE potVal1 = freqToPotVal(freq1);
	filter_t *currentFilter = filterConfigs[filterSelect];
	u8 wipersToWrite = currentFilter->filterOrder;
	u8 retries = 0;

	//if filter is a bandpass, write upper corner to lp pots (upper half of wiper array)
	if(filterSelect == FILTER_2ND_ORDER_BP || filterSelect == FILTER_6TH_ORDER_BP){
		xil_printf("Upper ");
		POT_R_TYPE potVal2 = freqToPotVal(freq2);

		for(int i = wipersToWrite / 2; i < wipersToWrite; i++){
			//will try to rewrite pot if i2c returned error status
			do{
				err = x9258_volatile_write(currentFilter->wipers[i], potVal2);
				retries++;
			}while(err != 0 && retries < MAX_RETRIES);
		}
		wipersToWrite = currentFilter->filterOrder / 2;
	}
	//Write the remaining pots (or all of them if filter is not BP
	for(int i = 0; i < wipersToWrite; i++){
		do{
			err = x9258_volatile_write(currentFilter->wipers[i], potVal1);
			retries++;
		}while(err != 0 && retries < MAX_RETRIES);
	}
	if(err == 0){
	  currentFilter->currentFreq[0] = freq1;
	  currentFilter->currentFreq[1] = freq2;
	}
	return err;
}

POT_R_TYPE freqToPotVal(FILTER_FREQ_TYPE freq){
  const float capacitance = 10.0 * pow(10.0, -9.0);

  int resistance = 1.0 / (2.0 * 3.14159 * capacitance * freq);
  xil_printf("Freq (hz): %d, R (ohm): %d\n", freq, resistance);
  return pot_value_conversion(resistance);
}

wiper_t* getWiperArray(filters_e filterEnum){
	return filterPotConfigs[filterEnum];
}
