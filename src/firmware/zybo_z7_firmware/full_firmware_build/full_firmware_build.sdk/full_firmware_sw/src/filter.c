#include "filter.h"

/*
 * Addresses of each x9258 IC (set by Ax pins)
 */
const uint8_t dev_address_a = 0b0000; //IC A
const uint8_t dev_address_b = 0b0110; //IC B
const uint8_t dev_address_c = 0b0010; //IC C
const uint8_t dev_address_d = 0b1010; //IC D

static wiper_t sixthOrderLP[6] = {
  //ic    wiper
  {dev_address_c,  1},
  {dev_address_c,  2}, ///////////////NOT RIGHT
  {dev_address_d,  0},
  {dev_address_d,  1},
  {dev_address_d,  2},
  {dev_address_d,  3}
};
static wiper_t sixthOrderHP[6] = {
  //ic    wiper
  {dev_address_c,  1},
  {dev_address_c,  2},
  {dev_address_d,  0},
  {dev_address_d,  1},
  {dev_address_d,  2},
  {dev_address_d,  3}
};
static wiper_t secondOrderBP[2] = {
  //ic    wiper
  {dev_address_a,  1},
  {dev_address_a,  2} //upper corner pot
};
static wiper_t sixthOrderBP[6] = {
  //filter tuning wipers
  //ic    wiper
  {dev_address_b,  1},
  {dev_address_c,  0},
  {dev_address_c,  3},
  //lp pots (upper corner)
  {dev_address_b,  0},
  {dev_address_b,  2},
  {dev_address_b,  3}
};
static wiper_t firstOrderLP[1] = {
  {dev_address_a,0}
};
static wiper_t firstOrderHP[1] = {
  {dev_address_a, 3}
};


/*
 * Array of pointers to wiper_t arrays for each filter (defined above).
 */
static wiper_t *filterPotConfigs[6] = {firstOrderLP, firstOrderHP, secondOrderBP, sixthOrderBP, sixthOrderHP, sixthOrderLP};

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
	.wipers = sixthOrderHP
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
 * For BP, freq1 is lower corner and freq2 is upper corner.
 * If the filter only has one corner, pass in NULL or 0 as freq2
 */
u8 tuneFilter(filters_e filterSelect, FILTER_FREQ_TYPE freq1, FILTER_FREQ_TYPE freq2){

	if(freq1 > FILTER_FREQ_MAX || freq1 < FILTER_FREQ_MIN ||
			  freq2 > FILTER_FREQ_MAX || freq2 < FILTER_FREQ_MIN || filterSelect <= NUM_FILTERS){
		  xil_printf("Invalid frequency values given\n");
		  return 1;
	}

	POT_I2C_ERR_TYPE err = 0;
	POT_R_TYPE potVal1 = freqToPotVal(freq1);
	POT_R_TYPE potVal2 = freqToPotVal(freq2);
	filter_t *currentFilter = filterConfigs[filterSelect];
	u8 wipersToWrite = currentFilter->filterOrder;
	u8 retries = 0;

	//if filter is a bandpass, write upper corner to lp pots (upper half of wiper array)
	if(freq2 > 0){
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
  xil_printf("Freq: %d, Calculated R: %d\n", freq, resistance);
  return pot_value_conversion(resistance);
}

wiper_t* getWiperArray(filters_e filterEnum){
	return filterPotConfigs[filterEnum];
}
