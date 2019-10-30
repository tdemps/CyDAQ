/**
* @file communications.h
* @brief Header file containing definitions required for
* both the UART TX and RX tasks
*
* @author SDMAY18-31
*/

#ifndef _communications_h
#define _communications_h

/*-----------------------------------------------------------------
-                           Includes
-----------------------------------------------------------------*/
#include <stdint.h>

/*-----------------------------------------------------------------
-                         Enumerations
-----------------------------------------------------------------*/
typedef enum
{
    START_COLLECTION    = 0,
    STOP_COLLECTION     = 1,
    ENABLE_DEV          = 2,
    DISABLE_DEV         = 3,    // Deprecated
    CONFIG_ADC0         = 4,
    CONFIG_ADC1         = 5,
    CONFIG_ADC2         = 6,
    CONFIG_I2C          = 7,
    CONFIG_SPI          = 8,
    RESET_DEV           = 9,    // Deprecated
    RESET_ALL           = 10,   // Deprecated
    REPORT_DEVICES      = 11,   // Deprecated
    TUNE_FILTER         = 12,
    START_BYTE          = 252,
    ACK                 = 253,
    NAK                 = 254,
    INVALID_CMD         = 255
} sig_cmds_e;

typedef enum
{
    SET_SAMPLING_RATE = 0,
    SET_FILTER_ROUTE  = 1
} adc_config_cmds_e;

/*-----------------------------------------------------------------
-                         Structures
-----------------------------------------------------------------*/
typedef struct __attribute__((__packed__))
{
    uint8_t     start_byte;
    uint8_t     cmd;     // Specifies cmd type
    uint32_t    param1;  // Actionable data for cmd
    uint32_t    param2;  // Actionable data for cmd
    uint16_t    crc;     // Error Detection Code
} rx_cmd_pkt_t;

typedef struct __attribute__((__packed__))
{
    uint16_t    data[5]; // Specifies cmd type
    uint16_t    crc;     // Error Detection Code
} rx_data_pkt_t;

#endif
