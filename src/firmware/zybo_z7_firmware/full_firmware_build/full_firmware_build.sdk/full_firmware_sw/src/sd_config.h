

#ifndef SRC_SD_CONFIG_H_
#define SRC_SD_CONFIG_H_

#include "xparameters.h"	/* SDK generated parameters */
#include "xsdps.h"		/* SD device driver */
#include "xil_printf.h"
#include "ff.h"
#include "xil_cache.h"
#include "xplatform_info.h"
#include "shared_definitions.h"
#include <string.h>
#include <stdio.h>
#include "mux_driver.h"
#include "x9258.h"
#include "filter.h"


#define MAX_FILE_LENGTH 1000 //in bytes, arbitrary
XStatus sdConfigInit();
XStatus sdConfigProcess(FIL* fp);

#endif /* SRC_SD_CONFIG_H_ */
