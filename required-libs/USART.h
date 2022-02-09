/*
 * USART.h
 *
 *  Created on: Oct 8, 2021
 *      Author: fabri
 */

#ifndef USART_H_
#define USART_H_

/* Includes */
#include <stdio.h>
#include <fsl_usart.h>
#include <fsl_swm.h>

/* USART configuration struct */
typedef struct {
	usart_config_t usart_config = {
		115200,							/* Baud rate */
		true,							/* Rx enabled */
		true,							/* Tx enabled */
		false,							/* Loopback disabled */
		false,							/* Continuous SCLK disabled */
		false,							/* Hardware control disabled */
		kUSART_ParityDisabled,			/* No parity */
		kUSART_OneStopBit,				/* One stop bit */
		kUSART_8BitsPerChar,			/* 8-bit data length */
		kUSART_SyncModeDisabled,		/* Asynchronous mode */
		kUSART_RxSampleOnFallingEdge	/* Sample on falling edge */
	};
	/* Movable pin functions */
	swm_select_movable_t swm_functions[2] {0};
	/* TX and RX pins */
	swm_port_pin_type_t swm_pins[2] = {
		kSWM_PortPin_P0_24,
		kSWM_PortPin_P0_25
	};
} usart_settings_t;

/* USART constants */
static constexpr uint8_t TXD_INDEX = 0;
static constexpr uint8_t RXD_INDEX = 1;

/* Class definition */
class USART {

	public:
		/* Constructors */
		USART(uint8_t usart_type, uint32_t baud_rate);
		/* Public methods */
		void print(uint8_t ch);
		void print(uint8_t *buff);
		void print(const char *str);
		void print(uint8_t *buff, uint8_t size);
		void printd(const char *format, int32_t value);
		void printf(const char *format, float value);
		void assignPins(uint8_t txd, uint8_t rxd);
		void attachInterrupt(void (*f)(void));
		uint8_t read(void);
		void read(uint8_t *buffer, uint8_t size);

	private:
		/* USART index */
		uint32_t usart_n;
		/* USART peropherial pointer */
		USART_Type *usart;
		/* USART settings struct */
		usart_settings_t settings;
};

#endif /* USART_H_ */
