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

/* GPIO inline functions */
inline uint8_t getPORT(uint32_t pin) {
	return (pin < 32)? 0 : 1;
}

inline syscon_connection_t getSYSCON_CONNECTION(uint32_t pin) {
	return (syscon_connection_t)(pin + (PINTSEL_ID << SYSCON_SHIFT));
}

/* Class Definition */
class Pin {

	public:
		/* Class constants */
		static constexpr uint8_t INPUT {(uint8_t)kGPIO_DigitalInput};
		static constexpr uint8_t OUTPUT {(uint8_t)kGPIO_DigitalOutput};

		static constexpr uint8_t LOW {0};
		static constexpr uint8_t HIGH {1};

		/* Configuration constants */
		static constexpr gpio_pin_config_t configOutput = {
		    		kGPIO_DigitalOutput,
					1,
		};

		static constexpr gpio_pin_config_t configInput = {
				kGPIO_DigitalInput,
				0,
		};

		/* PINT related constants */
		static constexpr pint_pin_enable_t PINT_NONE = kPINT_PinIntEnableNone;
		static constexpr pint_pin_enable_t PINT_RISE_EDGE = kPINT_PinIntEnableRiseEdge;
		static constexpr pint_pin_enable_t PINT_FALL_EDGE = kPINT_PinIntEnableFallEdge;
		static constexpr pint_pin_enable_t PINT_BOTH_EDGES = kPINT_PinIntEnableBothEdges;
		static constexpr pint_pin_enable_t PINT_LOW_LEVEL = kPINT_PinIntEnableLowLevel;
		static constexpr pint_pin_enable_t PINT_HIGH_LEVEL = kPINT_PinIntEnableHighLevel;

		/* Constructors */
		Pin(uint32_t pin_index, const uint32_t mode);
		Pin(uint32_t pin_index, const uint32_t mode, const uint32_t initial_value);
		/* Public methods */
		uint32_t read();
		void write(const uint32_t value);
		void set(void);
		void clear(void);
		void toggle(void);
		void attachInterrupt(void (*f)(void), pint_pin_enable_t pint_mode, uint32_t pint_pin_index);


	private:
		/* Index of the port associated with the Pin */
		uint8_t port;
		/* Pin index 0-31 */
		uint8_t	pin;
		/* General initialization private method */
		void init(uint32_t pin_index, const uint32_t mode);
};

/* Inline class methods */

/*!
 * @brief Pin read method.

 * Returns the digital value of the pin.
 *
 * @param None.
 *
 * @retval pin value.
 */
inline uint32_t Pin::read(void) {

	return GPIO->B[port][pin];
}

/*!
 * @brief Pin write method.

 * Writes a desired value in the output pin.
 *
 * @param value to be written in the output pin.
 *
 * @retval None.
 */
inline void Pin::write(const uint32_t value) {

	GPIO->B[port][pin] = value;
}

/*!
 * @brief Pin set method.

 * Sets the output pin.
 *
 * @param None.
 *
 * @retval None.
 */
inline void Pin::set(void) {

	GPIO->SET[port] |= 1UL << pin;
}

/*!
 * @brief Pin clear method.

 * Clears the output pin.
 *
 * @param None.
 *
 * @retval None.
 */
inline void Pin::clear(void) {

	GPIO->CLR[port] |= 1UL << pin;
}

/*!
 * @brief Pin toggle method.

 * Toggles the output pin.
 *
 * @param None.
 *
 * @retval None.
 */
inline void Pin::toggle(void) {

	GPIO->NOT[port] |= 1UL << pin;
}

/* Extra function prototypes */

void pint_callback(pint_pin_int_t pintr, uint32_t pmatch_status);

#endif /* GPIO_H_ */
