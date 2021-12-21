/*
 * LM35.cpp
 *
 *  Created on: Nov 18, 2021
 *      Author: fabri
 */

#include "LM35.h"

/*!
 * @brief LM35 getRaw method.

 * Reads the output voltage from the LM35.
 *
 * @param None.
 *
 * @retval raw 12 bit ADC value.
 */
uint32_t LM35::getRaw(void) {

	LM35_Data.raw = lm35_output.read();

	return LM35_Data.raw;
}

/*!
 * @brief LM35 getTemperatureCelsius method.

 * Reads the output voltage from the LM35 and converts
 * the value to temperature in Celsius.
 *
 * @param None.
 *
 * @retval temperature in Celsius.
 */
float LM35::getTemperatureCelsius(void) {

	getRaw();
	LM35_Data.temperatureCelsius = 3.3 * (float) LM35_Data.raw / 4095 * 100;

	return LM35_Data.temperatureCelsius;
}

/*!
 * @brief LM35 getTemperatureFahrenheit method.

 * Reads the output voltage from the LM35 and converts
 * the value to temperature in Fahrenheit.
 *
 * @param None.
 *
 * @retval temperature in Fahrenheit.
 */
float LM35::getTemperatureFahrenheit(void) {

	getTemperatureCelsius();
	LM35_Data.temperatureFahrenheit = (LM35_Data.temperatureCelsius * 9 / 5) + 32;

	return LM35_Data.temperatureFahrenheit; 
}