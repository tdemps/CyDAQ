/**
* @file filter_driver.c
* @brief This file contains the functions needed to tune the analog
* filters
*
* @author SDMAY18-31
*/

/*-----------------------------------------------------------------
-                           Includes
-----------------------------------------------------------------*/
#include "drivers/filter_driver.h"

/*-----------------------------------------------------------------
-                       Literal Constants
-----------------------------------------------------------------*/
// Filter Specifications
#define NUM_OF_FIRST_ORDER_LPF_WIPERS   1
#define NUM_OF_FIFTH_ORDER_LPF_WIPERS   5
#define NUM_OF_FIRST_ORDER_HPF_WIPERS   2
#define NUM_OF_FIFTH_ORDER_HPF_WIPERS   6
#define NUM_OF_SENCOND_ORDER_BPF_WIPERS 3
#define NUM_OF_SIXTH_ORDER_BPF_WIPERS   9

#define MAX_RETRIES 3

#define FIRSTLPF_NUM_FREQ 0xFF
#define FIFTHLPF_NUM_FREQ 0xFF
#define FIRSTHPF_NUM_FREQ 71
#define FIFTHHPF_NUM_FREQ 47
#define SECONDBPF_NUM_FREQ 0xFF
#define SIXTHBPF_NUM_FREQ 0xFF

/*-----------------------------------------------------------------
-                       Global Variables
-----------------------------------------------------------------*/


/*-----------------------------------------------------------------
-                          Procedures
-----------------------------------------------------------------*/

/**
 * Tune the first order LPF
 *
 * @param cutoff cutoff frequency
 *
 * @return FILTER_TUNE_SUCCESS on successful tuning and
 * FILTER_TUNE_FAILED on failure
 */
uint32_t filter_tune_first_order_LPF(uint16_t cutoff)
{
    const filter_t* filter = getFilter(FILTER_1ST_ORDER_LP);
    uint8_t wiperVal = filter[findFilterIndex(cutoff, filter, FIRSTLPF_NUM_FREQ)].cutoffWiperVal;

    for(int i = 0; i < NUM_OF_FIRST_ORDER_LPF_WIPERS; i++)
    {
        uint32_t err = 0;
        uint8_t retries = 0;
        do {
            err = pot_tune_wiper(firstOrderLPF[i], wiperVal);
            ++retries;
        } while(err != 0 && retries < MAX_RETRIES);

        if(err)
        {
            return FILTER_TUNE_FAILED;
        }
    }

    return FILTER_TUNE_SUCCESS;
}

/**
 * Tune the fifth order LPF
 *
 * @param cutoff cutoff frequency
 *
 * @return FILTER_TUNE_SUCCESS on successful tuning and
 * FILTER_TUNE_FAILED on failure
 */
uint32_t filter_tune_fifth_order_LPF(uint16_t cutoff)
{
    const filter_t* filter = getFilter(FILTER_5TH_ORDER_LP);
    uint8_t wiperVal = filter[findFilterIndex(cutoff, filter, FIFTHLPF_NUM_FREQ)].cutoffWiperVal;

    for(int i = 0; i < NUM_OF_FIFTH_ORDER_LPF_WIPERS; i++)
    {
        uint32_t err = 0;
        uint8_t retries = 0;
        do {
            err = pot_tune_wiper(fifthOrderLPF[i], wiperVal);
            ++retries;
        } while(err != 0 && retries < MAX_RETRIES);

        if(err)
        {
            return FILTER_TUNE_FAILED;
        }
    }

    return FILTER_TUNE_SUCCESS;
}

/**
 * Tune the first order HPF
 *
 * @param cutoff cutoff frequency
 *
 * @return FILTER_TUNE_SUCCESS on successful tuning and
 * FILTER_TUNE_FAILED on failure
 */
uint32_t filter_tune_first_order_HPF(uint16_t cutoff)
{
    const filter_t* filter = getFilter(FILTER_1ST_ORDER_HP);
    uint8_t index = findFilterIndex(cutoff, filter, FIRSTHPF_NUM_FREQ);
    uint8_t filterWiperVal = filter[index].cutoffWiperVal;
    uint8_t offsetWiperVal = filter[index].offsetWiperVal;

    //Tune filter cutoff wipers
    for(int i = 0; i < NUM_OF_FIRST_ORDER_HPF_WIPERS - 1; i++)
    {
        uint32_t err = 0;
        uint8_t retries = 0;
        do {
            err = pot_tune_wiper(firstOrderHPF[i], filterWiperVal);
            ++retries;
        } while(err != 0 && retries < MAX_RETRIES);

        if(err)
        {
            return FILTER_TUNE_FAILED;
        }
    }

    //Tune dc offset wipers
    uint32_t err = 0;
    uint8_t retries = 0;
    do {
        err = pot_tune_wiper(firstOrderHPF[NUM_OF_FIRST_ORDER_HPF_WIPERS - 1], offsetWiperVal);
        ++retries;
    } while(err != 0 && retries < MAX_RETRIES);

    if(err)
    {
        return FILTER_TUNE_FAILED;
    }

    return FILTER_TUNE_SUCCESS;
}

/**
 * Tune the fifth order HPF
 *
 * @param cutoff cutoff frequency
 *
 * @return FILTER_TUNE_SUCCESS on successful tuning and
 * FILTER_TUNE_FAILED on failure
 */
uint32_t filter_tune_fifth_order_HPF(uint16_t cutoff)
{
    const filter_t* filter = getFilter(FILTER_5TH_ORDER_HP);
    uint8_t index = findFilterIndex(cutoff, filter, FIFTHHPF_NUM_FREQ);
    uint8_t filterWiperVal = filter[index].cutoffWiperVal;
    uint8_t offsetWiperVal = filter[index].offsetWiperVal;

    //Tune filter cutoff wipers
    for(int i = 0; i < NUM_OF_FIFTH_ORDER_HPF_WIPERS - 1; i++)
    {
        uint32_t err = 0;
        uint8_t retries = 0;
        do {
            err = pot_tune_wiper(fifthOrderHPF[i], filterWiperVal);
            ++retries;
        } while(err != 0 && retries < MAX_RETRIES);

        if(err)
        {
            return FILTER_TUNE_FAILED;
        }
    }

    //Tune dc offset wipers
    uint32_t err = 0;
    uint8_t retries = 0;
    do {
        err = pot_tune_wiper(fifthOrderHPF[NUM_OF_FIFTH_ORDER_HPF_WIPERS - 1], offsetWiperVal);
        ++retries;
    } while(err != 0 && retries < MAX_RETRIES);

    if(err)
    {
        return FILTER_TUNE_FAILED;
    }

    return FILTER_TUNE_SUCCESS;
}

/**
 * Tune the second order BPF
 *
 * @param cutoff cutoff frequency
 *
 * @return FILTER_TUNE_SUCCESS on successful tuning and
 * FILTER_TUNE_FAILED on failure
 */
uint32_t filter_tune_second_order_BPF(uint16_t cutoff)
{
    const filter_t* filter = getFilter(FILTER_2ND_ORDER_BP);
    uint8_t index = findFilterIndex(cutoff, filter, SECONDBPF_NUM_FREQ);
    uint8_t filterWiperVal = filter[index].cutoffWiperVal;
    uint8_t offsetWiperVal = filter[index].offsetWiperVal;

    //Tune filter cutoff wipers
    for(int i = 0; i < NUM_OF_SENCOND_ORDER_BPF_WIPERS - 1; i++)
    {
        uint32_t err = 0;
        uint8_t retries = 0;
        do {
            err = pot_tune_wiper(secondOrderBPF[i], filterWiperVal);
            ++retries;
        } while(err != 0 && retries < MAX_RETRIES);

        if(err)
        {
            return FILTER_TUNE_FAILED;
        }
    }

    //Tune dc offset wipers
    uint32_t err = 0;
    uint8_t retries = 0;
    do {
        err = pot_tune_wiper(secondOrderBPF[NUM_OF_SENCOND_ORDER_BPF_WIPERS - 1], offsetWiperVal);
        ++retries;
    } while(err != 0 && retries < MAX_RETRIES);

    if(err)
    {
        return FILTER_TUNE_FAILED;
    }

    return FILTER_TUNE_SUCCESS;
}

/**
 * Tune the sixth order BPF
 *
 * @param cutoff cutoff frequency
 *
 * @return FILTER_TUNE_SUCCESS on successful tuning and
 * FILTER_TUNE_FAILED on failure
 */
uint32_t filter_tune_sixth_order_BPF(uint16_t cutoff)
{
    const filter_t* filter = getFilter(FILTER_6TH_ORDER_BP);
    uint8_t index = findFilterIndex(cutoff, filter, SIXTHBPF_NUM_FREQ);
    uint8_t filterWiperVal = filter[index].cutoffWiperVal;
    uint8_t offsetWiperVal = filter[index].offsetWiperVal;

    //Tune filter cutoff wipers
    for(int i = 0; i < NUM_OF_SIXTH_ORDER_BPF_WIPERS - 3; i++)
    {
        uint32_t err = 0;
        uint8_t retries = 0;
        do {
            err = pot_tune_wiper(sixthOrderBPF[i], filterWiperVal);
            ++retries;
        } while(err != 0 && retries < MAX_RETRIES);

        if(err)
        {
            return FILTER_TUNE_FAILED;
        }
    }

    //Tune dc offset wipers
    for(int i = NUM_OF_SIXTH_ORDER_BPF_WIPERS - 3; i < NUM_OF_SIXTH_ORDER_BPF_WIPERS; i++)
    {
        uint32_t err = 0;
        uint8_t retries = 0;
        do {
            err = pot_tune_wiper(sixthOrderBPF[i], offsetWiperVal);
            ++retries;
        } while(err != 0 && retries < MAX_RETRIES);

        if(err)
        {
            return FILTER_TUNE_FAILED;
        }
    }


    return FILTER_TUNE_SUCCESS;
}

/**
 * Find the filter index that corresponds to the wiper value
 *
 * @param cutoff cutoff frequency
 * @param filterVals pointer to the filter values
 * @param numFreq number of cutoff frequencies in filterVals
 *
 * @return the index of the corresponding wiper value
 */
uint8_t findFilterIndex(uint16_t cutoff, const filter_t* filterVals, uint8_t numFreq)
{
    if(filterVals[0].cutoffFreq > cutoff)
    {
        return 0;
    }

    int i;
    for(i = 1; i < numFreq; i++)
    {
        // if cutoff is between two indices
        if(filterVals[i - 1].cutoffFreq <= cutoff
            && filterVals[i].cutoffFreq >= cutoff)
            {
                // determine if i - 1 or i is closer
                uint16_t diff1 = abs_uint_diff( filterVals[i - 1].cutoffFreq, cutoff );
                uint16_t diff2 = abs_uint_diff( filterVals[i].cutoffFreq,     cutoff );

                if(diff1 < diff2)
                {
                    return i - 1;
                }
                else
                {
                    return i;
                }

            }
    }

    return i - 1;
}

uint16_t abs_uint_diff( uint16_t a, uint16_t b )
{
    return ( a >= b ) ? ( a - b ) : ( b - a );
}