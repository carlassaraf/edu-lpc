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

/* DAC constants */
static constexpr uint8_t IOCON_DACMODE_SHIFT = 16UL;
static constexpr uint16_t WAVE_MAX_VALUES = 360;

/* typedef struct for the sine and triangular waves */
typedef struct {
	DAC_Type *dac;
	uint32_t counter;
	uint32_t max_values;
	uint32_t frequency;
	float amplitude_multiplier;
	uint16_t values[360];
} wave_t;

/* DAC wave types */
enum class WAVE_TYPE : uint8_t {
	SINE,
	TRIANGULAR
};

/* Class definition */
class DAC {

	public:
		/* Constructors */
		DAC(uint8_t dac_channel);
		/* Public methods */
		void set(uint16_t value);
		uint16_t get(void);
		void setVoltage(float voltage);
		CTimer* getTimer(void);
		void sine(uint16_t frequency);
		void triangular(uint16_t frequency);

	private:
		/* Pointer to DAC peripherial */
		DAC_Type *base_dac;
		/* Pointer to CTimer object */
		CTimer *timer;
		/* DAC channel */
		uint8_t channel;
		/* Private methods */
		void wave(uint16_t frequency, WAVE_TYPE wave);
		void getSineValues(uint16_t frequency);
		void getTriangularValues(uint16_t frequency);
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
inline void DAC::set(uint16_t value) {
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
inline CTimer* DAC::getTimer(void) { return timer; }

/*!
 * @brief DAC sine method.

 * Calls the wave method to prepare the
 * registers to generate a sine wave.
 *
 * @param frequency of the sine wave.
 *
 * @retval None.
 */
inline void DAC::sine(uint16_t frequency) {
	wave(frequency, WAVE_TYPE::SINE);
}

/*!
 * @brief DAC triangular method.

 * Calls the wave method to prepare the
 * register to generate a triangular wave.
 *
 * @param frequency of the triangular wave.
 *
 * @retval None.
 */
inline void DAC::triangular(uint16_t frequency) {
	wave(frequency, WAVE_TYPE::TRIANGULAR);
}

/* Extra function prototypes */
void wave_channel_0(uint32_t flags);
void wave_channel_1(uint32_t flags);

#endif /* DAC_H_ */
