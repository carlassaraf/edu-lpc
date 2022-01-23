/*
 * SycTick.h
 *
 *  Created on: Dec 6, 2021
 *      Author: fabri
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

/* SysTick includes */
#include "LPC845.h"

/* Class definition */
class Tick {

	public:
		/* Constructors */
		Tick(uint32_t us, void (*f)(void));
};

#endif /* SYSTICK_H_ */
