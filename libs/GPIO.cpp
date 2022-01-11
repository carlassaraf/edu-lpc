/*
 * GPIO.cpp
 *
 *  Created on: Jun 10, 2021
 *      Author: carlassaraf
 */

#include <GPIO.h>

/*  PINT handler pointer  */
void (*pint_irq_ptr[PINT_INPUT_COUNT])(void);

/*!
 * @brief Pin constructor.

 * Creates a digital Pin object.
 *
 * @param pin_index index of the Pin object (0-54).
 * @param mode whether the Pin object will be input or output.
 *
 * @retval None.
 */
Pin::Pin(uint32_t pin_index, const uint32_t mode) {

	init(pin_index, mode);
}

/*!
 * @brief Pin constructor.

 * Creates a digital Pin object.
 *
 * @param pin_index index of the Pin object (0-54).
 * @param mode whether the Pin object will be input or output.
 * @param initial_value output value for the pin.
 *
 * @retval None.
 */
Pin::Pin(uint32_t pin_index, const uint32_t mode, const uint32_t initial_value) {

	init(pin_index, mode);
	GPIO->B[port][pin] = initial_value;
}

/*!
 * @brief Pin initializer.

 * Handles most of the GPIO initialization.
 *
 * @param pin_index index of the Pin object (0-54).
 * @param mode whether the Pin object will be input or output.
 *
 * @retval None.
 */
void Pin::init(uint32_t pin_index, const uint32_t mode) {

	pin = pin_index;
	port = getPORT(pin);

	// Enable GPIO CLK
	if (port == 1) {
		SYSCON->SYSAHBCLKCTRL0 |= SYSCON_SYSAHBCLKCTRL0_GPIO1_MASK;
	}
	else {
		SYSCON->SYSAHBCLKCTRL0 |= SYSCON_SYSAHBCLKCTRL0_GPIO0_MASK;
	}

	// Get the actual pin index within the port
	if (pin > 31) {
		pin -= 32;
	}

	if (mode == INPUT) {
		GPIO->DIR[port] &= ~(1UL << pin);
	}
	else {
		GPIO->DIR[port] |= 1UL << pin;
	}
}

/*!
 * @brief Pin read method.

 * Returns the digital value of the pin.
 *
 * @param None.
 *
 * @retval pin value.
 */
uint32_t Pin::read(void) {

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
void Pin::write(const uint32_t value) {

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
void Pin::set(void) {

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
void Pin::clear(void) {

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
void Pin::toggle(void) {

	GPIO->NOT[port] |= 1UL << pin;
}

/*!
 * @brief Pin attachInterrupt method.

 * Creates an interrupt for the pin.
 *
 * @param f function to be called when the interrupt is triggered.
 * @param pint_mode interrupt trigger mode.
 * @param pint_pin_index interrupt to be triggered.
 *
 * @retval None.
 */
void Pin::attachInterrupt(void (*f)(void), pint_pin_enable_t pint_mode, uint32_t pint_pin_index) {

	uint32_t pin_index = (port == 0)? pin : pin + 32;		// Get the original pin index

	SYSCON_AttachSignal(SYSCON, (pint_pin_int_t)pint_pin_index, getSYSCON_CONNECTION(pin_index));		// Connect the pin to the
																										// proper PINT channel

	pint_irq_ptr[pint_pin_index] = f;	// Save the function pointer

	PINT_Init(PINT);
	PINT_PinInterruptConfig(PINT, (pint_pin_int_t)pint_pin_index, pint_mode, pint_callback);
	PINT_EnableCallbackByIndex(PINT, (pint_pin_int_t)pint_pin_index);									// Enable trigger by channel
}

/*  PINT Callback  */

/*!
 * @brief Common pint callback.

 * Whenever a pin interrupt is triggered, the program comes here.
 *
 * @param pintr pin interrupt triggered.
 * @param pmatch_status not used.
 *
 * @retval None.
 */
void pint_callback(pint_pin_int_t pintr, uint32_t pmatch_status) {

	if(pint_irq_ptr[pintr]) {
		pint_irq_ptr[pintr]();
	}
}
