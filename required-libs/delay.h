/*
 * delay.h
 *
 *  Created on: Jan 22, 2022
 *      Author: fabri
 */

#ifndef DELAY_H_
#define DELAY_H_

#include <stdint.h>

/* Delay inline function */
void delay(uint32_t ms);

inline void delay(uint32_t ms) { for(uint32_t n = 0; n < ms * 600; n++); }

#endif /* DELAY_H */
