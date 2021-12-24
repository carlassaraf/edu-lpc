/*
 * BMP180.h
 *
 *  Created on: Dec 3, 2021
 *      Author: fabri
 */

#ifndef BMP180_H_
#define BMP180_H_

/*  BMP180 includes  */

#include <math.h>
#include "I2C.h"

/*  BMP180 data structs  */

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

typedef enum Bmp180StatusCode {
	kbmp180_ok,
	kbmp180_uncompensatedTemperature_err,
	kbmp180_uncompensatedPressure_err
} bmp180_status_code_t;

/*  BMP180 definitions  */

#define BMP180_ADDRESS	0x77	// BMP180 address

/*  Modes of operation  */
#define BMP180_ULTRALOWPOWER	0 	// Ultra low power mode (max conversion time 4.5 ms)
#define BMP180_STANDARD 		1   // Standard mode (max conversion time 7.5 ms)
#define BMP180_HIGHRES 			2   // High-res mode (max conversion time 13.5 ms)
#define BMP180_ULTRAHIGHRES 	3  	// Ultra high-res mode (max conversion time 25.5 ms)

#ifndef BMP180_OSS
#define BMP180_OSS		BMP180_OSS_STANDARD
#endif

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

/*  Class definition  */

class BMP180 {

	I2C i2c;

	public:

	  BMP180(uint32_t i2cn = 0, uint8_t mode = BMP180_ULTRAHIGHRES) : i2c(i2cn, 3000000U) { calibrate(i2cn, mode); };
	  void read(void);
	  float getTemperature(void);
	  int32_t getPressure(void);
	  float getAltitude(void);


	private:

	  bool calibrate(uint32_t i2c = 0, uint8_t mode = BMP180_ULTRAHIGHRES);
	  uint16_t readRawTemperature(void);
	  int32_t computeB5(int32_t UT);
	  uint32_t readRawPressure(void);
	  float calculateTemperature(int32_t UT, int32_t B5);
	  int32_t calculatePressure(int32_t UP, int32_t B5);
	  float calculateAltitude(int32_t pressure, float sealevelPressure = 101325); // std atmosphere
	  int32_t readSealevelPressure(float altitude_meters = 0);

	  uint8_t read8(uint8_t reg);
	  uint16_t read16(uint8_t reg);
	  status_t write8(uint8_t reg, uint8_t data);

	  uint8_t oversampling;
	  bmp180_calibrationData_t bmp180calibrationData;
	  bmp180_data_t bmp180data;
};

#endif /* BMP180_H_ */
