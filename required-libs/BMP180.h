/*
 * BMP180.h
 *
 *  Created on: Dec 3, 2021
 *      Author: fabri
 */

#ifndef BMP180_H_
#define BMP180_H_

/*  BMP180 includes  */
#include <delay.h>
#include <math.h>
#include "I2C.h"
#include "fsl_i2c.h"

/* BMP180 data structs */
typedef struct Bmp180CalibrationData {
    int16_t ac1;
    int16_t ac2;
    int16_t ac3;
    uint16_t ac4;
    uint16_t ac5;
    uint16_t ac6;
    int16_t b1;
    int16_t b2;
    int16_t mb;
    int16_t mc;
    int16_t md;
} bmp180_calibrationData_t;

typedef struct Bmp180Data {
    float temperature;
    long pressure;
    float altitude;
} bmp180_data_t;

/*  BMP180 definitions  */

#define BMP180_ADDRESS	0x77	// BMP180 address

/*  Register addresses  */
#define BMP180_CAL_AC1 	0xAA    	// Calibration data (16 bits)
#define BMP180_CAL_AC2 	0xAC    	// Calibration data (16 bits)
#define BMP180_CAL_AC3 	0xAE    	// Calibration data (16 bits)
#define BMP180_CAL_AC4 	0xB0    	// Calibration data (16 bits)
#define BMP180_CAL_AC5 	0xB2    	// Calibration data (16 bits)
#define BMP180_CAL_AC6	0xB4    	// Calibration data (16 bits)
#define BMP180_CAL_B1 	0xB6     	// Calibration data (16 bits)
#define BMP180_CAL_B2 	0xB8     	// Calibration data (16 bits)
#define BMP180_CAL_MB 	0xBA     	// Calibration data (16 bits)
#define BMP180_CAL_MC 	0xBC     	// Calibration data (16 bits)
#define BMP180_CAL_MD 	0xBE     	// Calibration data (16 bits)

#define BMP180_CONTROL 			0xF4    // Control register
#define BMP180_TEMPDATA 		0xF6    // Temperature data register
#define BMP180_PRESSUREDATA 	0xF6    // Pressure data register
#define BMP180_READTEMPCMD 		0x2E    // Read temperature control register value
#define BMP180_READPRESSURECMD 	0x34 	// Read pressure control register value

/* Class definition */
class BMP180 {

	public:

	  BMP180(uint8_t i2c = 0, uint8_t mode = ULTRAHIGHRES);
	  void read(void);
	  float getTemperature(void);
	  int32_t getPressure(void);
	  float getAltitude(void);

	  /* Ultra low power mode (max conversion time 4.5 ms) */
	  static constexpr uint8_t ULTRALOWPOWER {0};
	  /* Standard mode (max conversion time 7.5 ms) */
	  static constexpr uint8_t STANDARD {1};
	  /* High-res mode (max conversion time 13.5 ms) */
	  static constexpr uint8_t HIGHRESOLUTION {2};
	  /* Ultra high-res mode (max conversion time 25.5 ms) */
	  static constexpr uint8_t ULTRAHIGHRES {3};

	private:

	  I2C_Type *i2c_base;
	  uint8_t oversampling;
	  bmp180_calibrationData_t bmp180calibrationData;
	  bmp180_data_t bmp180data;

	  bool calibrate(void);
	  uint16_t readRawTemperature(void);
	  int32_t computeB5(int32_t UT);
	  uint32_t readRawPressure(void);
	  float calculateTemperature(int32_t B5);
	  int32_t calculatePressure(int32_t UP, int32_t B5);
	  float calculateAltitude(int32_t pressure, float sealevelPressure = 101325); // std atmosphere
	  int32_t readSealevelPressure(float altitude_meters = 0);

	  uint16_t read(uint8_t reg, uint8_t size);
	  void write(uint8_t reg, uint8_t data);
};

/* Inline methods */

/*!
 * @brief BMP180 getTemperature method.

 * Returns the temperature previously read.
 *
 * @param None.
 *
 * @retval temperature in Celsius.
 */
inline float BMP180::getTemperature(void) {
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
inline int32_t BMP180::getPressure(void) {
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
inline float BMP180::getAltitude(void) {
	return bmp180data.altitude;
}

#endif /* BMP180_H_ */
