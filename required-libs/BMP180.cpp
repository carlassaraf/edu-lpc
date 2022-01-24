/*
 * BMP180.cpp
 *
 *  Created on: Dec 3, 2021
 *      Author: fabri
 */

#include <BMP180.h>

/*!
 * @brief BMP180 constructor.

 * Creates a BMP object and does the calibration.
 *
 * @param i2cn index of the I2C peripherial to be used.
 * Defaults to 0;
 * @param mode type of sampling. Defautls to BMP180_ULTRAHIGHRES.
 *
 * @retval None.
 */
BMP180::BMP180(uint8_t i2c, uint8_t mode) {

	if (mode > ULTRAHIGHRES) {
		mode = ULTRAHIGHRES;
	}

	/* Creates I2C object to initialize peripherial */
	I2C *i2cobj = new I2C(i2c, 3000000U);

	/* Gets the I2C peripherial pointer */
	i2c_base = i2cobj->getI2C();

	/* Free memory */
	delete i2cobj;

	oversampling = mode;
	calibrate();
}

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

	temp = calculateTemperature(B5);
	pres = calculatePressure(UP, B5);
	altitude = calculateAltitude(pres);

	bmp180data.temperature = temp;
	bmp180data.pressure = pres;
	bmp180data.altitude = altitude;
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
bool BMP180::calibrate(void) {

	if (read(0xD0, 1) != 0x55) {
		return false;
	}

	int16_t *ptr;
	ptr = &(bmp180calibrationData.ac1);
	/* Read calibration data */
	for(uint8_t reg = BMP180_CAL_AC1; reg < 0xBF; reg += 0x02) {
		*ptr = read(reg, 2);
		ptr ++;
	}

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

	write(BMP180_CONTROL, BMP180_READTEMPCMD);
	delay(10);
	return read(BMP180_TEMPDATA, 2);
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

	write(BMP180_CONTROL, BMP180_READPRESSURECMD + (oversampling << 6));

	if (oversampling == ULTRALOWPOWER) { delay(5); }
	else if (oversampling == STANDARD) { delay(8); }
	else if (oversampling == HIGHRESOLUTION) { delay(14); }
	else { delay(27); }

	raw = read(BMP180_PRESSUREDATA, 2);

	raw <<= 8;
	raw |= read(BMP180_PRESSUREDATA + 2, 1);
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
float BMP180::calculateTemperature(int32_t B5) {

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
 * @brief BMP180 read private method.

 * Reads a number of bytes from the BMP180.
 *
 * @param reg register to read.
 *
 * @retval 16-bit or 8-bit value from the registers.
 */
uint16_t BMP180::read(uint8_t reg, uint8_t size) {
	/* Array to store the data */
	uint8_t retbuf[2];
	/* Write Address and RW bit to data register */
	i2c_base->MSTDAT = ((uint32_t)BMP180_ADDRESS << 1) | ((uint32_t)kI2C_Write & 1u);
	/* Start the transfer */
	i2c_base->MSTCTL = I2C_MSTCTL_MSTSTART_MASK;
	/* Access register */
	I2C_MasterWriteBlocking(i2c_base, &reg, 1, kI2C_TransferDefaultFlag);
	/* Stop transfer */
	i2c_base->MSTCTL = I2C_MSTCTL_MSTSTOP_MASK;
	/* Write Address and RW bit to data register */
	i2c_base->MSTDAT = ((uint32_t)BMP180_ADDRESS << 1) | ((uint32_t)kI2C_Read & 1u);
	/* Start the transfer */
	i2c_base->MSTCTL = I2C_MSTCTL_MSTSTART_MASK;
	/* Read two bytes */
	I2C_MasterReadBlocking(i2c_base, retbuf, size, kI2C_TransferDefaultFlag);
	/* Stop transfer */
	i2c_base->MSTCTL = I2C_MSTCTL_MSTSTOP_MASK;
	/* Return 16-bit value or 8-bit value */
	return (size == 2)? (retbuf[0] << 8) | retbuf[1] : (retbuf[0]);
}

/*!
 * @brief BMP180 write private method.

 * Writes a byte to the BMP180.
 *
 * @param reg register to write.
 * @param data byte to write
 *
 * @retval communication status.
 */
void BMP180::write(uint8_t reg, uint8_t data) {
	/* Array to send */
	uint8_t buf[] = { reg, data };
	/* Write Address and RW bit to data register */
	i2c_base->MSTDAT = ((uint32_t)BMP180_ADDRESS << 1) | ((uint32_t)kI2C_Write & 1u);
	/* Start the transfer */
	i2c_base->MSTCTL = I2C_MSTCTL_MSTSTART_MASK;
	/* Write register */
	I2C_MasterWriteBlocking(i2c_base, buf, 2, kI2C_TransferDefaultFlag);
	/* initiate NAK and stop */
	i2c_base->MSTCTL = I2C_MSTCTL_MSTSTOP_MASK;
}
