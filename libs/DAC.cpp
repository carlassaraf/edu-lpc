/*
 * DAC.cpp
 *
 *  Created on: Jun 13, 2021
 *      Author: fabri
 */

#include <DAC.h>

sine_wave_t sineWave;
triangular_wave_t triangularWave;

DAC::DAC(uint32_t dac_channel) {

	SYSCON->PDRUNCFG &= ~(1 << (dac_channel + SYSCON_PDRUNCFG_DAC0_SHIFT));		// Power up DACn

	if(!dac_channel) {
		SYSCON->SYSAHBCLKCTRL0 |= SYSCON_SYSAHBCLKCTRL0_DAC0_MASK;		// Enable clock for DAC0

		SYSCON->SYSAHBCLKCTRL0 	|= SYSCON_SYSAHBCLKCTRL0_IOCON_MASK;		// Enable clock for IOCON to enable DAC function
		IOCON->PIO[0] |= IOCON_PIO_DACMODE_MASK;
		SYSCON->SYSAHBCLKCTRL0	&= ~(SYSCON_SYSAHBCLKCTRL0_SWM_MASK);		// Disable clock for IOCON
		base_dac = DAC0;
	}
	else {
		SYSCON->SYSAHBCLKCTRL1 |= SYSCON_SYSAHBCLKCTRL1_DAC1_MASK;		// Enable clock for DAC1
		base_dac = DAC1;
	}

	SYSCON->SYSAHBCLKCTRL0 	|= SYSCON_SYSAHBCLKCTRL0_SWM_MASK;					// Enable clock for switch matrix (SWM)
	SWM0->PINENABLE0 &= ~(1 << (dac_channel + SWM_PINENABLE0_DACOUT0_SHIFT));	// Enable DAC function on SWM
	SYSCON->SYSAHBCLKCTRL0	&= ~(SYSCON_SYSAHBCLKCTRL0_SWM_MASK);				// Disable SWM clock
}

DAC::~DAC() {
	// TODO Auto-generated destructor stub
}

void DAC::set(uint32_t value) {

	base_dac->CR = (value & 0x3ff) << 6;
}

void DAC::sine(uint32_t freq) {

	CTimer_Match dacTimer(1);
	dacTimer.attachInterrupt(sine_wave);
	dacTimer.setFrequency(freq * 100);		// Calculate 10 points of the sine wave
	dacTimer.setOutputPin(8);

	sineWave.freq = freq;
	sineWave.dac = base_dac;
	sineWave.time_base = 1 / (100 * (float)freq);//2 * (CTIMER0->PR + 1) / (float)timer_freq;
	sineWave.counter = 0;

	dacTimer.start();
}

void DAC::triangular(uint32_t freq) {

	CTimer_Match dacTimer(1);
	dacTimer.attachInterrupt(triangular_wave);
	dacTimer.setFrequency(freq * 40);		// Calculate 10 points of the sine wave
	dacTimer.setOutputPin(8);

	triangularWave.freq = freq;
	triangularWave.dac = base_dac;
	triangularWave.dv = 10;

	triangularWave.dac->CR = 512;
	triangularWave.rising = true;

	dacTimer.start();
}

void sine_wave(uint32_t flags) {

	float t = sineWave.time_base * sineWave.counter;
	float val = 511 * sin(2 * M_PI * sineWave.freq * t);
	uint32_t value = 512 + val;//511 * sin(2 * M_PI * sineWave.freq * t);
	sineWave.dac->CR = (value & 0x3ff) << 6;
	sineWave.counter++;
	if(sineWave.counter > 99) {	// If we already calculated 10 points
		sineWave.counter = 0;	// reset
	}
}

void triangular_wave(uint32_t flags) {

	uint32_t value = ((triangularWave.dac->CR) >> 6) & 0x3ff;
	if(triangularWave.rising) {
		if(value < 512) {
			triangularWave.dac->CR = ((value + triangularWave.dv) & 0x3ff) << 6;
		}
		else {
			triangularWave.dac->CR = ((value - triangularWave.dv) & 0x3ff) << 6;
			triangularWave.rising = false;
		}
	}
	else {
		if(value > 0) {
			triangularWave.dac->CR = ((value - triangularWave.dv) & 0x3ff) << 6;
		}
		else {
			triangularWave.dac->CR = ((value + triangularWave.dv) & 0x3ff) << 6;
			triangularWave.rising = true;
		}
	}
}
