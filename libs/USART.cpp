/*
 * USART.cpp
 *
 *  Created on: Jun 10, 2021
 *      Author: fabri
 */

#include <USART.h>

USART::USART(uint8_t n) {
	// TODO Auto-generated constructor stub
	if (n == 0)
		type = USART0;
	else if (n == 1)
		type = USART1;
}

USART::USART(uint8_t n, uint32_t baudRate) {

	if (n == 0)
		type = USART0;
	else if (n == 1)
		type = USART1;

	USART_SetBaudRate(type, baudRate, 9000000U);
}

USART::~USART() {
	// TODO Auto-generated destructor stub
}

uint8_t USART::read(void) {

	uint8_t str;
	USART_ReadBlocking(type, &str, 1);
	return str;
}

uint8_t* USART::read(uint8_t length) {

	uint8_t str[length + 1] = "";
	USART_ReadBlocking(type, str, length + 1);
	return str;
}

void USART::print(const char *str) {

	USART_WriteBlocking(type, (const uint8_t*)str, strlen(str) + 1);
}

// Doesnt know how to handle floats
void USART::print(const char *str, float number) {

	char new_str[strlen(str) + 10];

	sprintf(new_str, str, number);
	print(new_str);
}

void USART::print(uint8_t* bytes) {

	USART_WriteBlocking(type, bytes, sizeof(bytes));
}

void USART::print(int32_t number) {

	char str[10];
	sprintf(str, "%ld", number);
	print(str);
}

/* USART0_IRQn interrupt handler */
void USART0_USART_IRQHANDLER(void) {
  /*  Place your code here */


  /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F
     Store immediate overlapping exception return operation might vector to incorrect interrupt. */
  #if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
  #endif
}

