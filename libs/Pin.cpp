/*
 * Pin.cpp
 *
 *  Created on: Jun 10, 2021
 *      Author: fabri
 */

#include "Pin.h"

Pin::Pin(uint8_t pin_n, const uint8_t mode) {
	// TODO Auto-generated constructor stub
	init(pin_n, mode);
}

Pin::Pin(uint8_t pin_n, const uint8_t mode, const uint8_t initial_value) {

	init(pin_n, mode);
	GPIO->B[port][pin] = initial_value;
}

Pin::~Pin() {
	// TODO Auto-generated destructor stub
}

void Pin::init(uint8_t pin_n, const uint8_t mode) {

	pin = pin_n;
	port = getPORT(pin);

	// Enable GPIO CLK
	if (port == 1)
		SYSCON->SYSAHBCLKCTRL0 |= SYSCON_SYSAHBCLKCTRL0_GPIO1_MASK;
	else
		SYSCON->SYSAHBCLKCTRL0 |= SYSCON_SYSAHBCLKCTRL0_GPIO0_MASK;

	if (pin > 31)	pin -= 32;

	if (mode == INPUT)
		GPIO->DIR[port] &= ~(1UL << pin);
	else
		GPIO->DIR[port] |= 1UL << pin;
}

uint8_t Pin::read(void) {

	return (uint8_t) GPIO->B[port][pin];
}

void Pin::write(const uint8_t value) {

	GPIO->B[port][pin] = value;
}

void Pin::set(void) {

	GPIO->SET[port] |= 1UL << pin;
}

void Pin::clear(void) {

	GPIO->CLR[port] = 1UL << pin;
}

void Pin::toggle(void) {

	GPIO->NOT[port] = 1UL << pin;
}
