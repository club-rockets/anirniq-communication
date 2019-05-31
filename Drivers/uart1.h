/*
 * uart1_dma.h
 *
 *  Created on: 30 mai 2019
 *      Author: Marc-Andre
 */

#ifndef DRIVERS_UART1_H_
#define DRIVERS_UART1_H_

#include "stm32f407xx.h"

#define UART1_BUFFER_SIZE 100

void uart1_init();

uint32_t uart1_transmit(uint8_t*buff,uint8_t size);




#endif /* DRIVERS_UART1_H_ */
