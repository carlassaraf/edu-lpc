/*
 * DAC.h
 *
 *  Created on: Jun 13, 2021
 *      Author: fabri
 */

#ifndef DAC_H_
#define DAC_H_

#include <fsl_dac.h>
#include <fsl_power.h>

/*  DAC external includes  */

#include <math.h>
#include "Timer.h"

/*  DAC definitions  */

#define IOCON_DACMODE_SHIFT		16UL
#define WAVE_MAX_VALUES			360

/*  typedef struct for the sine and triangular waves  */

typedef struct {
	DAC_Type *dac;
	uint32_t counter;
	uint32_t max_values;
	uint32_t frequency;
	float amplitude_multiplier;
} wave_t;

/*  Class definition  */

class DAC {

	public:
		DAC(uint32_t dac_channel);
		void set(uint32_t value);
		uint16_t get(void);
		void setVoltage(float voltage);
		void sine(uint32_t frequency);
		void triangular(uint32_t frequency);
		CTimer* getTimer(void);

	private:
		DAC_Type *base_dac;
		CTimer *timer;

		void getSineWaveValues(uint32_t frequency);
		void getTriangularValues(uint32_t frequency);
};

/*  Extra function prototypes  */
void sine_wave(uint32_t flags);
void triangular_wave(uint32_t flags);

#endif /* DAC_H_ */
