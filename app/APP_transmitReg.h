/*
 * heartBeat.h
 *
 *  Created on: 14 mars 2019
 *      Author: Marc-Andre
 *
 *      This thread blinks a led to show that the board is alive
 *
 */

#ifndef APP_TRANSMIT_REG_H_
#define APP_TRANSMIT_REG_H_

#include "cmsis_os.h"

union rxReg{
	uint32_t UINT;
	struct{
		uint16_t board;
		uint16_t id;
	} reg;
};

void configPadCallbacks();

void configLaunchCallbacks();

void tsk_transmitReg(void const * argument);

void registerUpdated(uint32_t board,uint32_t regId);

#endif //APP_TRANSMIT_REG_H_

