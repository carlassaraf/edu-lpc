/*
 * DAC.h
 *
 *  Created on: Jun 13, 2021
 *      Author: fabri
 */

#ifndef DAC_H_
#define DAC_H_

/* Includes */
#include <fsl_dac.h>
#include <fsl_power.h>
#include <math.h>
#include "CTimer.h"

/* DAC definitions */
#define IOCON_DACMODE_SHIFT		16UL
#define WAVE_MAX_VALUES			360

/* typedef struct for the sine and triangular waves */
typedef struct {
	DAC_Type *dac;
	uint32_t counter;
	uint32_t max_values;
	uint32_t frequency;
	float amplitude_multiplier;
} wave_t;

/* Class definition */
class DAC {

	public:
		/* Constructors */
		DAC(uint32_t dac_channel);
		/* Public methods */
		void triangular(uint32_t frequency);
		CTimer* getTimer(void);
		void set(uint32_t value);
		uint16_t get(void);
		void setVoltage(float voltage);
		void sine(uint32_t frequency);

	private:
		DAC_Type *base_dac;
		CTimer *timer;

		void getSineWaveValues(uint32_t frequency);
		void getTriangularValues(uint32_t frequency);
};

/* Inline methods */

/*!
 * @brief DAC set method.

 * Sets a 10 bit value to the DAC.
 *
 * @param value to be set in the CR register.
 *
 * @retval None.
 */
inline void DAC::set(uint32_t value) {
	/* Pass value to DAC register */
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
inline uint16_t DAC::get(void) {
	/* Get vallue from DAC register */
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
inline void DAC::setVoltage(float voltage) {
	/* Calculate 10-bit value */
	uint16_t value = voltage * 0x3ff / 3.3;
	/* Pass value to DAC register */
	base_dac->CR = (value & 0x3ff) << 6;
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
inline CTimer* DAC::getTimer(void) { return timer; 

/* Extra function prototypes */
void sine_wave(uint32_t flags);
void triangular_wave(uint32_t flags);

#endif /* DAC_H_ */
