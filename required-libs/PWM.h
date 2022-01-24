/*
 * PWM.h
 *
 *  Created on: Dec 24, 2021
 *      Author: fabri
 */

#ifndef PWM_H_
#define PWM_H_

/* PWM includes */
#include "SCTimer.h"
#include <fsl_swm.h>

/* Configuration struct */
typedef struct {
	/* PWM configuration struct */
	sctimer_pwm_signal_param_t config = {
		kSCTIMER_Out_0,			/* Defaults to the current output */
		kSCTIMER_HighTrue,		/* High level PWM */
		50U						/* 50% duty cycle */
	};
	/* PWM frequency */
	uint32_t frequency {1000U};	/* Defauls to 1KHz */
	/* PWM output pin */
	uint8_t outputPin {0U};		/* Defaults to 0 */
} pwm_config_t;

/* Class definition */
class PWM {

	public:
		/* Static class members */
		/* Keep track of SCTimer event number */
		static uint8_t s_currentEvent;
		/* Keep track of SCTimer unify 32-bit or low 16-bit match/capture register number */
		static uint8_t s_currentMatch;
		/* Keep track of SCTimer unify 32-bit or low 16-bit match/capture register number */
		static sctimer_out_t s_currentOutput;
		/* Only supports state 0 */
		static constexpr uint8_t s_currentState {0};
		/* Constructors */
		PWM(uint8_t outputPin, uint32_t frequency = 1000, uint8_t duty = 50);
		void setDuty(uint8_t duty);
		void setFrequency(uint32_t frequency);
		void logicLevel(bool logic);
		void setOutputPin(uint8_t pin);
		uint8_t getDuty(void);
		uint32_t getFrequency(void);

	private:
		/* Settings struct to keep track of configuration */
		pwm_config_t settings;
		/* Instance assigned period event */
		uint32_t periodEvent;
		/* Instance assigned pulse event */
		uint32_t pulseEvent;
		/* Private methods */
		void init(uint32_t frequency);
		uint8_t createEvent(uint32_t matchValue);
};


/* Inline methods */

/*!
 * @brief PWM getDuty method.

 * Gets the current PWM duty cycle.
 *
 * @param None.
 *
 * @retval PWM duty cycle in percentage.
 */
inline uint8_t PWM::getDuty(void) {
	/* Duty cycle from sesttings */
	return settings.config.dutyCyclePercent;
}

/*!
 * @brief PWM getFrequency method.

 * Gets the current PWM frequency.
 *
 * @param None.
 *
 * @retval PWM frequency in percentage.
 */
inline uint32_t PWM::getFrequency(void) {
	/* Frequency from settings */
	return settings.frequency;
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
inline uint8_t PWM::createEvent(uint32_t matchValue) {
	/* Control variable */
    uint32_t currentCtrlVal = (uint32_t)kSCTIMER_MatchEventOnly;
	/* Use Counter_L bits if counter is operating in 32-bit mode */
	currentCtrlVal |= SCT_EV_CTRL_MATCHSEL(s_currentMatch);
	/* Update match value for current event */
	SCT0->MATCH[s_currentMatch]    = matchValue;
	SCT0->MATCHREL[s_currentMatch] = matchValue;
	SCT0->EV[s_currentEvent].CTRL  = currentCtrlVal;
	/* Increment the match register number */
	s_currentMatch++;
	/* Enable the event in the current state and increment event */
    SCT0->EV[s_currentEvent++].STATE = (1UL << s_currentState);
	/* Return the event that was created */
    return s_currentEvent - 1;
}

#endif /* PWM_H_ */
