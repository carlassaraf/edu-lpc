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

#endif /* PWM_H_ */
