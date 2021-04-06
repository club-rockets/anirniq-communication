/*
 * heartBeat.h
 *
 *  Created on: 14 mars 2019
 *      Author: Marc-Andre
 *
 *      This thread blinks a led to show that the board is alive
 *
 */

#ifndef APP_TX_H_
#define APP_TX_H_

union rxReg{
	uint32_t UINT;
	struct{
		uint16_t board;
		uint16_t id;
	} reg;
};

void configPadCallbacks();

void configLaunchCallbacks();

void task_tx(void * pvParameters);

void registerUpdated(uint32_t board,uint32_t regId);

void regTransmiTtimer_callback( TimerHandle_t xTimer );

#endif //APP_TRANSMIT_REG_H_

