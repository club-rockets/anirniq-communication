/*
 * heartBeat.h
 *
 *  Created on: 14 mars 2019
 *      Author: Marc-Andre
 *
 *      This thread blinks a led to show that the board is alive
 *
 */


#include "APP_transmitReg.h"

#include "uart1.h"
#include "stm32f4xx_hal.h"
#include "main.h"
#include "cmsis_os.h"
#include "bsp_can.h"
#include "enums.h"
#include "id.h"
#include "cmsis_os.h"
#include "bsp_can_regdef.h"

uint8_t txBuff[50] = {0};

extern osMessageQId rxRegsHandle;

union rxReg{
	uint32_t UINT;
	struct{
		uint16_t board;
		uint16_t id;
	} reg;
};

void tsk_transmitReg(void const * argument){

	can_setRegisterCallback(MOTHERBOARD,CAN_MOTHERBOARD_STATUS_INDEX,registerUpdated);
	can_setRegisterCallback(MISSION,CAN_MOTHERBOARD_STATUS_INDEX,registerUpdated);

	osEvent rxEvent;
	union rxReg reg;
	while(1){
		rxEvent = osMessageGet(rxRegsHandle,osWaitForever);
		reg.UINT = rxEvent.value.v;
		uart1_transmit((uint8_t*)&reg,sizeof(union rxReg));



	}
}


void registerUpdated(uint32_t board,uint32_t regId){
	union rxReg reg;
	reg.reg.board = board;
	reg.reg.id = regId;
	osMessagePut(rxRegsHandle,reg.UINT,0);
}
