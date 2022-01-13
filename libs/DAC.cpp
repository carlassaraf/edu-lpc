/*
 * DAC.cpp
 *
 *  Created on: Jun 13, 2021
 *      Author: fabri
 */

#include <DAC.h>

wave_t sineWave, triangularWave;		// Global wave structs

/*  Global arrays to store up to 260 values of a sine or triangular wave  */
uint32_t sine_wave_values[WAVE_MAX_VALUES];
uint32_t triangular_wave_values[WAVE_MAX_VALUES];

/*!
 * @brief DAC constructor.

 * Creates a DAC object.
 *
 * @param DAC output channel (0-1).
 *
 * @retval None.
 */
DAC::DAC(uint32_t dac_channel) {

	SYSCON->PDRUNCFG &= ~(1 << (dac_channel + SYSCON_PDRUNCFG_DAC0_SHIFT));		// Power up DACn

	if(!dac_channel) {
		SYSCON->SYSAHBCLKCTRL0 |= SYSCON_SYSAHBCLKCTRL0_DAC0_MASK;				// Enable clock for DAC0

		SYSCON->SYSAHBCLKCTRL0 	|= SYSCON_SYSAHBCLKCTRL0_IOCON_MASK;			// Enable clock for IOCON to enable DAC function
		IOCON->PIO[0] |= IOCON_PIO_DACMODE_MASK;
		SYSCON->SYSAHBCLKCTRL0	&= ~(SYSCON_SYSAHBCLKCTRL0_SWM_MASK);			// Disable clock for IOCON
		base_dac = DAC0;
	}
	else {
		SYSCON->SYSAHBCLKCTRL1 |= SYSCON_SYSAHBCLKCTRL1_DAC1_MASK;				// Enable clock for DAC1
		base_dac = DAC1;
	}

	SYSCON->SYSAHBCLKCTRL0 	|= SYSCON_SYSAHBCLKCTRL0_SWM_MASK;					// Enable clock for switch matrix (SWM)
	SWM0->PINENABLE0 &= ~(1 << (dac_channel + SWM_PINENABLE0_DACOUT0_SHIFT));	// Enable DAC function on SWM
	SYSCON->SYSAHBCLKCTRL0	&= ~(SYSCON_SYSAHBCLKCTRL0_SWM_MASK);				// Disable SWM clock
}

/*!
 * @brief DAC set method.

 * Sets a 10 bit value to the DAC.
 *
 * @param value to be set in the CR register.
 *
 * @retval None.
 */
void DAC::set(uint32_t value) {

	base_dac->CR = (value & 0x3ff) << 6;
}

/*!
 * @brief DAC get method.

 * Gets the 10 bit value from the CR register.
 *
 * @param None..
 *
 * @retval 10 bit value in the CR register.
 */
uint16_t DAC::get(void) {

	return (base_dac->CR >> 6) & 0x3ff;
}

/*!
 * @brief DAC setVoltage method.

 * Sets a voltage output on the DAC.
 *
 * @param voltage to calculate the value in the CR register.
 *
 * @retval None.
 */
void DAC::setVoltage(float voltage) {

	uint32_t value = voltage * 0x3ff / 3.3;
	set(value);
}

/*!
 * @brief DAC sine method.

 * Initializes the necessary registers to create a sine wave.
 * Amplitude and fidelity of the sine wave depends on the frequency.
 *
 * @param frequency of the sine wave.
 *
 * @retval None.
 */
void DAC::sine(uint32_t frequency) {

	delete timer;
	timer = new CTimer(1);					// Reserves a timer for the sine wave
	timer->attachInterrupt(sine_wave);		// Creates an interrupt for every amtch of the timer

	getSineWaveValues(frequency);				// Calculate the necessary points of the sine wave

	timer->setFrequency(frequency * sineWave.max_values);		// Have a faster timer frequency

	sineWave.frequency = frequency;
	sineWave.dac = base_dac;
	sineWave.counter = 0;

	timer->reset();
	timer->start();
}

/*!
 * @brief DAC triangular method.

 * Initializes the necessary registers to create a triangular wave.
 * Amplitude and fidelity of the triangular wave depends on the frequency.
 *
 * @param frequency of the triangular wave.
 *
 * @retval None.
 */
void DAC::triangular(uint32_t frequency) {

	delete timer;
	timer = new CTimer(1);						// Reserves a timer for the triangular wave
	timer->attachInterrupt(triangular_wave);	// Creates an interrupt for every match of the timer

	getTriangularValues(frequency);				// Calculate necessary points of the triangular wave

	timer->setFrequency(frequency * triangularWave.max_values);		// Have a faster timer frequency

	triangularWave.frequency = frequency;
	triangularWave.dac = base_dac;
	triangularWave.counter = 0;

	timer->reset();
	timer->start();
}

/*!
 * @brief DAC getSineWaveValues private method.

 * Calculates the necesarry points for the sine wave
 * depending on the frequency.
 *
 * @param frequency of the sine wave.
 *
 * @retval None.
 */
void DAC::getSineWaveValues(uint32_t frequency) {

	sineWave.max_values = -0.09 * frequency + 100.09;				// Function approximation
	sineWave.amplitude_multiplier = -0.15 * frequency + 250.15;		// Function approximation

	float t = 0;
	float inc = 1 / ((float)sineWave.max_values * frequency);

	for (unsigned int i = 0 ; i < sineWave.max_values ; i++) {

		sine_wave_values[i] = 512 +  sineWave.amplitude_multiplier * sin(2 * M_PI * frequency * t);
		t += inc;
	}
}

/*!
 * @brief DAC getTriangularWaveValues private method.

 * Calculates the necesarry points for the triangular wave
 * depending on the frequency.
 *
 * @param frequency of the triangular wave.
 *
 * @retval None.
 */
void DAC::getTriangularValues(uint32_t frequency) {

	triangularWave.max_values = -0.09 * frequency + 100.09;
	triangularWave.amplitude_multiplier = -0.15 * frequency + 250.15;

	float dt = round((float)triangularWave.max_values / 4);
	float dv = triangularWave.amplitude_multiplier / dt;
	float t = dt;

	int i = 0;
	while (i < t) {
		triangular_wave_values[i] = 512 +  dv * i;
		i++;
	}

	int aux = i;
	t += dt;

	while (i < t) {
		triangular_wave_values[i] = triangular_wave_values[aux - (i + 1 - aux)];
		i++;
	}

	aux = i;
	t += dt;

	while (i < t) {
		triangular_wave_values[i] = -1 * triangular_wave_values[i + 1 - aux];
		i++;
	}

	aux = i;
	t += dt;

	while (i < t) {
		triangular_wave_values[i] = triangular_wave_values[aux - (i + 1 - aux)];
		i++;
	}
}

/*!
 * @brief DAC getTimer method.

 * Returns a pointer to the private CTimer
 * assigned to the DAC object.
 *
 * @param None.
 *
 * @retval pointer to the CTimer.
 */
CTimer* DAC::getTimer(void) {
	return timer;
}

/*!
 * @brief DAC sine wave handler.

 * Sets the sine wave value every timer match interrupt.
 *
 * @param flags CTIMER flags.
 *
 * @retval None.
 */
void sine_wave(uint32_t flags) {

	sineWave.dac->CR = (sine_wave_values[sineWave.counter++] & 0x3ff) << 6;
	if (sineWave.counter == sineWave.max_values) {
		sineWave.counter = 0;
	}
}

/*!
 * @brief DAC triangular wave handler.

 * Sets the triangular wave value every timer match interrupt.
 *
 * @param flags CTIMER flags.
 *
 * @retval None.
 */
void triangular_wave(uint32_t flags) {

	triangularWave.dac->CR = (triangular_wave_values[triangularWave.counter++] & 0x3ff) << 6;
	if (triangularWave.counter == triangularWave.max_values) {
		triangularWave.counter = 0;
	}
}
