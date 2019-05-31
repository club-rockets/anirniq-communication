/*
 * heartBeat.h
 *
 *  Created on: 14 mars 2019
 *      Author: Marc-Andre
 *
 *      This thread blinks a led to show that the board is alive
 *
 */


#include "APP_heartBeat.h"
#include "stm32f4xx_hal.h"
#include "main.h"
#include "bsp_can.h"
#include "id.h"
#include "enums.h"

void tsk_heartBeat(void const * argument){

	can_regData_u data = {.UINT32_T = 2};

	while(1){
		HAL_GPIO_WritePin(LED_4_GPIO_Port,LED_4_Pin,1);
		osDelay(50);
		HAL_GPIO_WritePin(LED_4_GPIO_Port,LED_4_Pin,0);
		osDelay(1000);
		//can_canSetRegisterLoopback(MOTHERBOARD,CAN_MOTHERBOARD_STATUS_INDEX,&data);
	}
}
