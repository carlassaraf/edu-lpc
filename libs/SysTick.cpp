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
	/* Save function in pointer */
	isr_handler = f;
	/* Calculate the number of ticks to match */
	uint32_t ticks = SystemCoreClock * (us / 1E6);
    /* Configure SysTick */
	SysTick_Config(ticks);
}

/* SysTick IRQ handler */
extern "C" {
    /* Overwrite default handler */
    void SysTick_Handler(void) {
        /* Check if there is a function */
        if (*isr_handler) {
            /* Call function */
            (*isr_handler)();
        }
    }
}