/*
 * shared_definitions.h
 *
 * This file has common enums and objects shared by multiple libraries in this softare.
 *
 *
 *  Created on: Dec 3, 2019
 *      Author: tdempsay
 */

#ifndef SRC_SHARED_DEFINITIONS_H_
#define SRC_SHARED_DEFINITIONS_H_

/**
 * Meant to enable outputting of debug messages over uart (WIP).
 */
#define DEBUG 1
/**
 * Maps the filters/paths on the board to numbers.
 * Starts at 0 (used to create arrays of presets mapped to this enum.
 * Do not change order unless you know what you are doing.
 */
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

/**
 * Maps the board inputs to numbers.
 * Starts at 0.
 */
typedef enum inputs
{
    AUXILARY35MM,
    ANALOG,
    SENSOR1V8,
    SENSOR3V3,
    SENSOR5V0,
	DIGITAL_I2C,
	DITIAL_SPI,
	DIGITAL_UART,
    NUM_INPUTS
} inputs_e;

/**
 * Maps commands sent to/from GUI to numbers. Must match command enum
 * used in GUI src code.
 */
typedef enum commands
{
	INPUT_SELECT = 0,
	SAMPLE_RATE_SET = 1,
	FILTER_SELECT = 2,
	CORNER_FREQ_SET = 3,
	START_SAMPLING = 8,
	STOP_SAMPLING = 9,
	NUM_COMMANDS = 6

} commands_e;
#endif /* SRC_SHARED_DEFINITIONS_H_ */
