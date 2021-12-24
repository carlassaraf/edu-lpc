/*
 * CTimer.cpp
 *
 *  Created on: Nov 12, 2021
 *      Author: fabri
 */

#include "Timer.h"

/*  CTimer IRQ handler pointer  */
/*  Match0, Match1, Match2, Match3, Capture0, Capture1, Capture2, Capture3  */
ctimer_callback_t ctimer_callbacks[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

/*!
 * @brief CTimer constructor.

 * Creates a CTimer object.
 *
 * @param match_channel to use (0-3).
 *
 * @retval None.
 */
CTimer::CTimer(uint32_t match_channel) {

	matchChannel = match_channel;

	SYSCON->SYSAHBCLKCTRL0 |= SYSCON_SYSAHBCLKCTRL0_CTIMER_MASK;	// Enable CTIMER CLK

	SYSCON->PRESETCTRL0 &= ~(SYSCON_PRESETCTRL0_CTIMER_RST_N_MASK);	// Reset the CTIMER
	SYSCON->PRESETCTRL0 |= SYSCON_PRESETCTRL0_CTIMER_RST_N_MASK;	// Clear the reset

	ctimer_config_t ctimer_config;

    CTIMER_GetDefaultConfig(&ctimer_config);
	CTIMER_Init(CTIMER0, &ctimer_config);

	ctimer_match_config_t match_config;		// Default Match Configuration

    match_config.enableCounterReset = true;
    match_config.enableCounterStop  = false;
    match_config.matchValue         = 0xffffffff;
    match_config.outControl         = kCTIMER_Output_Toggle;
    match_config.outPinInitState    = true;
    match_config.enableInterrupt    = false;

    CTIMER_SetupMatch(CTIMER0, (ctimer_match_t)match_channel, &match_config);
}

/*!
 * @brief CTimer start method.

 * Starts the counter.
 *
 * @param None.
 *
 * @retval None.
 */
void CTimer::start(void) {
	CTIMER_StartTimer(CTIMER0);
}

/*!
 * @brief CTimer stop method.

 * Stops the counter.
 *
 * @param None.
 *
 * @retval None.
 */
void CTimer::stop(void) {
	CTIMER_StopTimer(CTIMER0);
}

/*!
 * @brief CTimer setOutputPin method.

 * Configures a pin as an output for the match event.
 *
 * @param match_output_pin index of the pin to be used.
 *
 * @retval None.
 */
void CTimer::setOutputPin(uint32_t match_output_pin) {

	SYSCON->SYSAHBCLKCTRL0 	|= SYSCON_SYSAHBCLKCTRL0_SWM_MASK;		// Enable Switch Matrix

	if (matchChannel < 4) {
		SWM0->PINASSIGN_DATA[13] = match_output_pin << (SWM_PINASSIGN13_T0_MAT0_SHIFT + matchChannel * 8U);
	}
	else if(matchChannel == 3) {
		SWM0->PINASSIGN_DATA[14] = match_output_pin;
	}

	SYSCON->SYSAHBCLKCTRL0	&= ~(SYSCON_SYSAHBCLKCTRL0_SWM_MASK);	// Enable Switch Matrix
}

/*!
 * @brief CTimer setMatch method.

 * Sets a new value to the counter.
 *
 * @param match_value counter value.
 *
 * @retval None.
 */
void CTimer::setMatch(uint32_t match_value) {

	CTIMER0->MR[matchChannel] = match_value;
	CTIMER0->TC = 0;
}

/*!
 * @brief CTimer setOutputNoAction method.

 * Configures the output pin behavior to none.
 *
 * @param None.
 *
 * @retval None.
 */
void CTimer::setOutputNoAction(void) {
	CTIMER0->EMR &= ~(CTIMER_EMR_EMC0_MASK << matchChannel);								// Clear previous state
}

/*!
 * @brief CTimer setOutputToggle method.

 * Configures the output pin behavior to toggle.
 *
 * @param None.
 *
 * @retval None.
 */
void CTimer::setOutputToggle(void) {
	CTIMER0->EMR &= ~(CTIMER_EMR_EMC0_MASK << matchChannel);								// Clear previous state
	CTIMER0->EMR |= kCTIMER_Output_Toggle << (matchChannel * 2U + CTIMER_EMR_EMC0_SHIFT);	// Set functionality
}

/*!
 * @brief CTimer setOutputClear method.

 * Configures the output pin behavior to clear.
 *
 * @param None.
 *
 * @retval None.
 */
void CTimer::setOutputClear(void) {
	CTIMER0->EMR &= ~(CTIMER_EMR_EMC0_MASK << matchChannel);								// Clear previous state
	CTIMER0->EMR |= kCTIMER_Output_Clear << (matchChannel * 2U + CTIMER_EMR_EMC0_SHIFT);	// Set functionality
}

/*!
 * @brief CTimer setOutputSet method.

 * Configures the output pin behavior to set.
 *
 * @param None.
 *
 * @retval None.
 */
void CTimer::setOutputSet(void) {
	CTIMER0->EMR &= ~(CTIMER_EMR_EMC0_MASK << matchChannel);							// Clear previous state
	CTIMER0->EMR |= kCTIMER_Output_Set << (matchChannel * 2U + CTIMER_EMR_EMC0_SHIFT);	// Set functionality
}

/*!
 * @brief CTimer setFrequency method.

 * Sets the match register to trigger the event with
 * the desired frequency..
 *
 * @param frequency times per second that the match has to happen.
 *
 * @retval None.
 */
void CTimer::setFrequency(uint32_t freq) {

	uint32_t ctimer_freq = CLOCK_GetFreq(kCLOCK_CoreSysClk) / (CTIMER0->PR + 1);	// Get the CTIMER running frequency
	setMatch(ctimer_freq / freq);
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

	CTIMER0->MCR |= 1 << (matchChannel * 3U);											// Enable appropiate interrupt
	(void)EnableIRQ(CTIMER_IRQS);
	ctimer_callbacks[matchChannel] = f;													// Add callback function to the callback array
	CTIMER_RegisterCallBack(CTIMER0, &ctimer_callbacks[0], kCTIMER_MultipleCallback);	// Register the callback
}
