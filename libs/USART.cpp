/*
 * USART.cpp
 *
 *  Created on: Oct 8, 2021
 *      Author: fabri
 */

#include <USART.h>

void (*usart_irq_ptr[3])(void);

IRQn_Type usart_irq_arr[3] = {
	USART0_IRQn,
	USART1_IRQn,
	USART2_IRQn
};

USART::USART(uint32_t usart_type, uint32_t baud_rate) {

	usart_n = usart_type;

    switch (usart_type) {

    	case 0:	usart = USART0;
    			for(int i = kSWM_USART0_TXD ; i < kSWM_USART1_TXD ; i++) {
    				swm_pins[i - kSWM_USART0_TXD] = (swm_select_movable_t)i;
    			}
				break;

    	case 1:	usart = USART1;
    			for(int i = kSWM_USART1_TXD ; i < kSWM_USART2_TXD ; i++) {
					swm_pins[i - kSWM_USART1_TXD] = (swm_select_movable_t)i;
				}
    			break;

    	case 2:	usart = USART2;
    			for(int i = kSWM_USART2_TXD ; i < kSWM_SPI0_SCK ; i++) {
    				swm_pins[i - kSWM_USART2_TXD] = (swm_select_movable_t)i;
    			}
				break;

    	case 3:	usart = USART3;
    			for(int i = kSWM_USART3_TXD ; i < kSWM_USART4_TXD ; i++) {
    				swm_pins[i - kSWM_USART3_TXD] = (swm_select_movable_t)i;
    			}
				break;

    	case 4:	usart = USART4;
    			for(int i = kSWM_USART4_TXD ; i < kSWM_T0_MAT_CHN0 ; i++) {
    				swm_pins[i - kSWM_USART4_TXD] = (swm_select_movable_t)i;
    			}
				break;
    }

    CLOCK_Select((clock_select_t)CLK_MUX_DEFINE(FCLKSEL[usart_type], 1U));		// Select USART CLK from main CLK

	SYSCON->SYSAHBCLKCTRL0 |= SYSCON_SYSAHBCLKCTRL0_SWM_MASK;			// Enable SWM CLK
	SWM_SetMovablePinSelect(SWM0, swm_pins[0], kSWM_PortPin_P0_25);	    // USART_TXD connect to P0_25
	SWM_SetMovablePinSelect(SWM0, swm_pins[1], kSWM_PortPin_P0_24);		// USART_RXD connect to P0_24
	SYSCON->SYSAHBCLKCTRL0 &= ~SYSCON_SYSAHBCLKCTRL0_SWM_MASK;			// Disable SWM CLK

    usart_config_t user_config;
    USART_GetDefaultConfig(&user_config);
    /* Default config by using USART_GetDefaultConfig():
     * config->baudRate_Bps = 9600U;
     * config->parityMode = kUSART_ParityDisabled;
     * config->stopBitCount = kUSART_OneStopBit;
     * config->bitCountPerChar = kUSART_8BitsPerChar;
     * config->loopback = false;
     * config->enableRx = false;
     * config->enableTx = false;
     * config->syncMode = kUSART_SyncModeDisabled;
     */

    user_config.baudRate_Bps = baud_rate;
    user_config.enableTx = true;
    user_config.enableRx = true;
    USART_Init(usart, &user_config, CLOCK_GetFreq(kCLOCK_MainClk));
}

void USART::print(uint8_t ch) {

	USART_WriteBlocking(usart, &ch, 1);
}

void USART::print(uint8_t *buff) {

	print((char*)buff);
}

void USART::print(const char *str) {

	int size = 0;
	while(*str) {		// Get the full size of the string
		size++;
		str++;
	}
	str = str - size;
	USART_WriteBlocking(usart, str, (size_t)size);
}

void USART::print(uint8_t *buff, uint32_t size) {

	USART_WriteBlocking(usart, buff, (size_t)size);
}

void USART::printd(const char *format, int32_t value) {

	char buff[50];

	sprintf(buff, format, value);
	print(buff);
}

void USART::printf(const char *format, float value) {

	char buff[50];

	sprintf(buff, format, value);
	print(buff);
}

void USART::assignPins(uint8_t txd, uint8_t rxd) {

	SYSCON->SYSAHBCLKCTRL0 |= SYSCON_SYSAHBCLKCTRL0_SWM_MASK;	// Enable SWM CLK
	SWM_SetMovablePinSelect(SWM0, swm_pins[0], kSWM_PortPin_Reset);	    			// Reset peripheral pin
	SWM_SetMovablePinSelect(SWM0, swm_pins[1], kSWM_PortPin_Reset);					// Reset peripheral pin
	SWM_SetMovablePinSelect(SWM0, swm_pins[0], (swm_port_pin_type_t)txd);	    	// USART_TXD connect to txd
	SWM_SetMovablePinSelect(SWM0, swm_pins[1], (swm_port_pin_type_t)rxd);			// USART_RXD connect to rxd
	SYSCON->SYSAHBCLKCTRL0 &= ~SYSCON_SYSAHBCLKCTRL0_SWM_MASK;	// Disable SWM CLK
}

void USART::attachInterrupt(void (*f)(void)) {

	usart_irq_ptr[usart_n] = f;
	USART_EnableInterrupts(usart, kUSART_RxReadyInterruptEnable);
	NVIC_EnableIRQ(usart_irq_arr[usart_n]);
}

uint8_t USART::read(void) {

	uint8_t ch[1];
	USART_ReadBlocking(usart, ch, 1);
	return ch[0];
}

void USART::read(uint8_t *buffer, uint32_t size) {

	USART_ReadBlocking(usart, buffer, size);
}

extern "C" {

	void USART0_IRQHandler(void) {

		if(usart_irq_ptr[0]) {
			usart_irq_ptr[0]();
		}
	}

	void USART1_IRQHandler(void) {

		USART_WriteByte(USART1, USART_ReadByte(USART1));
		if(usart_irq_ptr[1]) {
			usart_irq_ptr[1]();
		}
	}

	void USART2_IRQHandler(void) {

		if(usart_irq_ptr[2]) {
			usart_irq_ptr[2]();
		}
	}
}