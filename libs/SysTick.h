/*
 * SycTick.h
 *
 *  Created on: Dec 6, 2021
 *      Author: fabri
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

#include "LPC845.h"

class Tick {

	public:

		Tick(uint32_t us, void (*f)(void));
		uint32_t getTickCounter(void);

	private:

		uint32_t tickCounter;
		bool tickDone;
};

#endif /* SYSTICK_H_ */
