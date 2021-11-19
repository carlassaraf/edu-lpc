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
#include <math.h>

#include "Timer.h"

#define IOCON_DACMODE_SHIFT				16UL

typedef struct {
	uint32_t freq;
	DAC_Type *dac;
	float time_base;
	uint32_t counter;
} sine_wave_t;

typedef struct {
	uint32_t freq;
	DAC_Type *dac;
	bool rising;
	uint32_t dv;
} triangular_wave_t;

class DAC {
	public:
		DAC(uint32_t dac_channel);
		virtual ~DAC();
		void set(uint32_t value);
		void sine(uint32_t freq);
		void triangular(uint32_t freq);

	private:
		DAC_Type *base_dac;
};

void sine_wave(uint32_t flags);
void triangular_wave(uint32_t flags);

#endif /* DAC_H_ */
