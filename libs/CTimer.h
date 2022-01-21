/*
 * CTimer.h
 *
 *  Created on: Nov 12, 2021
 *      Author: fabri
 */

#ifndef CTIMER_H_
#define CTIMER_H_

/* CTimer includes */
#include <fsl_ctimer.h>

/* CTimer IRQ types */
typedef enum {
	kMR0INT_IRQ,
	kMR1INT_IRQ,
	kMR2INT_IRQ,
	kMR3INT_IRQ
} ctimer_irq_type_t;

/* Settings struct */
typedef struct {
	/* Default CTimer configuration */
	ctimer_config_t ctimer_config = {
		/* Run as a timer */
		kCTIMER_TimerMode,
		/* This field is ignored when mode is timer */
		kCTIMER_Capture_0,
		/* Timer counter is incremented on every APB bus clock */
		0
	};
	/* Default Match configuration */
	ctimer_match_config_t match_config = {
		/* Match value to avoid matching */
		0xffffffff,
		/* Matching will reset the timer */
		true,
		/* Match will not stop the counter */
		false,
		/* No output action taken on match */
		kCTIMER_Output_NoAction,
		/* Initial value of output pin */
		false,
		/* No interrupt on match */
		false
	};
	/* Default match channel */
	ctimer_match_t match_channel = kCTIMER_Match_0;
} ctimer_settings_t;

/* CTimer class definition */
class CTimer {

	public:

		CTimer(uint32_t match_channel);
		/* Public methods */
		void start(void);
		void stop(void);
		void reset(void);
		void setOutputPin(uint32_t match_output_pin);
		void setMatch(uint32_t match_value);
		void setOutputNoAction(void);
		void setOutputToggle(void);
		void setOutputClear(void);
		void setOutputSet(void);
		void setFrequency(uint32_t freq);
		void attachInterrupt(void (*f)(uint32_t));

	private:
		/* CTimer and Match settings */
		ctimer_settings_t settings;
};

#endif /* TIMER_H_ */
