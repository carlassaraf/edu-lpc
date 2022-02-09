/*
 * PWM.cpp
 *
 *  Created on: Dec 24, 2021
 *      Author: fabri
 */

#include <PWM.h>

/* SCTimer object for every PWM channel */
static SCTimer *sctimer = new SCTimer();
/* Initialize static members */
uint8_t PWM::s_currentEvent = 0;
uint8_t PWM::s_currentMatch = 0;
sctimer_out_t PWM::s_currentOutput = kSCTIMER_Out_0;

/*!
 * @brief PWM constructor.

 * Creates a PWM object.
 *
 * @param outputPin index of the pin to be used.
 * @param frequency PWM signal frequency in Hz. Defaults to 1000 Hz.
 * @param duty PWM duty cycle in percentage. Defaults to 50%.
 *
 * @retval None.
 */
PWM::PWM(uint8_t pin, uint32_t frequency, uint8_t duty) {
	/* Update settings with the user arguments */
	settings.outputPin = pin;
	settings.config.output = s_currentOutput;
	settings.config.dutyCyclePercent = duty;
	/* Increment SCT Output for the next PWM */
	int out = (int)s_currentOutput;
	s_currentOutput = (sctimer_out_t)(out + 1);
    /* Configure output pin */
	setOutputPin(pin);
	/* Initialize PWM with the current settings */
	init(frequency);
}

/*!
 * @brief PWM frequency method.

 * Sets the PWM frequency.
 *
 * @param frequency PWM frequency in Hz.
 *
 * @retval None.
 */
void PWM::frequency(uint32_t frequency) {
	/* Update frequency on settings */
	settings.frequency = frequency;
	/* Update values to overwrite events */
	s_currentEvent = periodEvent;
	s_currentMatch = periodEvent;
	/* Overwrite the original PWM values */
	init(frequency);
}

/*!
 * @brief PWM duty method.

 * Sets the PWM duty cycle.
 *
 * @param duty PWM duty cycle in percentage.
 *
 * @retval None.
 */
void PWM::duty(uint8_t duty) {
	/* Set duty to 100% if it was greater */
	if(duty > 100) { duty = 100; }
	/* Set duty to 0% if it was lesser */
	else if(duty < 0) { duty = 0; }
	/* Update duty in settings */
	settings.config.dutyCyclePercent = duty;
	/* Auxiliary variables */
    uint32_t periodMatchReg, pulseMatchReg;
    uint32_t pulsePeriod = 0, period;
    /* Retrieve the match register number for the PWM period */
    periodMatchReg = SCT0->EV[periodEvent].CTRL & SCT_EV_CTRL_MATCHSEL_MASK;
    /* Retrieve the match register number for the PWM pulse period */
    pulseMatchReg = SCT0->EV[pulseEvent].CTRL & SCT_EV_CTRL_MATCHSEL_MASK;
    /* Get period value */
    period = SCT0->MATCH[periodMatchReg];
    /* Calculate pulse width and period match value */
    pulsePeriod = (uint32_t)(((uint64_t)period * duty) / 100U);
 	/* "pulsePeriod = period - 1U" results in 100% dutycyle. */
    if (duty == 100U) { pulsePeriod = period - 1U; }
    /* Stop the counter */
	SCT0->CTRL |= (SCT_CTRL_HALT_L_MASK);
    /* Update dutycycle */
    SCT0->MATCH[pulseMatchReg]    = SCT_MATCH_MATCHn_L(pulsePeriod);
    SCT0->MATCHREL[pulseMatchReg] = SCT_MATCHREL_RELOADn_L(pulsePeriod);
    /* Restart the counter */
	SCT0->CTRL &= ~(SCT_CTRL_HALT_L_MASK);
}

/*!
 * @brief PWM logicLevel method.

 * Sets the PWM logic level.
 *
 * @param logic polarity of the PWM.
 * - false: Low level PWM
 * - true: High level PWM
 *
 * @retval None.
 */
void PWM::logicLevel(bool logic) {
	/* Update logic level in settings */
	settings.config.level = (sctimer_pwm_level_select_t)logic;
	/* Overwrite PWM values */
	init(settings.frequency);
}

/*!
 * @brief PWM outputPin private method.

 * Sets the PWM output pin.
 *
 * @param pin index of the pin to be used.
 *
 * @retval None.
 */
void PWM::outputPin(uint8_t pin) {
	/* Update outputPin in settings */
	settings.outputPin = pin;
	/* Enable SWM clock */
    CLOCK_EnableClock(kCLOCK_Swm);
    /* Move PWM function */
    SWM_SetMovablePinSelect(SWM0, (swm_select_movable_t)(settings.config.output + kSWM_SCT_OUT0), (swm_port_pin_type_t)pin);
    /* Disable SWM clock */
    CLOCK_DisableClock(kCLOCK_Swm);
}

/*!
 * @brief PWM init private method.

 * Deals with the initialization of the SCTimer and PWM.
 *
 * @param frequency PWM signal frequency in Hz.
 *
 * @retval None.
 */
void PWM::init(uint32_t frequency, bool logic) {
	/* Update frequency in settings if it's different */
	if(settings.frequency != frequency) { settings.frequency = frequency; }
	/* Stop counter before configuring another PWM */
	SCT0->CTRL |= (SCT_CTRL_HALT_L_MASK);
    /* Auxiliary variables */
    uint32_t period, pulsePeriod = 0;
    uint32_t sctimerClock = CLOCK_GetFreq(kCLOCK_Fro);
    uint32_t sctClock    = sctimerClock / (((SCT0->CTRL & SCT_CTRL_PRE_L_MASK) >> SCT_CTRL_PRE_L_SHIFT) + 1U);
    uint32_t reg;
    /* Check if there are enough events */
    if ((s_currentEvent + 2U) > (uint32_t)FSL_FEATURE_SCT_NUMBER_OF_EVENTS) { return; }
	/* Use bi-directional mode for center-aligned PWM */
    SCT0->CTRL |= SCT_CTRL_BIDIR_L_MASK;
	/* Calculate PWM period match value */
	period = sctClock / (frequency * 2U);
	/* Calculate pulse width and period match value */
	pulsePeriod = (uint32_t)(((uint64_t)period * settings.config.dutyCyclePercent) / 100U);
	/* "pulsePeriod = period - 1U" results in 100% dutycyle */
	if (settings.config.dutyCyclePercent >= 100U) { pulsePeriod = period - 1U; }
	/* Schedule an event when we reach the PWM period */
	periodEvent = createEvent(period);
	/* Schedule an event when we reach the pulse width */
	pulseEvent = createEvent(pulsePeriod);
	/* Reset the counter when we reach the PWM period */
	//SCTIMER_SetupCounterLimitAction(SCT0, kSCTIMER_Counter_U, periodEvent);
	SCT0->LIMIT |= SCT_LIMIT_LIMMSK_L(1UL << periodEvent);
	/* For high-true level */
	if ((bool)settings.config.level) {
		/* Set the initial output level to low which is the inactive state */
		SCT0->OUTPUT &= ~(1UL << (uint32_t)settings.config.output);
		/* Clear the output when we reach the PWM pulse event */
		SCT0->OUT[settings.config.output].CLR |= (1UL << pulseEvent);
	}
	/* For low-true level */
	else {
		/* Set the initial output level to high which is the inactive state */
		SCT0->OUTPUT |= (1UL << (uint32_t)settings.config.output);
		/* Set the output when we reach the PWM pulse event */
		SCT0->OUT[settings.config.output].SET |= (1UL << pulseEvent);
	}
	/* Reverse output when down counting */
	reg = SCT0->OUTPUTDIRCTRL;
	reg &= ~((uint32_t)SCT_OUTPUTDIRCTRL_SETCLR0_MASK << (2U * (uint32_t)settings.config.output));
	reg |= (1UL << (2U * (uint32_t)settings.config.output));
	SCT0->OUTPUTDIRCTRL = reg;
	/* Start Timer counter again */
	SCT0->CTRL &= ~(SCT_CTRL_HALT_L_MASK);
}