/*
 * shared_definitions.h
 *
 *  Created on: Dec 3, 2019
 *      Author: tdempsay
 */

#ifndef SRC_SHARED_DEFINITIONS_H_
#define SRC_SHARED_DEFINITIONS_H_

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

typedef enum inputs
{
    AUXILARY35MM,
    ANALOG,
    SENSOR1V8,
    SENSOR3V3,
    SENSOR5V0,
    NUM_INPUTS
} inputs_e;

#endif /* SRC_SHARED_DEFINITIONS_H_ */
