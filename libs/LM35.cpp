/*
 * LM35.cpp
 *
 *  Created on: Nov 18, 2021
 *      Author: fabri
 */

#include <LM35.h>

LM35::~LM35() {
	// TODO Auto-generated destructor stub
}

uint32_t LM35::getRaw(void) {

	LM35_Data.rawTotal = v_total.read();
	LM35_Data.rawDiodes = v_diodes.read();

	LM35_Data.rawTemperature = LM35_Data.rawTotal - LM35_Data.rawDiodes;
	return LM35_Data.rawTemperature;
}

float LM35::getTemperatureCelsius(void) {

	getRaw();
	LM35_Data.temperatureCelsius = 3.3 * (float) LM35_Data.rawTemperature / 4095 * 100;

	return LM35_Data.temperatureCelsius;
}
