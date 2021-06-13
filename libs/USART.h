/*
 * USART.h
 *
 *  Created on: Jun 10, 2021
 *      Author: fabri
 */

#ifndef USART_H_
#define USART_H_

#include <fsl_usart.h>
#include <stdio.h>
#include <string.h>

class USART {

	public:
		USART(uint8_t n);
		USART(uint8_t n, uint32_t baudRate);
		virtual ~USART();
		uint8_t read(void);
		uint8_t* read(uint8_t length);
		void print(const char *str);
		void print(const char *str, float number);
		void print(uint8_t* bytes);
		void print(int32_t number);
		//void print(float number);

	private:
		USART_Type* type;
};

#endif /* USART_H_ */
