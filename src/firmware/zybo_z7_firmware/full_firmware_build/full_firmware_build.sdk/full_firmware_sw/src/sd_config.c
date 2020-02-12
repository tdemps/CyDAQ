/*
 * sd_config.c
 *
 *  Created on: Feb 10, 2020
 *      Author: tdempsay
 *
 *      V1.0 Release (2-11-2020): Initial release of this library for use in EE 423.
 */


#include "sd_config.h"
#include "jsmn.h"
#define TEST 7

/*
 * This function attempts to mount the sd card and open the configuration file if present.
 * If it fails initialization or cannot find the file, it will return XST_FAILURE. If the
 * file is present, it will call sdConfigProcess() to read it's contents. I believe the xilffs
 * library assumes a FAT file system.
 */
XStatus sdConfigInit(){

	/************************** Variable Definitions *****************************/
	static FIL fil;		/* File object */
	static FATFS fatfs;
	/*
	 * To test logical drive 0, FileName should be "0:/<File name>" or
	 * "<file_name>". For logical drive 1, FileName should be "1:/<file_name>"
	 */
	static char FileName[40] = "config.json";
	static char *SD_File;
	FRESULT Res;

	/*
	 * To test logical drive 0, Path should be "0:/"
	 * For logical drive 1, Path should be "1:/"
	 */
	TCHAR *Path = "0:/";

	/*
	 * Register volume work area, initialize device
	 */
	Res = f_mount(&fatfs, Path, 0);

	if (Res != FR_OK) {
		return XST_FAILURE;
	}

	/*
	 * Open file with required permissions.
	 * Here - Creating new file with read/write permissions. .
	 * To open file with write permissions, file system should not
	 * be in Read Only mode.
	 */
	SD_File = (char *)FileName;

	Res = f_open(&fil, SD_File, FA_OPEN_ALWAYS | FA_READ);
	if (Res) {
		if(DEBUG){
			xil_printf("Error: can't find config.json\n");
		}
		return XST_FAILURE;
	}

	/*
	 * Pointer to beginning of file .
	 */
	Res = f_lseek(&fil, 0);
	if (Res) {
		return XST_FAILURE;
	}

	/*
	 * Pointer to beginning of file .
	 */
	Res = f_lseek(&fil, 0);
	if (Res) {
		return XST_FAILURE;
	}

	Res = sdConfigProcess(&fil);
	if(Res == FR_INVALID_OBJECT){
		xil_printf("Error reading config.json.\n");
	}
	/*
	 * Close file.
	 */
	f_close(&fil);

	return Res;

}
/**
 * TODO: Add support for sampling rate key in config files.
 *
 * This function reads file and processes contents with jsmn library (JSON parser).
 * It then loops through the top level keys looking for the required keys: 'input', 'filter', 'corners'
 * Will return XST_FAILURE if key doesn't match one of those or if there is an error programming the cydaq.
 */
XStatus sdConfigProcess(FIL* fp){

	if(DEBUG){
		xil_printf("Processing configuration file.\n");
	}
	char buffer[MAX_FILE_LENGTH];
	XStatus status = XST_SUCCESS;
	s32 numTokens;
	unsigned int data = 0, cursor = 1, datatwo = 0;
	jsmn_parser p;
	jsmntok_t t[20];
	jsmntok_t temp, child;
	UINT len;
	//read file into buffer
	status = f_read(fp, (void*)buffer, MAX_FILE_LENGTH, &len);
	if(status != XST_SUCCESS){
		return status;
	}
	//initialize and parse json
	jsmn_init(&p);
	numTokens = jsmn_parse(&p, buffer, len, t, 20);
	//jsmn library uses negative values for erros, positive value is number of tokens
	if(numTokens < 0){
		return XST_FAILURE;
	}
	//used to check against keys read from top level of json
	char *keyInput = "input";
	char *keyFilter = "filter";
	char *keyCorner = "corners";
	//loops through all top level tokens (keys) of json.
	//index is updated through if statements so temp should always be key
	while(cursor < numTokens){
		temp = t[cursor];
		//top level node should always be string key
		if(temp.type != JSMN_STRING || temp.size == 0){
			xil_printf("Error");
		}
		//compares n first bytes of each string, uses indices from temp to determine how long
		//the string is
		if(strncmp(&buffer[temp.start], keyInput,temp.end-temp.start) == 0){
			child = t[++cursor];
			//all numbers should be primitive object
			if(child.type != JSMN_PRIMITIVE){
				status = XST_FAILURE;
				break;
			}
			//gets number from string, puts it in data var
			sscanf(&buffer[child.start], "%1u", &data);
			xil_printf("Input: %d\n", data);
			status = muxSetInputPins((inputs_e) data);

		}else if(strncmp(&buffer[temp.start], keyFilter,temp.end-temp.start) == 0){
			child = t[++cursor];
			if(child.type != JSMN_PRIMITIVE){
				xil_printf("Error, wrong object format for child token\n");
				status = XST_FAILURE;
				break;
			}
			sscanf(&buffer[child.start], "%1u", &data);
			xil_printf("Filter: %d\n", data);
			status = muxSetActiveFilter((filters_e) data);

		}else if(strncmp(&buffer[temp.start], keyCorner,temp.end-temp.start) == 0){
			child = t[++cursor];
			//corners should be in array format
			if(child.type != JSMN_ARRAY){
				xil_printf("Error, wrong object format for child token\n");
				status = XST_FAILURE;
				break;
			}
			child = t[++cursor];
			if(child.type != JSMN_PRIMITIVE){
				xil_printf("Error, wrong object format for child token\n");
				status = XST_FAILURE;
				break;
			}
			sscanf(&buffer[child.start], "%5u", &data);
			child = t[++cursor];
			if(child.type != JSMN_PRIMITIVE){
				xil_printf("Error, wrong object format for child token\n");
				status = XST_FAILURE;
				break;
			}
			sscanf(&buffer[child.start], "%5u", &datatwo);
			xil_printf("Corners: %d, %d\n", data, datatwo);
			status = tuneFilter(50, data, datatwo);

		}else{
			xil_printf("Error, invalid command\n");
			status = XST_FAILURE;
		}
		if(status != XST_SUCCESS){
			xil_printf("Error programming board\n");
			break;
		}
		//increments based on top level node size
		cursor += temp.size;
	}
	if(DEBUG){
		xil_printf("Config file successfully processed.\n");
	}
	return status;
}
