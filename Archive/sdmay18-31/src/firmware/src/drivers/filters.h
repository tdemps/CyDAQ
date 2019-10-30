/**
* @file filters.h
* @brief Header file for the filters library
*
* @author SDMAY18-31
*/
#ifndef __FILTERS_H__
#define __FILTERS_H__

/*-----------------------------------------------------------------
-                           Includes
-----------------------------------------------------------------*/
#include <stdint.h>
#include "drivers/shared_defs.h"

/*----------------------------------------------------------------
-                           Structs
-----------------------------------------------------------------*/
typedef struct _filter_t
{
    uint16_t cutoffFreq;
    uint8_t cutoffWiperVal;
    uint8_t offsetWiperVal;
} filter_t;

/*----------------------------------------------------------------
-                          Prototype
-----------------------------------------------------------------*/
const filter_t* getFilter(uint8_t filter);

#endif // __FILTERS_H__