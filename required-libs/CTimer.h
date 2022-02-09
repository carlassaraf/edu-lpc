/*
 * CTimer.h
 *
 *  Created on: Nov 12, 2021
 *      Author: fabri
 */

#ifndef CTIMER_H_
#define CTIMER_H_

/* Includes */
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
		/* Output action constants */
		static constexpr uint8_t None {kCTIMER_Output_NoAction};
		static constexpr uint8_t Clear {kCTIMER_Output_Clear};
		static constexpr uint8_t Set {kCTIMER_Output_Set};
		static constexpr uint8_t Toggle {kCTIMER_Output_Toggle};
		/* Constructors */
		CTimer(uint32_t match_channel);
		/* Public methods */
		void outputPin(uint8_t match_output_pin);
		void outputAction(uint8_t action);
		void frequency(uint32_t freq);
		void attachInterrupt(void (*f)(uint32_t));		
		void start(void);
		void stop(void);
		void reset(void);

	private:
		/* CTimer and Match settings */
		ctimer_settings_t settings;
		/* Private methods */
		void match(uint32_t match_value);
};

/* Inline methods */

/*!
 * @brief CTimer setOutputAction method.

 * Configures the output pin behavior.
 *
 * @param action to take. Possible values are:
 * - None
 * - Clear
 * - Set
 * - Toggle
 *
 * @retval None.
 */
inline void CTimer::setOutputAction(uint8_t action) {
	/* Update settings */
	settings.match_config.outControl = kCTIMER_Output_NoAction;
	/* Clear previous state */
	CTIMER0->EMR &= ~(CTIMER_EMR_EMC0_MASK << settings.match_channel);
	/* Check if action is needed */
	if (action) {
		/* Set functionality */
		CTIMER0->EMR |= action << (settings.match_channel * 2U + CTIMER_EMR_EMC0_SHIFT);
	}
}

/*!
 * @brief CTimer start method.

 * Starts the counter.
 *
 * @param None.
 *
 * @retval None.
 */
inline void CTimer::start(void) { CTIMER0->TCR |= CTIMER_TCR_CEN_MASK; }

/*!
 * @brief CTimer stop method.

 * Stops the counter.
 *
 * @param None.
 *
 * @retval None.
 */
inline void CTimer::stop(void) { CTIMER0->TCR &= ~CTIMER_TCR_CEN_MASK; }

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
 * @brief CTimer match private method.

 * Sets a new value to the counter.
 *
 * @param match_value counter value.
 *
 * @retval None.
 */
inline void CTimer::match(uint32_t match_value) {
	/* Update match value in settings */
	settings.match_config.matchValue = match_value;
	/* Overwrite match value */
	CTIMER0->MR[settings.match_channel] = match_value;
	/* Reset counter */
	CTIMER0->TC = 0;
}

#endif /* TIMER_H_ */
