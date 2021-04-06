/*
 * APP_receiveReg.c
 *
 *  Created on: Jun 3, 2019
 *      Author: Marc-Andre Denis
 *
 *      Receive register update command via uart and transmit it to the can bus
 *
 */

#include "rx.h"

#include "../../shared/interfaces/radio/radio_packet.h"
#include "main.h"
#include "uart1.h"
#include "../../shared/bsp/bsp_can.h"
#include "string.h"
#include "tx.h"

#define SIGNAL_NEW_DATA (1<<0)

QueueHandle_t xQueueRxRegsHandle;
SemaphoreHandle_t xSemaphoreRxRegsHandle = NULL;
StaticSemaphore_t xSemaphoreBuffer;

void bytesReceived();

static union{
	uint8_t UINT8[sizeof(radio_packet_t)];
	radio_packet_t packet;
} packetBuff = {0};

static uint8_t packetBuffIndex = 0;

void task_rx(void * pvParameters){
	union rxReg regConf = {0};

	xSemaphoreRxRegsHandle = xSemaphoreCreateBinaryStatic( &xSemaphoreBuffer ); //Create binary semaphore

	while(1){
		//wait for uart transmission
		uart1_registerRxCallback(bytesReceived);
		if(xSemaphoreTake( xSemaphoreRxRegsHandle, portMAX_DELAY ) == pdTRUE){

			uart1_registerRxCallback(0);

			//get data from uart buffer
			while(uart1_rxBuffAvailable()){
				//fill the packet buffer
				packetBuffIndex--;
				packetBuff.UINT8[packetBuffIndex] = uart1_get();
				if(packetBuffIndex < (sizeof(packetBuff) - 1)){
					continue;
				}

				//compute the crc and compare
				if(radio_compute_crc(&(packetBuff.packet)) == packetBuff.packet.checksum){
					//the message is valid
					packetBuffIndex = 0;

					//send packet on internal can bus call callback only if board is com
					if (can_canSetAnyRegisterData(packetBuff.packet.node,\
							packetBuff.packet.message_id,\
							(can_regData_u*)(&(packetBuff.packet.payload)),\
							(packetBuff.packet.node == COMMUNICATION)) ){
						//send confirmation to the base station
						regConf.reg.board = packetBuff.packet.node;
						regConf.reg.id = packetBuff.packet.message_id;
						xQueueSend( xQueueRxRegsHandle,regConf.UINT,0 );
					}
				}
				else{
					//packet is invalid left shift entire buffer to recover the start of next message
					uint8_t i = 0;
					for(i = 0;i < (sizeof(packetBuff) - 1); i++){
						packetBuff.UINT8[i] = packetBuff.UINT8[i+1];
					}
					--packetBuffIndex;
				}
			}
		}
	}
}

void bytesReceived(){
	xSemaphoreGive( xSemaphoreRxRegsHandle );
}