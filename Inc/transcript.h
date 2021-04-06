#ifndef TRANSCRIPT_H
#define TRANSCRIPT_H

#include "main.h"
#include "stdint.h"

//Transcript enable define
#define configTRANSCRIPT_ENABLED 	1

//Transcript message
#define configTRANSCRIPT_WARNING 	1
#define configTRANSCRIPT_ERROR 		1
#define configTRANSCRIPT_DEFAULT 	1

#define configTRANSCRIPT_UART_PERIPHERAL &huartx

enum{

	CWARNING = 1,
	CERROR

};

void transcript(const char* module, const char* pData, uint8_t code);

#endif
