/*
 * LM35.h
 *
 *  Created on: Nov 18, 2021
 *      Author: fabri
 */

#ifndef LM35_H_
#define LM35_H_

/*  LM35 external includes  */

#include "ADC.h"

/*  LM35 data struct  */

typedef struct {

	uint32_t raw;
	float temperatureCelsius;
	float temperatureFahrenheit;

} LM35_Data_t;

/*  Class definition  */

class LM35 {

	ADC lm35_output;

	public:
		LM35(uint32_t channel) : lm35_output(channel) {};
		uint32_t getRaw(void);
		float getTemperatureCelsius(void);
		float getTemperatureFahrenheit(void);

	private:
		LM35_Data_t LM35_Data;
};

#endif /* LM35_H_ */