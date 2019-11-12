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
#include "mux_driver.h">
#define MAX_ORDER 8
#define MAX_RETRIES 3
#define FREQ_TYPE unsigned int
#define FREQ_MAX 40000
#define FILTER_ORDER_TYPE u8

typedef enum filters
{
    FILTER_1ST_ORDER_LP,
    FILTER_1ST_ORDER_HP,
    FILTER_2ND_ORDER_BP,
    FILTER_6TH_ORDER_BP,
    FILTER_6TH_ORDER_HP,
    FILTER_6TH_ORDER_LP,
    FILTER_60_HZ_NOTCH,
    FILTER_PASSTHROUGH,
    NUM_FILTERS
} filters_e;

typedef struct _filter_t{
	  filters_e filterEnum;
	  FREQ_TYPE currentFreq[2];
	  FILTER_ORDER_TYPE filterOrder;
	  wiper_t *wipers;
	  SELECT_TYPE muxSetting;
} filter_t;

unsigned char tuneFilter(filter_t *filter, FREQ_TYPE freq1, FREQ_TYPE freq2);
POT_R_TYPE freqToPotVal(FREQ_TYPE freq);
wiper_t* getWiperArray(filters_e filterEnum);

#endif /* SRC_FILTER_H_ */
