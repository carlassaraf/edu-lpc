/*
 * SysTick.cpp
 *
 *  Created on: Dec 6, 2021
 *      Author: fabri
 */

#include <SysTick.h>

static void (*isr_handler[1])(void);

Tick::Tick(uint32_t us, void (*f)(void)) {

	tickCounter = 0;
	isr_handler[0] = f;

	uint32_t ticks = SystemCoreClock * (us / 1E6);
	SysTick_Config(ticks);
}

uint32_t Tick::getTickCounter(void) {
	return tickCounter;
}

void SysTick_Handler(void) {

	if (isr_handler[0]) {
		isr_handler[0]();
	}
}
