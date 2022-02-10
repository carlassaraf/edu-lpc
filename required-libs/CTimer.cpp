/*
 * CTimer.cpp
 *
 *  Created on: Nov 12, 2021
 *      Author: fabri
 */

#include <CTimer.h>

/* Initialization of the callbacks array */
ctimer_callback_t CTimer::ctimer_callbacks[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

/*!
 * @brief CTimer constructor.

 * Creates a CTimer object.
 *
 * @retval None.
 */
CTimer::CTimer(void) {
	/* Initialize match channel */
	current_match_channel = kCTIMER_Match_0;
	/* Enable CTIMER CLK */
	SYSCON->SYSAHBCLKCTRL0 |= SYSCON_SYSAHBCLKCTRL0_CTIMER_MASK;
	/* Reset the CTIMER */
	SYSCON->PRESETCTRL0 &= ~(SYSCON_PRESETCTRL0_CTIMER_RST_N_MASK);
	/* Clear the reset */
	SYSCON->PRESETCTRL0 |= SYSCON_PRESETCTRL0_CTIMER_RST_N_MASK;
	/* CTimer initialization */
	CTIMER_Init(CTIMER0, &settings);
}
