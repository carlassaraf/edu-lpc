/*
 * SysTick.cpp
 *
 *  Created on: Dec 6, 2021
 *      Author: fabri
 */

#include <SysTick.h>

/*  SysTick IRQ handler pointer  */
static void (*isr_handler[1])(void);

/*!
 * @brief Tick constructor.

 * Creates an Tick object.
 *
 * @param us time in microseconds to set the interrupt.
 * @param f function to be called when interrupt triggers.
 *
 * @retval None.
 */
Tick::Tick(uint32_t us, void (*f)(void)) {

	tickCounter = 0;
	isr_handler[0] = f;

	uint32_t ticks = SystemCoreClock * (us / 1E6);
	SysTick_Config(ticks);
}

/*!
 * @brief Tick getTickCounter method.

 * Returns the counter value.
 *
 * @param None.
 *
 * @retval counter value.
 */
uint32_t Tick::getTickCounter(void) {
	return tickCounter;
}

/*  SysTick IRQ handler  */

extern "C" {

    void SysTick_Handler(void) {

        if (isr_handler[0]) {
            isr_handler[0]();
        }
    }
}