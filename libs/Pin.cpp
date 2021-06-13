/*
 * Pin.cpp
 *
 *  Created on: Jun 10, 2021
 *      Author: fabri
 */

#include "Pin.h"

Pin::Pin(uint8_t pin_n, const uint8_t mode) {
	// TODO Auto-generated constructor stub
	pin = pin_n;
	port = getPORT(pin);
	index = getPinIndex(pin);

	if (pin > 31)
		pin -= 32;

	const uint32_t PIN = (	/* Selects pull-up function */
							IOCON_PIO_MODE_PULLUP |
							/* Enable hysteresis */
							IOCON_PIO_HYS_EN |
							/* Input not invert */
							IOCON_PIO_INV_DI |
							/* Disables Open-drain function */
							IOCON_PIO_OD_DI |
							/* Bypass input filter */
							IOCON_PIO_SMODE_BYPASS |
							/* IOCONCLKDIV0 */
							IOCON_PIO_CLKDIV0);

	IOCON_PinMuxSet(IOCON, index, PIN);
	GPIO_PinInit(GPIO, port, pin, (mode)? &configOutput : &configInput);
}

Pin::~Pin() {
	// TODO Auto-generated destructor stub
}

uint8_t Pin::read(void) {

	return (uint8_t) GPIO_PinRead(GPIO, port, pin);
}

void Pin::write(const uint8_t value) {

	if (value)
		set();
	else
		clear();
}

void Pin::set(void) {

	GPIO_PortSet(GPIO, port, 1<<pin);
}

void Pin::clear(void) {

	GPIO_PortClear(GPIO, port, 1<<pin);
}

void Pin::toggle(void) {

	GPIO_PortToggle(GPIO, port, 1<<pin);
}

uint8_t getPinIndex(uint8_t pin) {

	switch(pin) {

		case 0:
			return IOCON_INDEX_PIO0_0;
			break;

		case 1:
			return IOCON_INDEX_PIO0_1;
			break;

		case 2:
			return IOCON_INDEX_PIO0_2;
			break;

		case 3:
			return IOCON_INDEX_PIO0_3;
			break;

		case 4:
			return IOCON_INDEX_PIO0_4;
			break;

		case 5:
			return IOCON_INDEX_PIO0_5;
			break;

		case 6:
			return IOCON_INDEX_PIO0_6;
			break;

		case 7:
			return IOCON_INDEX_PIO0_7;
			break;

		case 8:
			return IOCON_INDEX_PIO0_8;
			break;

		case 9:
			return IOCON_INDEX_PIO0_9;
			break;

		case 10:
			return IOCON_INDEX_PIO0_10;
			break;

		case 11:
			return IOCON_INDEX_PIO0_11;
			break;

		case 12:
			return IOCON_INDEX_PIO0_12;
			break;

		case 13:
			return IOCON_INDEX_PIO0_13;
			break;

		case 14:
			return IOCON_INDEX_PIO0_14;
			break;

		case 15:
			return IOCON_INDEX_PIO0_15;
			break;

		case 16:
			return IOCON_INDEX_PIO0_16;
			break;

		case 17:
			return IOCON_INDEX_PIO0_17;
			break;

		case 18:
			return IOCON_INDEX_PIO0_18;
			break;

		case 19:
			return IOCON_INDEX_PIO0_19;
			break;

		case 20:
			return IOCON_INDEX_PIO0_20;
			break;

		case 21:
			return IOCON_INDEX_PIO0_21;
			break;

		case 22:
			return IOCON_INDEX_PIO0_22;
			break;

		case 23:
			return IOCON_INDEX_PIO0_24;
			break;

		case 24:
			return IOCON_INDEX_PIO0_25;
			break;

		case 25:
			return IOCON_INDEX_PIO0_25;
			break;

		case 26:
			return IOCON_INDEX_PIO0_26;
			break;

		case 27:
			return IOCON_INDEX_PIO0_27;
			break;

		case 28:
			return IOCON_INDEX_PIO0_28;
			break;

		case 29:
			return IOCON_INDEX_PIO0_29;
			break;

		case 30:
			return IOCON_INDEX_PIO0_30;
			break;

		case 31:
			return IOCON_INDEX_PIO0_31;
			break;

		case 32:
			return IOCON_INDEX_PIO1_0;
			break;

		case 33:
			return IOCON_INDEX_PIO1_1;
			break;

		case 34:
			return IOCON_INDEX_PIO1_2;
			break;

		case 35:
			return IOCON_INDEX_PIO1_3;
			break;

		case 36:
			return IOCON_INDEX_PIO1_4;
			break;

		case 37:
			return IOCON_INDEX_PIO1_5;
			break;

		case 38:
			return IOCON_INDEX_PIO1_6;
			break;

		case 39:
			return IOCON_INDEX_PIO1_7;
			break;

		case 40:
			return IOCON_INDEX_PIO1_8;
			break;

		case 41:
			return IOCON_INDEX_PIO1_9;
			break;

		case 42:
			return IOCON_INDEX_PIO1_10;
			break;

		case 43:
			return IOCON_INDEX_PIO1_11;
			break;

		case 44:
			return IOCON_INDEX_PIO1_12;
			break;

		case 45:
			return IOCON_INDEX_PIO1_13;
			break;

		case 46:
			return IOCON_INDEX_PIO1_14;
			break;

		case 47:
			return IOCON_INDEX_PIO1_15;
			break;

		case 48:
			return IOCON_INDEX_PIO1_16;
			break;

		case 49:
			return IOCON_INDEX_PIO1_17;
			break;

		case 50:
			return IOCON_INDEX_PIO1_18;
			break;

		case 51:
			return IOCON_INDEX_PIO1_19;
			break;

		case 52:
			return IOCON_INDEX_PIO1_20;
			break;

		case 53:
			return IOCON_INDEX_PIO1_21;
			break;
	}
	return 0;
}
