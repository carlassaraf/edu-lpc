/*
 * Event.h
 *
 *  Created on: Feb 10, 2022
 *      Author: fabri
 */

#ifndef EVENT_H_
#define EVENT_H_

/* Includes */
#include "CTimer.h"

/* Settings struct */
typedef struct {
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
		/* Interrupt on match */
		false
	};
	/* Default match channel */
	ctimer_match_t match_channel = kCTIMER_Match_0;
} timer_event_settings_t;

/* Class definition */
class Event {

	public:
		/* Output action constants */
		static constexpr ctimer_match_output_control_t None {kCTIMER_Output_NoAction};
		static constexpr ctimer_match_output_control_t Clear {kCTIMER_Output_Clear};
		static constexpr ctimer_match_output_control_t Set {kCTIMER_Output_Set};
		static constexpr ctimer_match_output_control_t Toggle {kCTIMER_Output_Toggle};
		/* Constructors */
		Event(CTimer *ctimer);
		/* Public methods */
		void outputPin(uint8_t match_output_pin);
		void outputAction(uint8_t action);
		void frequency(uint32_t frequency);
		void attachInterrupt(void (*f)(uint32_t));
		void start(void);
		void stop(void);
		void reset(void);

	private:
		CTimer *timer;
		/* Settings attribute */
		timer_event_settings_t settings;
		/* Private methods */
		void match(uint32_t match_value);
};


/* Inline methods */

/*!
 * @brief Event outputAction method.

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
inline void Event::outputAction(uint8_t action) {
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
 * @brief Event start method.

 * Starts the counter.
 *
 * @param None.
 *
 * @retval None.
 */
inline void Event::start(void) { CTIMER0->TCR |= CTIMER_TCR_CEN_MASK; }

/*!
 * @brief Event stop method.

 * Stops the counter.
 *
 * @param None.
 *
 * @retval None.
 */
inline void Event::stop(void) { CTIMER0->TCR &= ~CTIMER_TCR_CEN_MASK; }

/*!
 * @brief Event stop method.

 * Resets the counter.
 *
 * @param None.
 *
 * @retval None.
 */
inline void Event::reset(void) {
	/* Reset counter */
	CTIMER0->TCR |= CTIMER_TCR_CRST_MASK;
	/* Clear reset */
	CTIMER0->TCR &= ~CTIMER_TCR_CRST_MASK;
}

/*!
 * @brief Event match private method.

 * Sets a new value to the counter.
 *
 * @param match_value counter value.
 *
 * @retval None.
 */
inline void Event::match(uint32_t match_value) {
	/* Update match value in settings */
	settings.match_config.matchValue = match_value;
	/* Overwrite match value */
	CTIMER0->MR[settings.match_channel] = match_value;
	/* Reset counter */
	CTIMER0->TC = 0;
}

#endif /* EVENT_H_ */
