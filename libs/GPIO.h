/*
 * Pin.h
 *
 *  Created on: Jun 10, 2021
 *      Author: fabri
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "fsl_gpio.h"
#include "fsl_iocon.h"
#include "pin_mux.h"
#include "fsl_pint.h"
#include "fsl_syscon.h"
#include "fsl_syscon_connections.h"

/*  GPIO Macros  */

#define getPORT(pin)				(pin < 32)? 0 : 1
#define getSYSCON_CONNECTION(pin)	(syscon_connection_t)(pin + (PINTSEL_ID << SYSCON_SHIFT))

/* Define GPIO modes and outputs */

#define INPUT	kGPIO_DigitalInput
#define OUTPUT	kGPIO_DigitalOutput

#define LOW		0
#define HIGH	1

/*  PINT Modes  */

#define PINT_NONE			kPINT_PinIntEnableNone
#define PINT_RISE_EDGE		kPINT_PinIntEnableRiseEdge
#define PINT_FALL_EDGE		kPINT_PinIntEnableFallEdge
#define PINT_BOTH_EDGES		kPINT_PinIntEnableBothEdges
#define PINT_LOW_LEVEL		kPINT_PinIntEnableLowLevel
#define PINT_HIGH_LEVEL		kPINT_PinIntEnableHighLevel

static const gpio_pin_config_t configOutput = {
    		kGPIO_DigitalOutput,
			1,
};

static const gpio_pin_config_t configInput = {
		kGPIO_DigitalInput,
		0,
};

/*  Class Definition  */

class Pin {

	public:

		Pin(uint32_t pin_index, const uint32_t mode);
		Pin(uint32_t pin_index, const uint32_t mode, const uint32_t initial_value);
		uint32_t read();
		void write(const uint32_t value);
		void set(void);
		void clear(void);
		void toggle(void);
		void attachInterrupt(void (*f)(void), pint_pin_enable_t pint_mode, uint32_t pint_pin_index);


	private:
		uint8_t port;
		uint8_t	pin;

		void init(uint32_t pin_index, const uint32_t mode);
};

/*  Extra function prototypes  */

void pint_callback(pint_pin_int_t pintr, uint32_t pmatch_status);

#endif /* GPIO_H_ */
