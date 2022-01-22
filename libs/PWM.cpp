/*
 * PWM.cpp
 *
 *  Created on: Dec 24, 2021
 *      Author: fabri
 */

#include <PWM.h>

/* SCTimer object for every PWM channel */
SCTimer *sctimer = new SCTimer();
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
PWM::PWM(uint8_t outputPin, uint32_t frequency, uint8_t duty) {
	/* Update settings with the user arguments */
	settings.outputPin = outputPin;
	settings.config.output = s_currentOutput;
	settings.config.dutyCyclePercent = duty;
	/* Increment SCT Output for the next PWM */
	int out = (int)s_currentOutput;
	s_currentOutput = (sctimer_out_t)(out + 1);
    /* Configure output pin */
	setOutputPin(outputPin);
	/* Initialize PWM with the current settings */
	init(frequency);
}

/*!
 * @brief PWM setDuty method.

 * Sets the PWM duty cycle.
 *
 * @param duty PWM duty cycle in percentage.
 *
 * @retval None.
 */
void PWM::setDuty(uint8_t duty) {

	SCTIMER_UpdatePwmDutycycle(SCT0, settings.config.output, duty, event);
}

/*!
 * @brief PWM getDuty method.

 * Gets the current PWM duty cycle.
 *
 * @param None.
 * 
 * @retval PWM duty cycle in percentage.
 */
uint8_t PWM::getDuty(void) {

	return settings.config.dutyCyclePercent;
}

/*!
 * @brief PWM setFrequency method.

 * Sets the PWM frequency.
 *
 * @param frequency PWM frequency in Hz.
 *
 * @retval None.
 */
void PWM::setFrequency(uint32_t frequency) {

	pwmFrequency = frequency;
	
	/* Overwrite the original PWM values */
	s_currentEvent = periodEvent;
	s_currentMatch = periodEvent;
	init(frequency, settings.config.level);
}

/*!
 * @brief PWM getFrequency method.

 * Gets the current PWM frequency.
 *
 * @param None.
 * 
 * @retval PWM frequency in percentage.
 */
uint32_t PWM::getFrequency(void) {

	return pwmFrequency;
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

	settings.config.level = (sctimer_pwm_level_select_t)logic;
	init(pwmFrequency);
}

/*!
 * @brief PWM setOutputPin method.

 * Sets the PWM output pin.
 *
 * @param pin index of the pin to be used.
 *
 * @retval None.
 */
void PWM::setOutputPin(uint8_t pin) {

    CLOCK_EnableClock(kCLOCK_Swm);
    SWM_SetMovablePinSelect(SWM0, (swm_select_movable_t)(settings.config.output + kSWM_SCT_OUT0), (swm_port_pin_type_t)pin);
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

	if(pwmFrequency != frequency) {
		pwmFrequency = frequency;
	}
	/* Stop counter before configuring another PWM */
	SCTIMER_StopTimer(SCT0, kSCTIMER_Counter_U);

    /* If we do not have enough events available (this function will create two events),
     * the function will return fail.
     */
    uint32_t period, pulsePeriod = 0;
    uint32_t sctimerClock = CLOCK_GetFreq(kCLOCK_Fro);
    uint32_t sctClock    = sctimerClock / (((SCT0->CTRL & SCT_CTRL_PRE_L_MASK) >> SCT_CTRL_PRE_L_SHIFT) + 1U);
    uint32_t reg;

    if ((s_currentEvent + 2U) > (uint32_t)FSL_FEATURE_SCT_NUMBER_OF_EVENTS) {
    	return;
    }

	/* Use bi-directional mode for center-aligned PWM */
	SCT0->CTRL |= SCT_CTRL_BIDIR_L_MASK;

	/* Calculate PWM period match value */
	period = sctClock / (frequency * 2U);

	/* Calculate pulse width and period match value:
	 * For EdgeAlignedPwm, "pulsePeriod = 0" results in 0% dutycyle, "pulsePeriod = period - 1U" results in 100%
	 * dutycyle. For CenterAlignedPwm, , "pulsePeriod = 0" results in 0% dutycyle, "pulsePeriod = period + 2U"
	 * results in 100% dutycyle.
	 */

	pulsePeriod = (uint32_t)(((uint64_t)period * settings.config.dutyCyclePercent) / 100U);

	if (settings.config.dutyCyclePercent >= 100U) {
		pulsePeriod = period - 1U;
	}

	/* Schedule an event when we reach the PWM period */
	periodEvent = createEvent(period, kSCTIMER_Counter_U);
	/* Schedule an event when we reach the pulse width */
	pulseEvent = createEvent(pulsePeriod, kSCTIMER_Counter_U);
	/* Reset the counter when we reach the PWM period */
	SCTIMER_SetupCounterLimitAction(sc, kSCTIMER_Counter_U, periodEvent);

	/* For high-true level */
	if ((uint32_t)settings.config.level == (uint32_t)kSCTIMER_HighTrue) {
		/* Set the initial output level to low which is the inactive state */
		SCT0->OUTPUT &= ~(1UL << (uint32_t)settings.config.output);			/* Clear the output when we reach the PWM pulse event */
		SCTIMER_SetupOutputClearAction(sc, (uint32_t)settings.config.output, pulseEvent);
	}
	/* For low-true level */
	else {
		/* Set the initial output level to high which is the inactive state */
		SCT0->OUTPUT |= (1UL << (uint32_t)settings.config.output);
		/* Set the output when we reach the PWM pulse event */
		SCTIMER_SetupOutputSetAction(sc, (uint32_t)settings.config.output, pulseEvent);
	}

	/* Reverse output when down counting */
	reg = SCT0->OUTPUTDIRCTRL;
	reg &= ~((uint32_t)SCT_OUTPUTDIRCTRL_SETCLR0_MASK << (2U * (uint32_t)settings.config.output));
	reg |= (1UL << (2U * (uint32_t)settings.config.output));
	SCT0->OUTPUTDIRCTRL = reg;

	/* Start Timer counter again */
	SCTIMER_StartTimer(sc, kSCTIMER_Counter_U);
}

/*!
 * @brief PWM createEvent private method.

 * Creates two new match events for the SCTimer. The first to
 * define what the counter will do when it reaches the PWM period
 * and the other when it reaches the match value.
 *
 * @param matchValue value to be reached by the counter and trigger
 * the event.
 *
 * @retval event index.
 */
uint8_t PWM::createEvent(uint32_t matchValue) {

    uint32_t currentCtrlVal = (uint32_t)kSCTIMER_MatchEventOnly;

    if (s_currentEvent < (uint32_t)FSL_FEATURE_SCT_NUMBER_OF_EVENTS) {

		if ((kSCTIMER_Counter_L == whichCounter) && (0U == (SCT0->CONFIG & SCT_CONFIG_UNIFY_MASK))) {

			if (s_currentMatch < (uint32_t)FSL_FEATURE_SCT_NUMBER_OF_MATCH_CAPTURE) {

				currentCtrlVal |= SCT_EV_CTRL_MATCHSEL(s_currentMatch);

				/* Use Counter_L bits if user wants to setup the Low counter */
				SCT0->MATCH_ACCESS16BIT[s_currentMatch].MATCHL       = (uint16_t)matchValue;
				SCT0->MATCHREL_ACCESS16BIT[s_currentMatch].MATCHRELL = (uint16_t)matchValue;
				SCT0->EV[s_currentEvent].CTRL                        = currentCtrlVal;

				/* Increment the match register number */
				s_currentMatch++;
			}
		}
		else if ((kSCTIMER_Counter_H == whichCounter) && (0U == (SCT0->CONFIG & SCT_CONFIG_UNIFY_MASK))) {

			if (s_currentMatchhigh < (uint32_t)FSL_FEATURE_SCT_NUMBER_OF_MATCH_CAPTURE) {

				currentCtrlVal |= SCT_EV_CTRL_MATCHSEL(s_currentMatchhigh);

				/* Use Counter_H bits if user wants to setup the High counter */
				currentCtrlVal |= SCT_EV_CTRL_HEVENT(1U);
				SCT0->MATCH_ACCESS16BIT[s_currentMatchhigh].MATCHH       = (uint16_t)matchValue;
				SCT0->MATCHREL_ACCESS16BIT[s_currentMatchhigh].MATCHRELH = (uint16_t)matchValue;

				SCT0->EV[s_currentEvent].CTRL = currentCtrlVal;
				/* Increment the match register number */
				s_currentMatchhigh++;
			}
		}
		else if ((kSCTIMER_Counter_U == whichCounter) && (0U != (SCT0->CONFIG & SCT_CONFIG_UNIFY_MASK))) {

			if (s_currentMatch < (uint32_t)FSL_FEATURE_SCT_NUMBER_OF_MATCH_CAPTURE) {

				/* Use Counter_L bits if counter is operating in 32-bit mode */
				currentCtrlVal |= SCT_EV_CTRL_MATCHSEL(s_currentMatch);

				SCT0->MATCH[s_currentMatch]    = matchValue;
				SCT0->MATCHREL[s_currentMatch] = matchValue;
				SCT0->EV[s_currentEvent].CTRL  = currentCtrlVal;

				/* Increment the match register number */
				s_currentMatch++;
			}
		}
	}
	/* Enable the event in the current state */
	SCT0->EV[s_currentEvent].STATE = (1UL << s_currentState);

	/* Increment the event number */
	s_currentEvent++;
    return s_currentEvent - 1;
}