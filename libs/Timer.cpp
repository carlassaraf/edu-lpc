/*
 * Timer.cpp
 *
 *  Created on: Nov 12, 2021
 *      Author: fabri
 */

#include "Timer.h"

ctimer_callback_t ctimer_callbacks[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

CTimer_Match::CTimer_Match(uint32_t match_channel) {

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

CTimer_Match::~CTimer_Match() {
	// TODO Auto-generated destructor stub
}

void CTimer_Match::start(void) {
	CTIMER_StartTimer(CTIMER0);
}

void CTimer_Match::stop(void) {
	CTIMER_StopTimer(CTIMER0);
}

void CTimer_Match::setOutputPin(uint32_t match_output_pin) {

	SYSCON->SYSAHBCLKCTRL0 	|= SYSCON_SYSAHBCLKCTRL0_SWM_MASK;		// Enable Switch Matrix

	if (matchChannel < 4) {
		SWM0->PINASSIGN_DATA[13] = match_output_pin << (SWM_PINASSIGN13_T0_MAT0_SHIFT + matchChannel * 8U);
	}
	else if(matchChannel == 3) {
		SWM0->PINASSIGN_DATA[14] = match_output_pin;
	}

	SYSCON->SYSAHBCLKCTRL0	&= ~(SYSCON_SYSAHBCLKCTRL0_SWM_MASK);	// Enable Switch Matrix
}

void CTimer_Match::setMatch(uint32_t match_value) {

	CTIMER0->MR[matchChannel] = match_value;
	CTIMER0->TC = 0;
}

void CTimer_Match::setOutputToggle(void) {
	CTIMER0->EMR &= ~(CTIMER_EMR_EMC0_MASK << matchChannel);								// Clear previous state
	CTIMER0->EMR |= kCTIMER_Output_Toggle << (matchChannel * 2U + CTIMER_EMR_EMC0_SHIFT);	// Set functionality
}

void CTimer_Match::setOutputClear(void) {
	CTIMER0->EMR &= ~(CTIMER_EMR_EMC0_MASK << matchChannel);								// Clear previous state
	CTIMER0->EMR |= kCTIMER_Output_Clear << (matchChannel * 2U + CTIMER_EMR_EMC0_SHIFT);	// Set functionality
}

void CTimer_Match::setOutputSet(void) {
	CTIMER0->EMR &= ~(CTIMER_EMR_EMC0_MASK << matchChannel);							// Clear previous state
	CTIMER0->EMR |= kCTIMER_Output_Set << (matchChannel * 2U + CTIMER_EMR_EMC0_SHIFT);	// Set functionality
}

void CTimer_Match::setFrequency(uint32_t freq) {

	uint32_t ctimer_freq = CLOCK_GetFreq(kCLOCK_CoreSysClk) / (CTIMER0->PR + 1);	// Get the CTIMER running frequency
	setMatch(ctimer_freq / freq);
}

void CTimer_Match::attachInterrupt(void (*f)(uint32_t)) {

	CTIMER0->MCR |= 1 << (matchChannel * 3U);											// Enable appropiate interrupt
	(void)EnableIRQ(CTIMER_IRQS);
	ctimer_callbacks[matchChannel] = f;													// Add callback function to the callback array
	CTIMER_RegisterCallBack(CTIMER0, &ctimer_callbacks[0], kCTIMER_MultipleCallback);	// Register the callback
}
