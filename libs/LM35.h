/*
 * LM35.h
 *
 *  Created on: Nov 18, 2021
 *      Author: fabri
 */

#ifndef LM35_H_
#define LM35_H_

#include "ADC.h"

typedef struct {

	uint32_t rawTotal;
	uint32_t rawDiodes;
	uint32_t rawTemperature;
	float temperatureCelsius;
	float temperatureFahrenheit;

} LM35_Data_t;

class LM35 {

	ADC v_total;
	ADC v_diodes;

	public:
		LM35(void) : v_total(8), v_diodes(7) {};
		virtual ~LM35();
		uint32_t getRaw(void);
		float getTemperatureCelsius(void);

	private:
		LM35_Data_t LM35_Data;
};

#endif /* LM35_H_ */
