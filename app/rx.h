/*
 * APP_receiveReg.h
 *
 *  Created on: Jun 3, 2019
 *      Author: Marc-Andre Denis
 *
 *      Receive register update command via uart and transmit it to the can bus
 *
 */

#ifndef APP_RX_H_
#define APP_RX_H_

void task_rx(void * pvParameters);

#endif /* APP_RX_H_ */
