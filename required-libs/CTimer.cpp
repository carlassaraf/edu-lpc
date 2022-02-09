/*
 * CTimer.cpp
 *
 *  Created on: Nov 12, 2021
 *      Author: fabri
 */

#include "CTimer.h"

/* CTimer IRQ handler pointer */
/* Match0, Match1, Match2, Match3, Capture0, Capture1, Capture2, Capture3 */
ctimer_callback_t ctimer_callbacks[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

/*!
 * @brief CTimer constructor.

 * Creates a CTimer object.
 *
 * @param match_channel to use (0-3). Defaults to 0.
 *
 * @retval None.
 */
CTimer::CTimer(uint32_t match_channel) {
	/* Override match channel */
	settings.match_channel = (ctimer_match_t)match_channel;
	/* Enable CTIMER CLK */
	SYSCON->SYSAHBCLKCTRL0 |= SYSCON_SYSAHBCLKCTRL0_CTIMER_MASK;
	/* Reset the CTIMER */
	SYSCON->PRESETCTRL0 &= ~(SYSCON_PRESETCTRL0_CTIMER_RST_N_MASK);
	/* Clear the reset */
	SYSCON->PRESETCTRL0 |= SYSCON_PRESETCTRL0_CTIMER_RST_N_MASK;
	/* CTimer initialization */
	CTIMER_Init(CTIMER0, &settings.ctimer_config);
	/* CTimer match setup */
    CTIMER_SetupMatch(CTIMER0, settings.match_channel, &settings.match_config);
}

/*!
 * @brief CTimer outputPin method.

 * Configures a pin as an output for the match event.
 *
 * @param match_output_pin index of the pin to be used.
 *
 * @retval None.
 */
void CTimer::outputPin(uint8_t match_output_pin) {
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
 * @brief CTimer frequency method.

 * Sets the match register to trigger the event with
 * the desired frequency..
 *
 * @param frequency times per second that the match has to happen.
 *
 * @retval None.
 */
void CTimer::frequency(uint32_t freq) {
	/* Get the CTIMER running frequency */
	uint32_t ctimer_freq = CLOCK_GetFreq(kCLOCK_CoreSysClk) / (CTIMER0->PR + 1);
	/* Set match value */
	setMatch(ctimer_freq / frequency);
}

/*!
 * @brief CTimer attachInterrupt method.

 * Attaches a function to the match interrupt.
 *
 * @param f function to be called.
 *
 * @retval None.
 */
void CTimer::attachInterrupt(void (*f)(uint32_t)) {
	/* Update settings */
	settings.match_config.enableInterrupt = true;
	/* Enable appropriate interrupt */
	CTIMER0->MCR |= 1 << (settings.match_channel * 3U);
	(void)EnableIRQ(CTIMER_IRQS);
	/* Add callback function to the callback array */
	ctimer_callbacks[settings.match_channel] = f;
	/* Register the callback */
	CTIMER_RegisterCallBack(CTIMER0, &ctimer_callbacks[0], kCTIMER_MultipleCallback);
}
