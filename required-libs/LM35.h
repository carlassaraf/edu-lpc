/*
 * LM35.h
 *
 *  Created on: Nov 18, 2021
 *      Author: fabri
 */

#ifndef LM35_H_
#define LM35_H_

/* LM35 external includes */
#include "ADC.h"

/* LM35 data struct */
typedef struct {
	/* 12-bit raw value */
	uint16_t raw;
	/* Temperature in Celsius */
	float tempC;
	/* Temperature in Fahrenheit */
	float tempF;
} lm35_data_t;

/* Class definition */
class LM35 {

	public:
		/* Constructors */
		LM35(uint8_t channel = 1);
		/* Public methods */
		void update(void);
		uint16_t getRaw(void);
		float getTempC(void);
		float getTempF(void);

	private:
		/* ADC object pointer */
		ADC *adc;
		/* LM35 data variable */
		lm35_data_t data;
};

/*!
 * @brief LM35 constructor.

 * Creates a LM35 object.
 *
 * @param channel index of the ADC channel
 * to be used. Defaults to 1.
 *
 * @retval None.
 */
inline LM35::LM35(uint8_t channel) {
	/* Create a pointer to the ADC instance */
	adc = new ADC(channel);
}

/*!
 * @brief LM35 update method.

 * Reads the output voltage from the LM35
 * and update the data struct.
 *
 * @param None.
 *
 * @retval None.
 */
inline void LM35::update(void) {
	/* Read and update raw value */
	data.raw = adc->read();
	/* Calculate temperature value in Celsius */
	data.tempC = 3.3 * (float) data.raw / 4095 * 100;
	/* Calculate temperature value in Fahrenheit */
	data.tempF = (data.tempC * 9 / 5) + 32;
}

/*!
 * @brief LM35 getRaw method.

 * Returns the stored raw value from ADC.
 *
 * @param None.
 *
 * @retval raw 12 bit ADC value.
 */
inline uint16_t LM35::getRaw(void) { return data.raw; }

/*!
 * @brief LM35 getTempC method.

 * Returns the stored temperature in Celsius.
 *
 * @param None.
 *
 * @retval temperature in Celsius.
 */
inline float LM35::getTempC(void) { return data.tempC; }

/*!
 * @brief LM35 getTempF method.

 * Returns the stored temperature in Fahrenheit.
 *
 * @param None.
 *
 * @retval temperature in Fahrenheit.
 */
inline float LM35::getTempF(void) {	return data.tempF; }

#endif /* LM35_H_ */