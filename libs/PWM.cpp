/*
 * PWM.cpp
 *
 *  Created on: Dec 24, 2021
 *      Author: fabri
 */

#include <PWM.h>

SCTimer *sctimer = new SCTimer();	// SCTimer object for every PWM channel

/*!< @brief Keep track of SCTimer event number */
static uint32_t s_currentEvent = 0;

/*!< @brief Keep track of SCTimer unify 32-bit or low 16-bit match/capture register number. */
static uint32_t s_currentMatch = 0;

/*!< @brief Keep track of SCTimer high 16-bit match/capture register number. */
static uint32_t s_currentMatchhigh = 0;

/*!< @brief Keep track of SCTimer state number */
static uint32_t s_currentState = 0;

/*!< @brief Keep track of SCTimer output number */
static sctimer_out_t s_currentOutput = kSCTIMER_Out_0;

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
PWM::PWM(uint32_t pwm_output, uint32_t frequency, uint32_t outputPin, uint32_t duty) {

	pwmParams.output = s_currentOutput;
	pwmParam.level = kSCTIMER_HighTrue;
	pwmParam.dutyCyclePercent = duty;

	pwmFrequency = frequency;

	/* Increment SCT Output for the next PWM */
	int out = (int)s_currentOutput;
	s_currentOutput = (sctimer_out_t)(out + 1);
    
	setOutputPin(outputPin);

	timer = sctimer;

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
void PWM::setDuty(uint32_t duty) {

	SCTIMER_UpdatePwmDutycycle(timer->getSCTimer(), pwmParam.output, duty, event);
}

/*!
 * @brief PWM getDuty method.

 * Gets the current PWM duty cycle.
 *
 * @param None.
 * 
 * @retval PWM duty cycle in percentage.
 */
uint32_t PWM::getDuty(void) {

	return pwmParam.dutyCyclePercent;
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
	init(frequency, pwmParam.level);
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

	pwmParams.level = (sctimer_pwm_level_select_t)logic;
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
void PWM::setOutputPin(uint32_t pin) {

    CLOCK_EnableClock(kCLOCK_Swm);
    SWM_SetMovablePinSelect(SWM0, (swm_select_movable_t)(pwmParams.output + kSWM_SCT_OUT0), (swm_port_pin_type_t)pin);
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

	SCT_Type *sc = timer->getSCTimer();
	/* Stop counter before configuring another PWM */
	SCTIMER_StopTimer(sc, kSCTIMER_Counter_U);

    /* If we do not have enough events available (this function will create two events),
     * the function will return fail.
     */
    uint32_t period, pulsePeriod = 0;
    uint32_t sctimerClock = CLOCK_GetFreq(kCLOCK_Fro);
    uint32_t sctClock    = sctimerClock / (((sc->CTRL & SCT_CTRL_PRE_L_MASK) >> SCT_CTRL_PRE_L_SHIFT) + 1U);
    uint32_t reg;

    if ((s_currentEvent + 2U) > (uint32_t)FSL_FEATURE_SCT_NUMBER_OF_EVENTS) {
    	return;
    }

	/* Use bi-directional mode for center-aligned PWM */
	sc->CTRL |= SCT_CTRL_BIDIR_L_MASK;

	/* Calculate PWM period match value */
	period = sctClock / (frequency * 2U);

	/* Calculate pulse width and period match value:
	 * For EdgeAlignedPwm, "pulsePeriod = 0" results in 0% dutycyle, "pulsePeriod = period - 1U" results in 100%
	 * dutycyle. For CenterAlignedPwm, , "pulsePeriod = 0" results in 0% dutycyle, "pulsePeriod = period + 2U"
	 * results in 100% dutycyle.
	 */

	pulsePeriod = (uint32_t)(((uint64_t)period * pwmParams.dutyCyclePercent) / 100U);

	if (pwmParams.dutyCyclePercent >= 100U) {
		pulsePeriod = period - 1U;
	}

	/* Schedule an event when we reach the PWM period */
	periodEvent = createEvent(period, kSCTIMER_Counter_U);
	/* Schedule an event when we reach the pulse width */
	pulseEvent = createEvent(pulsePeriod, kSCTIMER_Counter_U);
	/* Reset the counter when we reach the PWM period */
	SCTIMER_SetupCounterLimitAction(sc, kSCTIMER_Counter_U, periodEvent);

	/* For high-true level */
	if ((uint32_t)pwmParams.level == (uint32_t)kSCTIMER_HighTrue) {
		/* Set the initial output level to low which is the inactive state */
		sc->OUTPUT &= ~(1UL << (uint32_t)pwmParams.output);			/* Clear the output when we reach the PWM pulse event */
		SCTIMER_SetupOutputClearAction(sc, (uint32_t)pwmParams.output, pulseEvent);
	}
	/* For low-true level */
	else {
		/* Set the initial output level to high which is the inactive state */
		sc->OUTPUT |= (1UL << (uint32_t)pwmParams.output);
		/* Set the output when we reach the PWM pulse event */
		SCTIMER_SetupOutputSetAction(sc, (uint32_t)pwmParams.output, pulseEvent);
	}

	/* Reverse output when down counting */
	reg = sc->OUTPUTDIRCTRL;
	reg &= ~((uint32_t)SCT_OUTPUTDIRCTRL_SETCLR0_MASK << (2U * (uint32_t)pwmParams.output));
	reg |= (1UL << (2U * (uint32_t)pwmParams.output));
	sc->OUTPUTDIRCTRL = reg;

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
 * @param whichCounter whether it is a 16 bit counter, lower or high; or
 * the unified 32 bit counter.
 *
 * @retval event index.
 */
uint32_t PWM::createEvent(uint32_t matchValue, sctimer_counter_t whichCounter) {

	SCT_Type *sc = timer->getSCTimer();
    uint32_t currentCtrlVal = (uint32_t)kSCTIMER_MatchEventOnly;

    if (s_currentEvent < (uint32_t)FSL_FEATURE_SCT_NUMBER_OF_EVENTS) {

		if ((kSCTIMER_Counter_L == whichCounter) && (0U == (sc->CONFIG & SCT_CONFIG_UNIFY_MASK))) {

			if (s_currentMatch < (uint32_t)FSL_FEATURE_SCT_NUMBER_OF_MATCH_CAPTURE) {

				currentCtrlVal |= SCT_EV_CTRL_MATCHSEL(s_currentMatch);

				/* Use Counter_L bits if user wants to setup the Low counter */
				sc->MATCH_ACCESS16BIT[s_currentMatch].MATCHL       = (uint16_t)matchValue;
				sc->MATCHREL_ACCESS16BIT[s_currentMatch].MATCHRELL = (uint16_t)matchValue;
				sc->EV[s_currentEvent].CTRL                        = currentCtrlVal;

				/* Increment the match register number */
				s_currentMatch++;
			}
		}
		else if ((kSCTIMER_Counter_H == whichCounter) && (0U == (sc->CONFIG & SCT_CONFIG_UNIFY_MASK))) {

			if (s_currentMatchhigh < (uint32_t)FSL_FEATURE_SCT_NUMBER_OF_MATCH_CAPTURE) {

				currentCtrlVal |= SCT_EV_CTRL_MATCHSEL(s_currentMatchhigh);

				/* Use Counter_H bits if user wants to setup the High counter */
				currentCtrlVal |= SCT_EV_CTRL_HEVENT(1U);
				sc->MATCH_ACCESS16BIT[s_currentMatchhigh].MATCHH       = (uint16_t)matchValue;
				sc->MATCHREL_ACCESS16BIT[s_currentMatchhigh].MATCHRELH = (uint16_t)matchValue;

				sc->EV[s_currentEvent].CTRL = currentCtrlVal;
				/* Increment the match register number */
				s_currentMatchhigh++;
			}
		}
		else if ((kSCTIMER_Counter_U == whichCounter) && (0U != (sc->CONFIG & SCT_CONFIG_UNIFY_MASK))) {

			if (s_currentMatch < (uint32_t)FSL_FEATURE_SCT_NUMBER_OF_MATCH_CAPTURE) {

				/* Use Counter_L bits if counter is operating in 32-bit mode */
				currentCtrlVal |= SCT_EV_CTRL_MATCHSEL(s_currentMatch);

				sc->MATCH[s_currentMatch]    = matchValue;
				sc->MATCHREL[s_currentMatch] = matchValue;
				sc->EV[s_currentEvent].CTRL  = currentCtrlVal;

				/* Increment the match register number */
				s_currentMatch++;
			}
		}
	}
	/* Enable the event in the current state */
	sc->EV[s_currentEvent].STATE = (1UL << s_currentState);

	/* Increment the event number */
	s_currentEvent++;
    return s_currentEvent - 1;
}