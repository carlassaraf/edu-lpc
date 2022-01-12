/*
 * init.h
 *
 *  Created on: Jan 11, 2022
 *      Author: fabri
 */

#ifndef INIT_H_
#define INIT_H_

#include "GPIO.h"

extern USART serial;
extern LM35 lm;
extern DAC analogOut;

Pin gled(32, OUTPUT, 1);
Pin bled(33, OUTPUT, 1);
Pin rled(34, OUTPUT, 1);

Pin user(4, INPUT);

#endif /* INIT_H_ */