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
#include "radio_packet.h"
#include "string.h"



uint8_t txBuff[50] = {0};

extern osMessageQId rxRegsHandle;
extern osTimerId regTransmiTtimerHandle;

static can_regData_u regData = {0};

static enum {transmitPad,transmitLaunch} transmitMode = transmitPad;

void tsk_transmitReg(void const * argument){

	osEvent rxEvent;
	union rxReg reg;
	radio_packet_t packet = {0};

	configPadCallbacks();

	//variables for register scan
	uint32_t lastRegisterTick1 = 0,lastRegisterTick2 = 0;
	uint32_t boardIndex = 0;
	uint32_t registerIndex = 0;
	uint32_t lastBoardIndex = 0;
	uint32_t lastRegisterIndex = 0;

	osTimerStart(regTransmiTtimerHandle,500);

	while(1){
		//if a callback enabled register is updated
		//wait and send an other register
		if(transmitMode == transmitLaunch){
			rxEvent = osMessageGet(rxRegsHandle,30);
		}
		else{
			rxEvent = osMessageGet(rxRegsHandle,osWaitForever);
		}

		if(rxEvent.status == osEventMessage){
			reg.UINT = rxEvent.value.v;

			memset((void*)(&packet),0,sizeof(radio_packet_t));
			packet.node = reg.reg.board;
			packet.message_id = reg.reg.id;
			memcpy(&(packet.payload),(void*)(&(can_registers[packet.node][packet.message_id].data)),sizeof(radio_packet_payload_t));
			packet.checksum = radio_compute_crc(&packet);

			uart1_transmit((uint8_t*)(&packet),sizeof(packet));
		}
		if(transmitMode == transmitLaunch){
			//verify that the register is not already handled by a change callback or sent recently
			do{
				if(registerIndex == 0 && boardIndex == 0){
					lastRegisterTick1 = lastRegisterTick2;
					lastRegisterTick2 = HAL_GetTick();
				}
				registerIndex = (registerIndex + 1)%(can_registersSize[boardIndex]);
				if(!registerIndex){
					boardIndex = (boardIndex + 1)%(CAN_NUMBER_OF_NODES);
				}
				if(boardIndex == lastBoardIndex && registerIndex == lastRegisterIndex){
					break;
				}
			}while(can_registers[boardIndex][registerIndex].changeCallback == registerUpdated ||\
					can_registers[boardIndex][registerIndex].lastTick < lastRegisterTick1);
			if(!(boardIndex == lastBoardIndex && registerIndex == lastRegisterIndex)){
				lastBoardIndex= boardIndex;
				lastRegisterIndex = registerIndex;
				//transmit the register over uart
				packet.node = boardIndex;
				packet.message_id = registerIndex;
				memcpy(&(packet.payload),(void*)(&(can_registers[packet.node][packet.message_id].data)),sizeof(radio_packet_payload_t));
				packet.checksum = radio_compute_crc(&packet);
				uart1_transmit((uint8_t*)(&packet),sizeof(packet));
			}
		}
	}
}

void configPadCallbacks(){
	transmitMode = transmitPad;
	can_setRegisterCallback(MISSION,CAN_MISSION_STATUS_INDEX,registerUpdated);
	can_setRegisterCallback(COMMUNICATION,CAN_COMMUNICATION_STATUS_INDEX,registerUpdated);
	can_setRegisterCallback(ACQUISITION,CAN_ACQUISITION_STATUS_INDEX,registerUpdated);
	can_setRegisterCallback(MOTHERBOARD,CAN_MOTHERBOARD_STATUS_INDEX,registerUpdated);

	can_setRegisterCallback(ACQUISITION,CAN_ACQUISITION_GPS_LAT_INDEX,registerUpdated);
	can_setRegisterCallback(ACQUISITION,CAN_ACQUISITION_GPS_LON_INDEX,registerUpdated);

	can_setRegisterCallback(MISSION,CAN_MISSION_CHARGE_STATUS_INDEX,registerUpdated);
	can_setRegisterCallback(MISSION,CAN_MISSION_ROCKET_STATUS_INDEX,registerUpdated);

	can_setRegisterCallback(COMMUNICATION,CAN_COMMUNICATION_CONTROL_EJECT_DROGUE_INDEX,registerUpdated);
	can_setRegisterCallback(COMMUNICATION,CAN_COMMUNICATION_CONTROL_EJECT_MAIN_INDEX,registerUpdated);
	can_setRegisterCallback(COMMUNICATION,CAN_COMMUNICATION_CONTROL_SLEEP_INDEX,registerUpdated);
}
void configLaunchCallbacks(){
	transmitMode = transmitLaunch;
	can_setRegisterCallback(MISSION,CAN_MISSION_STATUS_INDEX,0);
	can_setRegisterCallback(COMMUNICATION,CAN_COMMUNICATION_STATUS_INDEX,0);
	can_setRegisterCallback(ACQUISITION,CAN_ACQUISITION_STATUS_INDEX,0);
	can_setRegisterCallback(MOTHERBOARD,CAN_MOTHERBOARD_STATUS_INDEX,0);
}

void registerUpdated(uint32_t board,uint32_t regId){
	union rxReg reg;
	reg.reg.board = board;
	reg.reg.id = regId;
	osMessagePut(rxRegsHandle,reg.UINT,0);
	if(board == COMMUNICATION && regId == CAN_COMMUNICATION_STATUS_INDEX){
		can_getRegisterData(board,regId,&regData);
		if(regData.UINT32_T == PAD_TRANSMISSION){
			configPadCallbacks();
		}
		else configLaunchCallbacks();
	}
}

void regTransmiTtimer_callback(){
	static uint8_t i = 0;
	can_canSetRegisterData(i,0);
	i++;
	i%=can_registersSize[COMMUNICATION];
}
