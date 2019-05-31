/*
 * heartBeat.h
 *
 *  Created on: 14 mars 2019
 *      Author: Marc-Andre
 *
 *      This thread blinks a led to show that the board is alive
 *
 */

#ifndef APP_APP_HEARTBEAT_H_
#define APP_APP_HEARTBEAT_H_

#include "cmsis_os.h"

#define HEARTBEAT_LED_PORT GPIOD
#define HEARTBEAT_LED_PIN GPIO_PIN_15

void tsk_heartBeat(void const * argument);



#endif /* APP_APP_HEARTBEAT_H_ */
