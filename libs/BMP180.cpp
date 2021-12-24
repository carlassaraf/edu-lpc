/*
 * BMP180.cpp
 *
 *  Created on: Dec 3, 2021
 *      Author: fabri
 */

#include <BMP180.h>

/*!
 * @brief BMP180 read method.

 * Reads temperature and pressure and calculates
 * the altitude. Updates the BMP180 data structure.
 *
 * @param None.
 *
 * @retval None.
 */
void BMP180::read(void) {

	int32_t UT, UP, B5, pres;
	float temp, altitude;

	UT = readRawTemperature();
	UP = readRawPressure();

	B5 = computeB5(UT);

	temp = calculateTemperature(UT, B5);
	pres = calculatePressure(UP, B5);
	altitude = calculateAltitude(pres);

	bmp180data.temperature = temp;
	bmp180data.pressure = pres;
	bmp180data.altitude = altitude;
}

/*!
 * @brief BMP180 getTemperature method.

 * Returns the temperature previously read.
 *
 * @param None.
 *
 * @retval temperature in Celsius.
 */
float BMP180::getTemperature(void) {
	return bmp180data.temperature;
}

/*!
 * @brief BMP180 getPressure method.

 * Returns the pressure previously read.
 *
 * @param None.
 *
 * @retval pressure in Pa.
 */
int32_t BMP180::getPressure(void) {
	return bmp180data.pressure;
}

/*!
 * @brief BMP180 getAltitude method.

 * Returns the altitude previously calculated.
 *
 * @param None.
 *
 * @retval altitude in meters.
 */
float BMP180::getAltitude(void) {
	return bmp180data.altitude;
}

/*!
 * @brief BMP180 calibrate private method.

 * Reads the calibration data.
 *
 * @param i2c index of the I2C used. Defaults to 0.
 * @param mode resolution given by the mode of operation. Defaults
 * to BMP180_ULTRAHIGHRES
 *
 * @retval true if the communication had no issues.
 */
bool BMP180::calibrate(uint32_t i2c, uint8_t mode) {

	if (mode > BMP180_ULTRAHIGHRES) {
		mode = BMP180_ULTRAHIGHRES;
	}

	oversampling = mode;

	if (read8(0xD0) != 0x55) {
		return false;
	}

	/* read calibration data */
	bmp180calibrationData.ac1 = read16(BMP180_CAL_AC1);
	bmp180calibrationData.ac2 = read16(BMP180_CAL_AC2);
	bmp180calibrationData.ac3 = read16(BMP180_CAL_AC3);
	bmp180calibrationData.ac4 = read16(BMP180_CAL_AC4);
	bmp180calibrationData.ac5 = read16(BMP180_CAL_AC5);
	bmp180calibrationData.ac6 = read16(BMP180_CAL_AC6);

	bmp180calibrationData.b1 = read16(BMP180_CAL_B1);
	bmp180calibrationData.b2 = read16(BMP180_CAL_B2);

	bmp180calibrationData.mb = read16(BMP180_CAL_MB);
	bmp180calibrationData.mc = read16(BMP180_CAL_MC);
	bmp180calibrationData.md = read16(BMP180_CAL_MD);

	return true;
}

/*!
 * @brief BMP180 readRawTemperature private method.

 * Reads and returns temperature without correction.
 *
 * @param None.
 *
 * @retval raw temperature.
 */
uint16_t BMP180::readRawTemperature(void) {

	write8(BMP180_CONTROL, BMP180_READTEMPCMD);
	for(uint32_t i = 0 ; i < 100000 ; i++);
	return read16(BMP180_TEMPDATA);
}

/*!
 * @brief BMP180 computeB5 private method.

 * Calculates a correction factor.
 *
 * @param UT raw temperature.
 *
 * @retval correction factor (B5).
 */
int32_t BMP180::computeB5(int32_t UT) {

	int32_t X1 = (UT - (int32_t)bmp180calibrationData.ac6) * ((int32_t)bmp180calibrationData.ac5) >> 15;
	int32_t X2 = ((int32_t)bmp180calibrationData.mc << 11) / (X1 + (int32_t)bmp180calibrationData.md);
	return X1 + X2;
}

/*!
 * @brief BMP180 readRawPressure private method.

 * Reads and returns pressure without correction.
 *
 * @param None.
 *
 * @retval raw pressure.
 */
uint32_t BMP180::readRawPressure(void) {

	uint32_t raw;

	write8(BMP180_CONTROL, BMP180_READPRESSURECMD + (oversampling << 6));

	if (oversampling == BMP180_ULTRALOWPOWER) {
		for(uint32_t i = 0 ; i < 100000 ; i++);
	}
	else if (oversampling == BMP180_STANDARD) {
		for(uint32_t i = 0 ; i < 200000 ; i++);
	}
	else if (oversampling == BMP180_HIGHRES) {
		for(uint32_t i = 0 ; i < 400000 ; i++);
	}
	else {
		for(uint32_t i = 0 ; i < 800000 ; i++);
	}

	raw = read16(BMP180_PRESSUREDATA);

	raw <<= 8;
	raw |= read8(BMP180_PRESSUREDATA + 2);
	raw >>= (8 - oversampling);
	return raw;
}

/*!
 * @brief BMP180 calculateTemperature private method.

 * Calculates the proper temperature.
 *
 * @param UT raw temperature.
 * @param B5 correction factor.
 *
 * @retval temperature.
 */
float BMP180::calculateTemperature(int32_t UT, int32_t B5) {

	float temp = (B5 + 8) >> 4;
	temp /= 10;

	return temp;
}

/*!
 * @brief BMP180 calculatePressure private method.

 * Calculates the proper pressure.
 *
 * @param UP raw pressure.
 * @param B5 correction factor.
 *
 * @retval pressure.
 */
int32_t BMP180::calculatePressure(int32_t UP, int32_t B5) {

	int32_t B3, B6, X1, X2, X3, p;
	uint32_t B4, B7;

	// Do pressure calculations
	B6 = B5 - 4000;
	X1 = ((int32_t)bmp180calibrationData.b2 * ((B6 * B6) >> 12)) >> 11;
	X2 = ((int32_t)bmp180calibrationData.ac2 * B6) >> 11;
	X3 = X1 + X2;
	B3 = ((((int32_t)bmp180calibrationData.ac1 * 4 + X3) << oversampling) + 2) / 4;

	X1 = ((int32_t)bmp180calibrationData.ac3 * B6) >> 13;
	X2 = ((int32_t)bmp180calibrationData.b1 * ((B6 * B6) >> 12)) >> 16;
	X3 = ((X1 + X2) + 2) >> 2;
	B4 = ((uint32_t)bmp180calibrationData.ac4 * (uint32_t)(X3 + 32768)) >> 15;
	B7 = ((uint32_t)UP - B3) * (uint32_t)(50000UL >> oversampling);

	if (B7 < 0x80000000) {
	p = (B7 * 2) / B4;
	} else {
	p = (B7 / B4) * 2;
	}
	X1 = (p >> 8) * (p >> 8);
	X1 = (X1 * 3038) >> 16;
	X2 = (-7357 * p) >> 16;

	p = p + ((X1 + X2 + (int32_t)3791) >> 4);
	return p;
}

/*!
 * @brief BMP180 calculateAltitude private method.

 * Calculates the altitude for a given pressure.
 *
 * @param pressure given pressure.
 * @param sealevelPressure defaults to 101325 Pa.
 *
 * @retval altitude in meters.
 */
float BMP180::calculateAltitude(int32_t pressure, float sealevelPressure) {

	float altitude;
	altitude = 44330 * (1.0 - pow((float)pressure / sealevelPressure, 0.1903));
	return altitude;
}

/*!
 * @brief BMP180 read8 private method.

 * Reads a byte from the BMP180.
 *
 * @param reg register to read.
 *
 * @retval 8-bit value from the register.
 */
uint8_t BMP180::read8(uint8_t reg) {

	uint8_t ret;

	if (kStatus_Success == i2c.write(BMP180_ADDRESS, &reg, 1)) {

		if (kStatus_Success == i2c.read(BMP180_ADDRESS, &ret, 1)) {
			return ret;
		}
	}
	return 0x00;
}

/*!
 * @brief BMP180 read16 private method.

 * Reads two bytes from the BMP180.
 *
 * @param reg register to read.
 *
 * @retval 16-bit value from the two registers.
 */
uint16_t BMP180::read16(uint8_t reg) {

	uint8_t retbuf[2];
	uint16_t ret;

	if (kStatus_Success == i2c.write(BMP180_ADDRESS, &reg, 1)) {

		if (kStatus_Success == i2c.read(BMP180_ADDRESS, retbuf, 2)) {
			ret = retbuf[1] | (retbuf[0] << 8);
			return ret;
		}
	}
	return 0x0000;
}

/*!
 * @brief BMP180 write8 private method.

 * Writes a byte to the BMP180.
 *
 * @param reg register to write.
 * @param data byte to write
 *
 * @retval communication status.
 */
status_t BMP180::write8(uint8_t reg, uint8_t data) {

	return i2c.write(reg, &data, 1);
}
