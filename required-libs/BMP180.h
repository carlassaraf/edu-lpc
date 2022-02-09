/*
 * BMP180.h
 *
 *  Created on: Dec 3, 2021
 *      Author: fabri
 */

#ifndef BMP180_H_
#define BMP180_H_

/*  Includes  */
#include <delay.h>
#include <math.h>
#include "I2C.h"

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

/* BMP180 address */
#define BMP180_ADDRESS	0x77

/* Calibration data (16 bits) */
#define BMP180_CAL_AC1 	0xAA
#define BMP180_CAL_AC2 	0xAC
#define BMP180_CAL_AC3 	0xAE
#define BMP180_CAL_AC4 	0xB0
#define BMP180_CAL_AC5 	0xB2
#define BMP180_CAL_AC6	0xB4
#define BMP180_CAL_B1 	0xB6
#define BMP180_CAL_B2 	0xB8
#define BMP180_CAL_MB 	0xBA
#define BMP180_CAL_MC 	0xBC
#define BMP180_CAL_MD 	0xBE

/* Control register */
#define BMP180_CONTROL 			0xF4
/* Temperature data register */
#define BMP180_TEMPDATA 		0xF6
/* Pressure data register */
#define BMP180_PRESSUREDATA 	0xF6
/* Read temperature control register value */
#define BMP180_READTEMPCMD 		0x2E
/* Read pressure control register value */
#define BMP180_READPRESSURECMD 	0x34

/* Class definition */
class BMP180 {

	public:
	  /* Ultra low power mode (max conversion time 4.5 ms) */
	  static constexpr uint8_t UltraLowPower {0};
	  /* Standard mode (max conversion time 7.5 ms) */
	  static constexpr uint8_t Standard {1};
	  /* High-res mode (max conversion time 13.5 ms) */
	  static constexpr uint8_t HighResolution {2};
	  /* Ultra high-res mode (max conversion time 25.5 ms) */
	  static constexpr uint8_t UltraHighRes {3};
	  /* Contructors */
	  BMP180(uint8_t i2c = 0, uint8_t mode = UltraHighRes);
	  /* Public methods */
	  void read(void);
	  float getTemperature(void);
	  int32_t getPressure(void);
	  float getAltitude(void);

	private:
	  /* I2C peripherial pointer */	
	  I2C_Type *i2c_base;
	  /* Type of resolution */
	  uint8_t oversampling;
	  /* Calibration data */
	  bmp180_calibrationData_t bmp180calibrationData;
	  /* Data results */
	  bmp180_data_t bmp180data;
	  /* Private methods */
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
 * @brief BMP180 temperature method.

 * Returns the temperature previously read.
 *
 * @param None.
 *
 * @retval temperature in Celsius.
 */
inline float BMP180::temperature(void) { return bmp180data.temperature; }

/*!
 * @brief BMP180 pressure method.

 * Returns the pressure previously read.
 *
 * @param None.
 *
 * @retval pressure in Pa.
 */
inline int32_t BMP180::getPressure(void) { return bmp180data.pressure; }

/*!
 * @brief BMP180 altitude method.

 * Returns the altitude previously calculated.
 *
 * @param None.
 *
 * @retval altitude in meters.
 */
inline float BMP180::altitude(void) { return bmp180data.altitude; }

#endif /* BMP180_H_ */
