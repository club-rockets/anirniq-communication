/*
 * uart1_dma.c
 *
 *  Created on: 30 mai 2019
 *      Author: Marc-Andre
 */

#include "uart1.h"

static void(*uart1_rxCallback)(void) = 0;

volatile struct{
	uint8_t start,end;
	uint8_t Buff[UART1_BUFFER_SIZE];
} uart1_RxBuff = {0},uart1_TxBuff = {0};

void uart1_init() {

	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	//enable the peripheral clock

	//enable the peripheral
	USART1->CR1 |= USART_CR1_UE;

	//set baud rate settings 256000
	//settings for 84mhz APB clock

	USART1->BRR = 20<<4 | 0x08;

	//enable receiver mode
	USART1->CR1 |= USART_CR1_RE;
	//ENABLE TRANSMIT MODE
	USART1->CR1 |= USART_CR1_TE;

	//enable rxne interrupts
	USART1->CR1 |= USART_CR1_RXNEIE;

	//enable usart1 in nvic
	NVIC_EnableIRQ(37);
}


//return positive is transfer possible without buffer overrun
uint32_t uart1_transmit(uint8_t*buff,uint8_t size){
	//check for space
	if(uart1_TxBuff.end<uart1_TxBuff.start){
		if(uart1_TxBuff.start-uart1_TxBuff.end -1 < size) return 0;
	}
	else if (uart1_TxBuff.end>uart1_TxBuff.start){
		if(UART1_BUFFER_SIZE -1 -uart1_TxBuff.end + uart1_TxBuff.start < size) return 0;
	}

	//copy the data in the buffer
	uint8_t i;
	for(i=0;i<size;i++){
		uart1_TxBuff.end++;
		uart1_TxBuff.end%=UART1_BUFFER_SIZE;
		uart1_TxBuff.Buff[uart1_TxBuff.end] = buff[i];
	}
	//enable transmit interrupt
	USART1->CR1 |= USART_CR1_TXEIE;
	return 1;
}

uint8_t uart1_rxBuffAvailable(){
	return !(uart1_RxBuff.end == uart1_RxBuff.start);
}

uint8_t uart1_get(){
	uart1_RxBuff.start++;
	uart1_RxBuff.start%=UART1_BUFFER_SIZE;
	return uart1_RxBuff.Buff[uart1_RxBuff.start];
}

void uart1_registerRxCallback(void (*callback)(void)){
	uart1_rxCallback = callback;
}

void USART1_IRQHandler(){
	if(USART1->SR & USART_SR_TXE){
		//if tx buffer not empty
		if(uart1_TxBuff.end-uart1_TxBuff.start){
			uart1_TxBuff.start++;
			uart1_TxBuff.start%=UART1_BUFFER_SIZE;
			USART1->DR = uart1_TxBuff.Buff[uart1_TxBuff.start];
		}
		else USART1->CR1 &=~USART_CR1_TXEIE;
	}
	if(USART1->SR & USART_SR_RXNE){
		uart1_RxBuff.end++;
		uart1_RxBuff.end%=UART1_BUFFER_SIZE;
		uart1_RxBuff.Buff[uart1_RxBuff.end] = USART1->DR;
		if(uart1_rxCallback){
			uart1_rxCallback();
		}
	}
}



