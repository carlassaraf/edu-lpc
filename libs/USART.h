/*
 * USART.h
 *
 *  Created on: Oct 8, 2021
 *      Author: fabri
 */

#ifndef USART_H_
#define USART_H_

#include <fsl_common.h>
#include <fsl_usart.h>
#include <fsl_swm.h>

class USART {
	public:

		uint32_t usart_n;
		USART_Type *usart;
		swm_select_movable_t swm_pins[5];	// { kSWM_USARTn_TXD, kSWM_USARTn_RXD, kSWM_USARTn_RTS, kSWM_USARTn_CTS, kSWM_USARTn_SCLK }

		USART(uint32_t usart_type, uint32_t baud_rate);
		virtual ~USART();

		void prints(const uint8_t *buff);
		void assignPins(uint8_t txd, uint8_t rxd);
		void attachInterrupt(void (*f)(void));
};

#endif /* USART_H_ */
