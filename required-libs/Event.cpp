/*
 * Event.cpp
 *
 *  Created on: Feb 10, 2022
 *      Author: fabri
 */
#include "Event.h"

/*!
 * @brief Event constructor.

 * Creates an Event object.
 *
 * @param ctimer pointer to the CTimer instance
 * to use.
 *
 * @retval None.
 */
Event::Event(CTimer *ctimer) {
	/* Get the pointer for the CTimer */
	timer = ctimer;
	/* Get the current match */
	settings.match_channel = timer->currentMatch();
	/* Create a new match event */
	timer->newMatch();
	/* Match setup */
	CTIMER_SetupMatch(CTIMER0, settings.match_channel, &settings.match_config);
}

/*!
 * @brief Event outputPin method.

 * Configures a pin as an output for the match event.
 *
 * @param match_output_pin index of the pin to be used.
 *
 * @retval None.
 */
void Event::outputPin(uint8_t match_output_pin) {
	/* Enable switch matrix */
	SYSCON->SYSAHBCLKCTRL0 	|= SYSCON_SYSAHBCLKCTRL0_SWM_MASK;
	/* Connect pin to match output channel */
	if (settings.match_channel < 4) {
		SWM0->PINASSIGN_DATA[13] = match_output_pin << (SWM_PINASSIGN13_T0_MAT0_SHIFT + settings.match_channel * 8U);
	}
	else if(settings.match_channel == 3) {
		SWM0->PINASSIGN_DATA[14] = match_output_pin;
	}
	/* Disable switch matric */
	SYSCON->SYSAHBCLKCTRL0	&= ~(SYSCON_SYSAHBCLKCTRL0_SWM_MASK);
}

/*!
 * @brief Event frequency method.

 * Sets the match register to trigger the event with
 * the desired frequency..
 *
 * @param frequency times per second that the match has to happen.
 *
 * @retval None.
 */
void Event::frequency(uint32_t frequency) {
	/* Get the CTIMER running frequency */
	uint32_t ctimer_freq = CLOCK_GetFreq(kCLOCK_CoreSysClk) / (CTIMER0->PR + 1);
	/* Set match */
	match(ctimer_freq / frequency);
}

/*!
 * @brief Event attachInterrupt method.

 * Attaches a function to the match interrupt.
 *
 * @param f function to be called.
 *
 * @retval None.
 */
void Event::attachInterrupt(void (*f)(uint32_t)) {
	/* Update settings */
	settings.match_config.enableInterrupt = true;
	/* Enable appropriate interrupt */
	CTIMER0->MCR |= 1 << (settings.match_channel * 3U);
	(void)EnableIRQ(CTIMER_IRQS);
	/* Add callback function to the callback array */
	timer->callbacks()[settings.match_channel] = f;
	/* Register the callback */
	CTIMER_RegisterCallBack(CTIMER0, timer->callbacks(), kCTIMER_MultipleCallback);
}


