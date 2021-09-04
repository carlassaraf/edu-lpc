/*
 * Pin.h
 *
 *  Created on: Jun 10, 2021
 *      Author: fabri
 */

#ifndef PIN_H_
#define PIN_H_

#include "fsl_gpio.h"
#include "fsl_iocon.h"
#include "pin_mux.h"

#define getPORT(pin)	(pin < 32)? 0 : 1

static const gpio_pin_config_t configOutput = {
    		kGPIO_DigitalOutput,
			1,
};

static const gpio_pin_config_t configInput = {
		kGPIO_DigitalInput,
		0,
};

/* Define GPIO modes and outputs */
#define INPUT	0
#define OUTPUT	1

#define LOW		0
#define HIGH	1

class Pin {

	public:

		Pin(uint8_t pin_n, const uint8_t mode);
		Pin(uint8_t pin_n, const uint8_t mode, const uint8_t initial_value);
		virtual ~Pin();
		uint8_t read();
		void write(const uint8_t value);
		void set(void);
		void clear(void);
		void toggle(void);

	private:
		uint8_t port;
		uint8_t	pin;

		void init(uint8_t pin_n, const uint8_t mode);
};

#endif /* PIN_H_ */
