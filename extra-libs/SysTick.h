/*
 * SycTick.h
 *
 *  Created on: Dec 6, 2021
 *      Author: fabri
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

/* Includes */
#include "LPC845.h"

/* Class definition */
class Tick {

	public:
		/* Constructors */
		Tick(uint32_t us, void (*f)(void));
		/* Public methods */
		void set(uint32_t us);
		void stop(void);
		void start(void);
		void reset(void);
};

/* Inline methods */

/*!
 * @brief Tick set method.

 * Sets a new time for the interrupt.
 *
 * @param us time in microseconds to set the interrupt.
 *
 * @retval None.
 */
inline void Tick::set(uint32_t us) {
	/* Stop the counter */
	stop();
	/* Calculate the number of ticks to match */
	uint32_t ticks = SystemCoreClock * (us / 1E6);
	/* Configure SysTick */
	SysTick->LOAD = ticks - 1;
	/* Reset counter */
	reset();
	/* Start counter */
	start();
}

/*!
 * @brief Tick stop method.

 * Stops the SysTick counter.
 *
 * @param None.
 *
 * @retval None.
 */
inline void Tick::stop(void) { SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; }

/*!
 * @brief Tick start method.

 * Starts the SysTick counter.
 *
 * @param None.
 *
 * @retval None.
 */
inline void Tick::start(void) {	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; }

/*!
 * @brief Tick stop method.

 * Resets the SysTick counter.
 *
 * @param None.
 *
 * @retval None.
 */
inline void Tick::reset(void) { SysTick->VAL = 0; }

#endif /* SYSTICK_H_ */
