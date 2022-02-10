/*
 * DAC.cpp
 *
 *  Created on: Jun 13, 2021
 *      Author: fabri
 */

#include <DAC.h>

/* Event array for every DAC channel */
Event *g_events[2] = { new Event(1, Event::None), new Event(1, Event::None) };
/* Global wave structs */
wave_t g_wave[2] {0};
/* Global pointers for Event interrupt calls */
void (*g_wave_ptr[2])(uint32_t) = { wave_channel_0, wave_channel_1 };

/*!
 * @brief DAC constructor.

 * Creates a DAC object.
 *
 * @param DAC output channel (0-1).
 *
 * @retval None.
 */
DAC::DAC(uint8_t dac_channel) {
	/* Power up DAC */
	SYSCON->PDRUNCFG &= ~(1 << (dac_channel + SYSCON_PDRUNCFG_DAC0_SHIFT));

	if(!dac_channel) {
		/* Enable clock for DAC0 */
		SYSCON->SYSAHBCLKCTRL0 |= SYSCON_SYSAHBCLKCTRL0_DAC0_MASK;
		/* Enable clock for IOCON */
		SYSCON->SYSAHBCLKCTRL0 	|= SYSCON_SYSAHBCLKCTRL0_IOCON_MASK;
		/* Enable DAC function in PIO0_17 */
		IOCON->PIO[0] |= IOCON_PIO_DACMODE_MASK;
		/* Disable clock for IOCON */
		SYSCON->SYSAHBCLKCTRL0	&= ~(SYSCON_SYSAHBCLKCTRL0_SWM_MASK);
		/* Save DAC0 pointer */
		base_dac = DAC0;
	}
	else {
		/* Enable clock for DAC1 */
		SYSCON->SYSAHBCLKCTRL1 |= SYSCON_SYSAHBCLKCTRL1_DAC1_MASK;
		/* Save DAC1 pointer */
		base_dac = DAC1;
	}
	/* Enable clock for switch matrix */
	SYSCON->SYSAHBCLKCTRL0 	|= SYSCON_SYSAHBCLKCTRL0_SWM_MASK;
	/* Enable DAC function on SWM */
	SWM0->PINENABLE0 &= ~(1 << (dac_channel + SWM_PINENABLE0_DACOUT0_SHIFT));
	/* Disable SWM clock */
	SYSCON->SYSAHBCLKCTRL0	&= ~(SYSCON_SYSAHBCLKCTRL0_SWM_MASK);
	/* Save channel */
	channel = dac_channel;
	/* Reserves a timer for the wave generation */
	event = g_events[channel];
	/* Creates an interrupt for every match of the Event */
	event->attachInterrupt(g_wave_ptr[channel]);
}

/*!
 * @brief DAC wave private method.

 * Prepares the DAC to generate a wave signal.
 *
 * @param frequency of the sine wave.
 * @param wave type of wave to be generated, could be:
 * - WAVE_TYPE::SINE
 * - WAVE_TYPE::TRIANGULAR
 *
 * @retval None.
 */
void DAC::wave(uint16_t frequency, WAVE_TYPE wave) {
	/* Checks what kind of wave the user called */
	if(wave == WAVE_TYPE::SINE) {
		/* Calculate the necessary points of the sine wave */
		getSineValues(frequency);
	}
	else if(wave == WAVE_TYPE::TRIANGULAR) {
		/* Calculate the necessary points of the triangular wave */
		getTriangularValues(frequency);
	}
	/* Have a faster timer frequency according to the number of values */
	event->frequency(frequency * g_wave[channel].max_values);
	/* Configure the wave settings */
	g_wave[channel].frequency = frequency;
	g_wave[channel].dac = base_dac;
	g_wave[channel].counter = 0;
	/* Reset the counter */
	event->reset();
	/* Start the counter */
	event->start();
}

/*!
 * @brief DAC getSineValues private method.

 * Calculates the necesarry points for the sine wave
 * depending on the frequency.
 *
 * @param frequency of the sine wave.
 *
 * @retval None.
 */
void DAC::getSineValues(uint16_t frequency) {
	/* Function approximation for the max values */
	g_wave[channel].max_values = -0.09 * frequency + 100.09;
	/* Function approximation for the amplitude multiplier */
	g_wave[channel].amplitude_multiplier = -0.15 * frequency + 250.15;

	float t = 0;
	/* Calculate increment so that 1 cycle matches the number of values */
	float inc = 1 / ((float)g_wave[channel].max_values * frequency);
	/* Calculate all the necessary values */
	for (unsigned int i = 0 ; i < g_wave[channel].max_values ; i++) {
		/* Calculate the value for that time value */
		g_wave[channel].values[i] = g_wave[channel].amplitude_multiplier * sin(2 * M_PI * frequency * t);
		/* Add offset */
		g_wave[channel].values[i] += 512;
		/* Go to the next t value */
		t += inc;
	}
}

/*!
 * @brief DAC getTriangularValues private method.

 * Calculates the necessary points for the triangular wave
 * depending on the frequency.
 *
 * @param frequency of the triangular wave.
 *
 * @retval None.
 */
void DAC::getTriangularValues(uint16_t frequency) {
	/* Function approximation for the max values */
	g_wave[channel].max_values = -0.09 * frequency + 100.09;
	/* Function approximation for the amplitude multiplier */
	g_wave[channel].amplitude_multiplier = -0.15 * frequency + 250.15;
	/* Calculate increment of time for only a cuarter */
	float dt = round((float)triangularWave.max_values / 4);
	/* Calculate amplitude increment for each value */
	float dv = triangularWave.amplitude_multiplier / dt;
	float t = dt;

	int i = 0;
	/* First positive slope */
	while (i < t) {
		/* Calculate value for that point plus offset */
		g_wave[channel].values[i] = 512 +  dv * i;
		i++;
	}
	/* Update variables */
	int aux = i;
	t += dt;
	/* First negative slope */
	while (i < t) {
		/* Calculate value for that point plus offset */
		g_wave[channel].values[i] = g_wave[channel].values[aux - (i + 1 - aux)];
		i++;
	}
	/* Update variables */
	aux = i;
	t += dt;
	/* Second negative slope */
	while (i < t) {
		/* Calculate value for that point plus offset */
		g_wave[channel].values[i] = -1 * g_wave[channel].values[i + 1 - aux];
		i++;
	}
	/* Update variables */
	aux = i;
	t += dt;
	/* Second positive slope */
	while (i < t) {
		/* Calculate value for that point plus offset */
		g_wave[channel].values[i] = g_wave[channel].values[aux - (i + 1 - aux)];
		i++;
	}
}

/*!
 * @brief DAC wave_channel_0 handler.

 * Sets the wave value every timer match interrupt.
 *
 * @param flags CTimer flags.
 *
 * @retval None.
 */
void wave_channel_0(uint32_t flags) {
	/* Set the DAC value */
	g_wave[0].dac->CR = (g_wave[0].values[g_wave[0].counter++] & 0x3ff) << 6;
	/* Check if the cycle is finished */
	if (g_wave[0].counter == g_wave[0].max_values) {
		/* Reset the wave */
		g_wave[0].counter = 0;
	}
}

/*!
 * @brief DAC triangular wave handler.

 * Sets the triangular wave value every timer match interrupt.
 *
 * @param flags CTimer flags.
 *
 * @retval None.
 */
void wave_channel_1(uint32_t flags) {
	/* Set the DAC value */
	g_wave[1].dac->CR = (g_wave[1].values[g_wave[1].counter++] & 0x3ff) << 6;
	/* Check if the cycle is finished */
	if (g_wave[1].counter == g_wave[1].max_values) {
		/* Reset the wave */
		g_wave[1].counter = 0;
	}
}