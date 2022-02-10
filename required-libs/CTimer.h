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

/* Class definition */
class CTimer {

	public:
		/* Constructors */
		CTimer(void);
		/* Public methods */
		ctimer_match_t currentMatch(void);
		void newMatch(void);
		ctimer_callback_t* callbacks(void);

	private:
		/* CTimer and Match settings */
		ctimer_config_t settings = {
			/* Run as a timer */
			kCTIMER_TimerMode,
			/* This field is ignored when mode is timer */
			kCTIMER_Capture_0,
			/* Timer counter is incremented on every APB bus clock */
			0
		};
		/* Current match channel */
		ctimer_match_t current_match_channel;
		/* Array for the CTimer callbacks */
		static ctimer_callback_t ctimer_callbacks[8];
		/* Match0, Match1, Match2, Match3, Capture0, Capture1, Capture2, Capture3 */
};

/* Inline methods */

/*!
 * @brief CTimer currentMatch method.

 * Returns the current match channel for
 * the CTimer.
 *
 * @param None.
 *
 * @retval current match channel.
 */
inline ctimer_match_t CTimer::currentMatch(void) { return current_match_channel; }

/*!
 * @brief CTimer newMatch method.

 * Increments the current match of the CTimer.
 *
 * @param None.
 *
 * @retval None.
 */
inline void CTimer::newMatch(void) {
	/* Cast match as an int */
	uint8_t match = (uint8_t)current_match_channel;
	/* Increment match */
	match++;
	/* Update current match attribute */
	current_match_channel = (ctimer_match_t)match;
}

/*!
 * @brief CTimer callbacks method.

 * Returns the CTimer callbacks array.
 *
 * @param None.
 *
 * @retval callbacks array.
 */
inline ctimer_callback_t* CTimer::callbacks(void) { return ctimer_callbacks; }

#endif /* CTIMER_H_ */
