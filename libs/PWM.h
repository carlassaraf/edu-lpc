/*
 * PWM.h
 *
 *  Created on: Dec 24, 2021
 *      Author: fabri
 */

#ifndef PWM_H_
#define PWM_H_

/*  PWM includes  */

#include "SCTimer.h"
#include <fsl_swm.h>

/*  Class definition  */

class PWM {

	public:

		PWM(uint32_t outputPin, uint32_t frequency = 1000, uint32_t duty = 50);
		void setDuty(uint32_t duty);
		uint32_t getDuty(void);
		void setFrequency(uint32_t frequency);
		uint32_t getFrequency(void);
		void logicLevel(bool logic);
		void setOutputPin(uint32_t pin);

	private:

		sctimer_pwm_signal_param_t pwmParams;

		uint32_t pwmFrequency;
		uint32_t periodEvent;
		uint32_t pulseEvent;

		SCTimer *timer;

		void init(uint32_t frequency);
		uint32_t createEvent(uint32_t matchValue, sctimer_counter_t whichCounter);
};

#endif /* PWM_H_ */
