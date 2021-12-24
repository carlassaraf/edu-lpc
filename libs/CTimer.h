/*
 * Timer.h
 *
 *  Created on: Nov 12, 2021
 *      Author: fabri
 */

#ifndef TIMER_H_
#define TIMER_H_

/*  Timer includes  */

#include <fsl_ctimer.h>

/*  Ctimer IRQ types  */

typedef enum {
	kMR0INT_IRQ,
	kMR1INT_IRQ,
	kMR2INT_IRQ,
	kMR3INT_IRQ
} ctimer_irq_type_t;

/*  Ctimer class definition  */

class CTimer {

	public:

		CTimer(uint32_t match_channel);
		void start(void);
		void stop(void);
		void setOutputPin(uint32_t match_output_pin);
		void setMatch(uint32_t match_value);
		void setOutputNoAction(void);
		void setOutputToggle(void);
		void setOutputClear(void);
		void setOutputSet(void);
		void setFrequency(uint32_t freq);
		void attachInterrupt(void (*f)(uint32_t));

	private:

		uint32_t matchChannel;
};

#endif /* TIMER_H_ */
