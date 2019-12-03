/*
 * filter.h
 *
 *  Created on: Oct 15, 2019
 *      Author: tdempsay
 */

#ifndef SRC_FILTER_H_
#define SRC_FILTER_H_

#include "x9258.h"
#include <xil_printf.h>
#include "mux_driver.h"
#include "shared_definitions.h"

#define MAX_ORDER 8
#define MAX_RETRIES 3
#define FILTER_FREQ_TYPE unsigned int
#define FILTER_FREQ_MAX 40000
#define FILTER_FREQ_MIN 10
#define FILTER_ORDER_TYPE u8

typedef struct _filter_t{
	  filters_e filterEnum;
	  FILTER_FREQ_TYPE currentFreq[2];
	  FILTER_ORDER_TYPE filterOrder;
	  wiper_t *wipers;
} filter_t;

u8 tuneFilter(filters_e filterSelect, FILTER_FREQ_TYPE freq1, FILTER_FREQ_TYPE freq2);
POT_R_TYPE freqToPotVal(FILTER_FREQ_TYPE freq);
wiper_t* getWiperArray(filters_e filterEnum);

#endif /* SRC_FILTER_H_ */
