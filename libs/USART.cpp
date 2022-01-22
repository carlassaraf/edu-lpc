/*
 * USART.cpp
 *
 *  Created on: Oct 8, 2021
 *      Author: fabri
 */

#include <USART.h>

/* USART IRQ handler pointer */
void (*usart_irq_ptr[3])(void);

/* Array with the different USART IRQs */
IRQn_Type usart_irq_arr[3] = {
	USART0_IRQn,
	USART1_IRQn,
	USART2_IRQn
};

/*!
 * @brief USART constructor.

 * Creates an USART object. Default pins are PIO0_24 for TXD
 * and PIO0_25 for RXD.
 *
 * @param usart_type is the USART module to use (0-2 supported).
 * @param baud_rate speed of the communication in bauds.
 *
 * @retval None.
 */
USART::USART(uint32_t usart_type, uint32_t baud_rate) {
	/* Save usart index*/
	usart_n = usart_type;
	/* Get the swm pin functions */
	settings.swm_functions[0] = (swm_select_movable_t)(kSWM_USART0_TXD + usart_n * kSWM_USART1_TXD);
	settings.swm_functions[1] = (swm_select_movable_t)(kSWM_USART0_RXD + usart_n * kSWM_USART1_TXD);
	/* Select USART CLK from main CLK */
    CLOCK_Select((clock_select_t)CLK_MUX_DEFINE(FCLKSEL[usart_type], 1U));
    /* Enable SWM CLK */
	SYSCON->SYSAHBCLKCTRL0 |= SYSCON_SYSAHBCLKCTRL0_SWM_MASK;
	/* USART_TXD connect to P0_24 */
	SWM_SetMovablePinSelect(SWM0, settings.swm_functions[TXD_INDEX], settings.swm_pins[TXD_INDEX]);
	/* USART_RXD connect to P0_25 */
	SWM_SetMovablePinSelect(SWM0, settings.swm_functions[RXD_INDEX], settings.swm_pins[RXD_INDEX]);
	/* Disable SWM CLK */
	SYSCON->SYSAHBCLKCTRL0 &= ~SYSCON_SYSAHBCLKCTRL0_SWM_MASK;
	/* Update settings baud rate */
    settings.usart_config.baudRate_Bps = baud_rate;
    /* Get USART pointer */
    usart = (USART_Type*)(0x40064000 + (uint32_t)usart_n * 0x4000);
    /* Initialize USART */
    USART_Init(usart, &settings.usart_config, CLOCK_GetFreq(kCLOCK_MainClk));
}

/*!
 * @brief USART print method.

 * Sends a single byte through USART.
 *
 * @param byte to be sent.
 *
 * @retval None.
 */
void USART::print(uint8_t ch) {
	/* Print a single byte via polling */
	USART_WriteBlocking(usart, &ch, 1);
}

/*!
 * @brief USART print method.

 * Sends an array through USART.
 *
 * @param buff array to be sent.
 *
 * @retval None.
 */
void USART::print(uint8_t *buff) {
	/* Cast to the proper type and print data */
	print((char*)buff);
}

/*!
 * @brief USART print method.

 * Sends a string through USART.
 *
 * @param str string to be sent.
 *
 * @retval None.
 */
void USART::print(const char *str) {
	/* Size counter */
	int size = 0;
	while(*str) {
		/* Get the full size of the string */		
		size++;
		str++;
	}
	/* Restore pointer */
	str = str - size;
	/* Write the number of bytes via polling */
	USART_WriteBlocking(usart, str, (size_t)size);
}

/*!
 * @brief USART print method.

 * Sends a number of bytes through USART.
 *
 * @param buff array to be sent.
 * @param size number of bytes to be sent.
 *
 * @retval None.
 */
void USART::print(uint8_t *buff, uint32_t size) {
	/* Send a number of bytes via polling */
	USART_WriteBlocking(usart, buff, (size_t)size);
}

/*!
 * @brief USART printd method.

 * Sends a formatted string with an integer through USART.
 *
 * @param format formatted string to be sent.
 * @param value integer to be sent with the string.
 *
 * @retval None.
 */
void USART::printd(const char *format, int32_t value) {
	/* Array to store the string data */
	char buff[50];
	/* Array to store the string data */
	sprintf(buff, format, value);
	/* Send the string */
	print(buff);
}

/*!
 * @brief USART printf method.

 * Sends a formatted string with an float through USART.
 *
 * @param format formatted string to be sent.
 * @param value float to be sent with the string.
 *
 * @retval None.
 */
void USART::printf(const char *format, float value) {
	/* Array to store the string data */
	char buff[50];
	/* Form string with the value */
	sprintf(buff, format, value);
	/* Send the string */
	print(buff);
}

/*!
 * @brief USART assignPins method.

 * Sets the TXD and RXD pins for the USART.
 *
 * @param txd index of the pin to be used.
 * @param rxd index of the pin to be used.
 *
 * @retval None.
 */
void USART::assignPins(uint8_t txd, uint8_t rxd) {
	/* Update settings TX pin */
	settings.swm_pins[TXD_INDEX] = (swm_port_pin_type_t)txd;
	/* Update settings RX pin */
	settings.swm_pins[RXD_INDEX] = (swm_port_pin_type_t)rxd;
	/* Enable SWM CLK */
	SYSCON->SYSAHBCLKCTRL0 |= SYSCON_SYSAHBCLKCTRL0_SWM_MASK;
	/* Reset USART TXD pin */
	SWM_SetMovablePinSelect(SWM0, settings.swm_functions[0], kSWM_PortPin_Reset);
	/* Reset USART RXD pin */
	SWM_SetMovablePinSelect(SWM0, settings.swm_functions[1], kSWM_PortPin_Reset);
	/* USART TXD connect to txd */
	SWM_SetMovablePinSelect(SWM0, settings.swm_functions[0], (swm_port_pin_type_t)txd);
	/* USART RXD connect to rxd */
	SWM_SetMovablePinSelect(SWM0, settings.swm_functions[1], (swm_port_pin_type_t)rxd);
	/* Disable SWM CLK */
	SYSCON->SYSAHBCLKCTRL0 &= ~SYSCON_SYSAHBCLKCTRL0_SWM_MASK;
}

/*!
 * @brief USART attachInterrupt method.

 * Attach a function to the interrupt handler.
 *
 * @param f function to be used.
 *
 * @retval None.
 */
void USART::attachInterrupt(void (*f)(void)) {
	/* Save function pointer to array callback */
	usart_irq_ptr[usart_n] = f;
	/* Enable RX ready interrupt */
	USART_EnableInterrupts(usart, kUSART_RxReadyInterruptEnable);
	NVIC_EnableIRQ(usart_irq_arr[usart_n]);
}

/*!
 * @brief USART read method.

 * Reads a byte from the USART by polling.
 *
 * @param None
 *
 * @retval byte read.
 */
uint8_t USART::read(void) {
	/* Check if there is an interrupt enabled */
	if(usart->INTENSET) {
		/* Just return the data in the register */
		return usart->RXDAT;
	}
	/* Variable to store the data */
	uint8_t ch[1];
	/* Read byte via polling */
	USART_ReadBlocking(usart, ch, 1);
	/* Return read byte */
	return ch[0];
}

/*!
 * @brief USART read method.

 * Reads a number of bytes from the USART by polling.
 *
 * @param buffer array to store the data.
 * @param size number of bytes to be read.
 *
 * @retval None.
 */
void USART::read(uint8_t *buffer, uint32_t size) {
	/* Read a number of bytes and store them in the array */
	USART_ReadBlocking(usart, buffer, size);
}

/*  USART IRQ handlers  */

extern "C" {

	void USART0_IRQHandler(void) {
		/* Check if function exists */	
		if(usart_irq_ptr[0]) {
			/* Call function */
			usart_irq_ptr[0]();
		}
	}

	void USART1_IRQHandler(void) {
		/* Check if function exists */
		if(usart_irq_ptr[1]) {
			/* Call function */
			usart_irq_ptr[1]();
		}
	}

	void USART2_IRQHandler(void) {
		/* Check if function exists */
		if(usart_irq_ptr[2]) {
			/* Call function */
			usart_irq_ptr[2]();
		}
	}
}