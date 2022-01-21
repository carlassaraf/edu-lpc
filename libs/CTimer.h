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

/* Inline methods */

/*!
 * @brief CTimer start method.

 * Starts the counter.
 *
 * @param None.
 *
 * @retval None.
 */
inline void CTimer::start(void) {
	/* Start counter */
	CTIMER0->TCR |= CTIMER_TCR_CEN_MASK;
}

/*!
 * @brief CTimer stop method.

 * Stops the counter.
 *
 * @param None.
 *
 * @retval None.
 */
inline void CTimer::stop(void) {
	/* Stop counter */
	CTIMER0->TCR &= ~CTIMER_TCR_CEN_MASK;
}

/*!
 * @brief CTimer stop method.

 * Resets the counter.
 *
 * @param None.
 *
 * @retval None.
 */
inline void CTimer::reset(void) {
	/* Reset counter */
	CTIMER0->TCR |= CTIMER_TCR_CRST_MASK;
	/* Clear reset */
	CTIMER0->TCR &= ~CTIMER_TCR_CRST_MASK;
}

/*!
 * @brief CTimer setMatch method.

 * Sets a new value to the counter.
 *
 * @param match_value counter value.
 *
 * @retval None.
 */
inline void CTimer::setMatch(uint32_t match_value) {
	/* Update match value in settings */
	settings.match_config.matchValue = match_value;
	/* Overwrite match value */
	CTIMER0->MR[settings.match_channel] = match_value;
	/* Reset counter */
	CTIMER0->TC = 0;
}

/*!
 * @brief CTimer setOutputNoAction method.

 * Configures the output pin behavior to none.
 *
 * @param None.
 *
 * @retval None.
 */
inline void CTimer::setOutputNoAction(void) {
	/* Update settings */
	settings.match_config.outControl = kCTIMER_Output_NoAction;
	/* Clear previous state */
	CTIMER0->EMR &= ~(CTIMER_EMR_EMC0_MASK << settings.match_channel);
}

/*!
 * @brief CTimer setOutputToggle method.

 * Configures the output pin behavior to toggle.
 *
 * @param None.
 *
 * @retval None.
 */
inline void CTimer::setOutputToggle(void) {
	/* Update settings */
	settings.match_config.outControl = kCTIMER_Output_Toggle;
	/* Clear previous state */
	CTIMER0->EMR &= ~(CTIMER_EMR_EMC0_MASK << settings.match_channel);
	/* Set functionality */
	CTIMER0->EMR |= kCTIMER_Output_Toggle << (settings.match_channel * 2U + CTIMER_EMR_EMC0_SHIFT);
}

/*!
 * @brief CTimer setOutputClear method.

 * Configures the output pin behavior to clear.
 *
 * @param None.
 *
 * @retval None.
 */
inline void CTimer::setOutputClear(void) {
	/* Update settings */
	settings.match_config.outControl = kCTIMER_Output_Clear;
	/* Clear previous state */
	CTIMER0->EMR &= ~(CTIMER_EMR_EMC0_MASK << settings.match_channel);
	/* Set functionality */
	CTIMER0->EMR |= kCTIMER_Output_Clear << (settings.match_channel * 2U + CTIMER_EMR_EMC0_SHIFT);
}

/*!
 * @brief CTimer setOutputSet method.

 * Configures the output pin behavior to set.
 *
 * @param None.
 *
 * @retval None.
 */
inline void CTimer::setOutputSet(void) {
	/* Update settings */
	settings.match_config.outControl = kCTIMER_Output_Set;
	/* Clear previous state */
	CTIMER0->EMR &= ~(CTIMER_EMR_EMC0_MASK << settings.match_channel);
	/* Set functionality */
	CTIMER0->EMR |= kCTIMER_Output_Set << (settings.match_channel * 2U + CTIMER_EMR_EMC0_SHIFT);
}

#endif /* TIMER_H_ */
