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
Pin::Pin(uint8_t pin_index, const uint8_t mode) {
	/* Call Pin initialization */
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
Pin::Pin(uint8_t pin_index, const uint8_t mode, const uint8_t initial_value) {
	/* Call Pin initialization */
	init(pin_index, mode);
	/* Set initial value */
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
void Pin::init(uint8_t pin_index, const uint8_t mode) {
	/* Get the pin index */
	pin = pin_index;
	/* Get the PORT number */
	port = getPORT(pin);
	/* Enable clock for GPIO1 */
	if (port == 1) { SYSCON->SYSAHBCLKCTRL0 |= SYSCON_SYSAHBCLKCTRL0_GPIO1_MASK; }
	/* Enable clock for GPIO0 */
	else { SYSCON->SYSAHBCLKCTRL0 |= SYSCON_SYSAHBCLKCTRL0_GPIO0_MASK; }
	/* Adjust, if necessary, the pin index value */
	if (pin > 31) { pin -= 32; }
	/* Initialize Pin as INPUT */
	if (mode == INPUT) { GPIO->DIR[port] &= ~(1UL << pin); }
	/* Initialize Pin as OUTPUT */
	else { GPIO->DIR[port] |= 1UL << pin; }
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
void Pin::attachInterrupt(void (*f)(void), pint_pin_enable_t pint_mode, uint8_t pint_pin_index) {
	/* Get the original pin index */
	uint8_t pin_index = (port == 0)? pin : pin + 32;
	/* Connect the Pin to the proper PINT channel */
	SYSCON_AttachSignal(SYSCON, (pint_pin_int_t)pint_pin_index, getSYSCON_CONNECTION(pin_index));
	/* Store the function pointer */
	pint_irq_ptr[pint_pin_index] = f;
	/* Enable PINT module */
	PINT_Init(PINT);
	/* PINT configuration */
	PINT_PinInterruptConfig(PINT, (pint_pin_int_t)pint_pin_index, pint_mode, pint_callback);
	/* Enable callback */
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
void pint_callback(pint_pin_int_t pintr, uint8_t pmatch_status) {
	/* Check if there is a function for that index */
	if(pint_irq_ptr[pintr]) {
		/* Call function */
		pint_irq_ptr[pintr]();
	}
}
