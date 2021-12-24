/*
 * USART.h
 *
 *  Created on: Oct 8, 2021
 *      Author: fabri
 */

#ifndef USART_H_
#define USART_H_

/*  USART includes  */

#include <fsl_common.h>
#include <fsl_usart.h>
#include <fsl_swm.h>

/*  Class definition  */

class USART {

	public:

		USART(uint32_t usart_type, uint32_t baud_rate);
		void print(uint8_t ch);
		void print(uint8_t *buff);
		void print(const char *str);
		void print(uint8_t *buff, uint32_t size);
		void printd(const char *format, int32_t value);
		void printf(const char *format, float value);
		void assignPins(uint8_t txd, uint8_t rxd);
		void attachInterrupt(void (*f)(void));
		uint8_t read(void);
		void read(uint8_t *buffer, uint32_t size);

	private:

		uint32_t usart_n;
		USART_Type *usart;
		swm_select_movable_t swm_pins[5];	// { kSWM_USARTn_TXD, kSWM_USARTn_RXD, kSWM_USARTn_RTS, kSWM_USARTn_CTS, kSWM_USARTn_SCLK }
};

#endif /* USART_H_ */
