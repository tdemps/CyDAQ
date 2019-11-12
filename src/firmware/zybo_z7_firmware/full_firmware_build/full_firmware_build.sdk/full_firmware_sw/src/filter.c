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
static wiper_t *FilterPotArrays[6] = {firstOrderLP, firstOrderHP, secondOrderBP, sixthOrderBP, sixthOrderHP, sixthOrderLP};

/*
 * Writes pots to get the desired frequency(ies).
 * For BP, freq1 is lower corner and freq2 is upper corner.
 * If the filter only has one corner, pass in NULL or 0 as freq2
 */
unsigned char tuneFilter(filter_t *filter, FREQ_TYPE freq1, FREQ_TYPE freq2){
  POT_I2C_ERR_TYPE err = 0;
  POT_R_TYPE potVal1 = freqToPotVal(freq1);
  POT_R_TYPE potVal2 = freqToPotVal(freq2);
  u8 wipersToWrite = filter->filterOrder;
  u8 retries = 0;

  //if filter is a bandpass, write upper corner to lp pots (upper half of wiper array)
  if(freq2 > 0){
    for(int i = wipersToWrite / 2; i < wipersToWrite; i++){
      //Teensy will try to rewrite pot if i2c returned error status
      do{
      err = x9258_volatile_write(filter->wipers[i], potVal2);
      retries++;
      }while(err != 0 && retries < MAX_RETRIES);
    }
    wipersToWrite = filter->filterOrder / 2;
  }
  //Write the remaining pots (or all of them if filter is not BP
  for(int i = 0; i < wipersToWrite; i++){
    do{
    err = x9258_volatile_write(filter->wipers[i], potVal1);
    retries++;
    }while(err != 0 && retries < MAX_RETRIES);
  }
  if(err == 0){
	  filter->currentFreq[0] = freq1;
	  filter->currentFreq[1] = freq2;
  }
  return err;
}

POT_R_TYPE freqToPotVal(FREQ_TYPE freq){
  const float capacitance = 10.0 * pow(10.0, -9.0);

  int resistance = 1.0 / (2.0 * 3.14159 * capacitance * freq);
  xil_printf("Freq: %d, Calculated R: %d\n", freq, resistance);
  return pot_value_conversion(resistance);
}

wiper_t* getWiperArray(filters_e filterEnum){
	return FilterPotArrays[filterEnum];
}
