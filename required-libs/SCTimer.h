/*
 * SCTimer.h
 *
 *  Created on: Jan 13, 2022
 *      Author: fabri
 */

#ifndef SCTIMER_H_
#define SCTIMER_H_

/* SCTimer includes */
#include "fsl_sctimer.h"

/* Class definition */
class SCTimer {

	public:
		SCTimer(void);
};

/* Inline methods */

/*!
 * @brief SCTimer constructor.

 * Creates a SCTimer object. Default timmer is SCT0.
 *
 * @param index SCTimer index.
 *
 * @retval None.
 */
inline SCTimer::SCTimer(void) {
	/* Default configuration */
	sctimer_config_t config {
		true,							/* Enable counter unify */
		kSCTIMER_System_ClockMode,		/* System clock */
		kSCTIMER_Clock_On_Rise_Input_0,	/* Clock selected */
		false, 							/* Up count Counter L */
		false,							/* Up count Counter H */
		0U,								/* Prescale Counter L */
		0U,								/* Prescale Counter H */
		0U,								/* Initial output value */
		0xFU							/* Input synchronization */
	};
	/* Reset any prevous SCTimer configuration */
	SCTIMER_Deinit(SCT0);
	/* Initialize SCTimer with defuault config */
	SCTIMER_Init(SCT0, &config);
}

#endif /* SCTIMER_H_ */
