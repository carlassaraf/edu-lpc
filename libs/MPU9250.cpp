/*
 * MPU9250.cpp
 *
 *  Created on: Jan 18, 2022
 *      Author: fabri
 */

#include "MPU9250.h"

/*!
 * @brief MPU9250 constructor.

 * Creates a MPU9250 object.
 *
 * @param address 7-bit address of the MPU device. Defaults to 0x68.
 * @param i2cn index of the I2C peripherial to be used. Defaults to 1.
 * @param frequency clock frequency for que SCL pin. Defaults to 400KHz.
 * @param mpu_setting configuration struct to set the parameters of the MPU.
 * Defaults to:
 * - accel_fs_sel = ACCEL_FS_SEL::A16G
 * - gyro_fs_sel = GYRO_FS_SEL::G2000DPS
 * - mag_output_bits = MAG_OUTPUT_BITS::M16BITS
 * - fifo_sample_rate = FIFO_SAMPLE_RATE::SMPL_200HZ
 * - gyro_fchoice = 0x03
 * - gyro_dlpf_cfg = GYRO_DLPF_CFG::DLPF_41HZ
 * - accel_fchoice = 0x01
 * - accel_dlpf_cfg ACCEL_DLPF_CFG::DLPF_45HZ
 *
 * @retval None.
 */
MPU9250::MPU9250(uint8_t address, uint32_t i2cn, uint32_t frequency, const MPU9250Setting mpu_setting) {

	mpu_i2c_addr = address;

	setting = mpu_setting;

	/* Creates I2C object to initialize peripherial */
	I2C *i2cobj = new I2C(i2cn, frequency);

	/* Get pointer to I2C peripherial */
	i2c = i2cobj->getI2C();

	/* Free memory */
	delete i2cobj;

	/* Check for connections to the MPU and AK */
	if (isConnectedMPU9250() && isConnectedAK8963()) {

		has_connected = true;
		/* Calibrate and initialize */
		calibrateAccelGyro();
		calibrateMag();
	}
}

/*!
 * @brief MPU9250 update method.

 * Updates the MPU data values to be used in the main program.
 *
 * @param None.
 *
 * @retval true if data was read, false if there was an error.
 */
bool MPU9250::update(void) {

	/* Check if the device is available */
	if (!available()) {
		return false;
	}

	uint8_t j = 0;
	/* Used to read all 14 bytes at once from the MPU9250 accel/gyro */
	uint8_t raw_data[14];
	/* Raw accel, gyro and magnet register data stored here */
	int16_t raw_acc_gyro_mag_data[7];
	/* Read the 14 raw data registers into data array */
	read(mpu_i2c_addr, ACCEL_XOUT_H, raw_data, 14);
	for(uint8_t i = 0; i < 14; i += 2) {
		/* Build 16-bit raw data */
		raw_acc_gyro_mag_data[j] = ((int16_t)raw_data[i] << 8) | (int16_t)raw_data[i + 1];
		j++;
	}

	for(uint8_t i = 0; i < 3; i++) {
		/* Now we'll calculate the acceleration value into actual g's */
		mpuData.acc[i] = (float)raw_acc_gyro_mag_data[i] * acc_resolution;
		/* Calculate the gyro value into actual degrees per second */
		mpuData.gy[i] = (float)raw_acc_gyro_mag_data[i + 3] * gyro_resolution;
	}

	/* Read the magnetometer x/y/z ADC values */
	if (read_mag(raw_acc_gyro_mag_data)) {
		/* Calculate the magnetometer values in milliGauss
		 * Include factory calibration per data sheet and user environmental corrections
		 * mag_bias is calculated in 16BITS */
		float bias_to_current_bits = mag_resolution / get_mag_resolution(MAG_OUTPUT_BITS::M16BITS);
		for(uint8_t i = 0; i < 3; i++) {
			/* Store real magnetometer values  */
			mpuData.mg[0] = (float)(raw_acc_gyro_mag_data[i] * mag_resolution * mag_bias_factory[i] - mag_bias[i] * bias_to_current_bits) * mag_scale[i];
		}
	}

	/* Read the temperature ADC values */
	int16_t temperature_raw = read_temperature_data();
	/* Store temperature in degrees Celsius */
	mpuData.temp = ((float)temperature_raw) / 333.87 + 21.0;

	return true;
}

/*!
 * @brief MPU9250 getAccX method.

 * Gets the last stored acceleration value of X axis.
 *
 * @param None.
 *
 * @retval X axis acceleration value.
 */
float MPU9250::getAccX(void) { return mpuData.acc[0]; }

/*!
 * @brief MPU9250 getAccY method.

 * Gets the last stored acceleration value of Y axis.
 *
 * @param None.
 *
 * @retval Y axis acceleration value.
 */
float MPU9250::getAccY(void) { return mpuData.acc[1]; }

/*!
 * @brief MPU9250 getAccZ method.

 * Gets the last stored acceleration value of Z axis.
 *
 * @param None.
 *
 * @retval Z axis acceleration value.
 */
float MPU9250::getAccZ(void) { return mpuData.acc[2]; }

/*!
 * @brief MPU9250 getGyroX method.

 * Gets the last stored gyroscope value of X axis.
 *
 * @param None.
 *
 * @retval X axis gyroscope value.
 */
float MPU9250::getGyroX(void) { return mpuData.gy[0]; }

/*!
 * @brief MPU9250 getGyroY method.

 * Gets the last stored gyroscope value of Y axis.
 *
 * @param None.
 *
 * @retval Y axis gyroscope value.
 */
float MPU9250::getGyroY(void) { return mpuData.gy[1]; }

/*!
 * @brief MPU9250 getGyroZ method.

 * Gets the last stored gyroscope value of Z axis.
 *
 * @param None.
 *
 * @retval Z axis gyroscope value.
 */
float MPU9250::getGyroZ(void) { return mpuData.gy[2]; }

/*!
 * @brief MPU9250 getMagX method.

 * Gets the last stored magnetometer value of X axis.
 *
 * @param None.
 *
 * @retval X axis magnetometer value.
 */
float MPU9250::getMagX(void) { return mpuData.mg[0]; }

/*!
 * @brief MPU9250 getMagY method.

 * Gets the last stored magnetometer value of Y axis.
 *
 * @param None.
 *
 * @retval Y axis magnetometer value.
 */
float MPU9250::getMagY(void) { return mpuData.mg[1]; }

/*!
 * @brief MPU9250 getMagZ method.

 * Gets the last stored magnetometer value of Z axis.
 *
 * @param None.
 *
 * @retval Z axis magnetometer value.
 */
float MPU9250::getMagZ(void) { return mpuData.mg[2]; }

/*!
 * @brief MPU9250 getTemperature method.

 * Gets the last stored temperature value.
 *
 * @param None.
 *
 * @retval temperature value.
 */
float MPU9250::getTemperature(void) { return mpuData.temp; }

/*!
 * @brief MPU9250 isConnectedMPU9250 private method.

 * Checks whether the MPU9250 is connected or not.
 *
 * @param None.
 *
 * @retval true if it's connected, false if it's not.
 */
bool MPU9250::isConnectedMPU9250(void) {

	/* Read and store the result of the WHO AM I register */
	uint8_t c = read(mpu_i2c_addr, WHO_AM_I_MPU9250);
	/* Compare it to the values of the
	 * MPU9250, MPU9255 and MPU6500 */
	bool b = (c == MPU9250_WHOAMI_DEFAULT_VALUE);
	b |= (c == MPU9255_WHOAMI_DEFAULT_VALUE);
	b |= (c == MPU6500_WHOAMI_DEFAULT_VALUE);
	return b;
}

/*!
 * @brief MPU9250 isConnectedAK8963 private method.

 * Checks whether the AK8963 is connected or not.
 *
 * @param None.
 *
 * @retval true if it's connected, false if it's not.
 */
bool MPU9250::isConnectedAK8963(void) {

	/* Read and store the result of the WHO AM I register */
	uint8_t c = read(AK8963_ADDRESS, WHO_AM_I_AK8963);
	/* Compare it to the default AK8963 value */
	return (c == AK8963_WHOAMI_DEFAULT_VALUE);
}

/*!
 * @brief MPU9250 available private method.

 * Checks whether the MPU9250 is available or not.
 *
 * @param None.
 *
 * @retval true if it's available, false if it's not.
 */
bool MPU9250::available(void) {

	/* Return true if it's connected  */
	return has_connected && (read(mpu_i2c_addr, INT_STATUS) & 0x01);
}

/*!
 * @brief MPU9250 calibrateAccelGyro private method.

 * Performs the MPU9250 calibations for the given settings.
 *
 * @param None.
 *
 * @retval None.
 */
void MPU9250::calibrateAccelGyro(void) {

	/* Write a one to bit 7 reset bit; toggle reset device */
	write(mpu_i2c_addr, PWR_MGMT_1, 0x80);
	delay(100);
	/* Get stable time source; Auto select clock source to be PLL
	 * gyroscope reference if ready else use the internal
	 * oscillator, bits 2:0 = 001 */
	write(mpu_i2c_addr, PWR_MGMT_1, 0x01);
	write(mpu_i2c_addr, PWR_MGMT_2, 0x00);
	delay(200);
	/* Configure device for bias calculation */
	/* Disable all interrupts */
	write(mpu_i2c_addr, INT_ENABLE, 0x00);
	/* Disable FIFO */
	write(mpu_i2c_addr, FIFO_EN, 0x00);
	/* Turn on internal clock source */
	write(mpu_i2c_addr, PWR_MGMT_1, 0x00);
	/* Disable I2C master */
	write(mpu_i2c_addr, I2C_MST_CTRL, 0x00);
	/* Disable FIFO and I2C master modes */
	write(mpu_i2c_addr, USER_CTRL, 0x00);
	/* Reset FIFO and DMP */
	write(mpu_i2c_addr, USER_CTRL, 0x0C);
	delay(15);
	/* Configure MPU6050 gyro and accelerometer for bias calculation */
	/* Set low-pass filter to 188 Hz */
	write(mpu_i2c_addr, MPU_CONFIG, 0x01);
	/* Set sample rate to 1 kHz */
	write(mpu_i2c_addr, SMPLRT_DIV, 0x00);
	/* Set gyro full-scale to 250 degrees per second, maximum sensitivity */
	write(mpu_i2c_addr, GYRO_CONFIG, 0x00);
	/* Set accelerometer full-scale to 2 g, maximum sensitivity */
	write(mpu_i2c_addr, ACCEL_CONFIG, 0x00);
	/* Configure FIFO to capture accelerometer and gyro data for bias calculation */
	/* Enable FIFO */
	write(mpu_i2c_addr, USER_CTRL, 0x40);
	/* Enable gyro and accelerometer sensors for FIFO  (max size 512 bytes in MPU-9150) */
	write(mpu_i2c_addr, FIFO_EN, 0x78);
	/* Accumulate 40 samples in 40 milliseconds = 480 bytes */
	delay(40);
	/* At end of sample accumulation, turn off FIFO sensor read */
	uint8_t j = 0;
	/* Data array to hold accelerometer and gyro x, y, z, data */
	uint8_t data[12];
	/* Disable gyro and accelerometer sensors for FIFO */
	write(mpu_i2c_addr, FIFO_EN, 0x00);
	/* Read FIFO sample count */
	read(mpu_i2c_addr, FIFO_COUNTH, data, 2);
	uint16_t fifo_count = ((uint16_t)data[0] << 8) | data[1];
	/* How many sets of full gyro and accelerometer data for averaging */
	uint16_t packet_count = fifo_count / 12;

	for (uint16_t ii = 0; ii < packet_count; ii++) {

		/* Temporal arrays to calculate 16-bit values */
		int16_t accel_temp[3] = {0, 0, 0}, gyro_temp[3] = {0, 0, 0};
		/* Read data for averaging */
		read(mpu_i2c_addr, FIFO_R_W, data, 12);

		j = 0;
		for(uint8_t i = 0; i < 6; i += 2) {
			/* Form signed 16-bit integer for each sample in FIFO */
			accel_temp[j] = (int16_t)(((int16_t)data[i] << 8) | data[i + 1]);
			gyro_temp[j] = (int16_t)(((int16_t)data[i + 6] << 8) | data[i + 7]);
			/* Sum individual signed 16-bit biases to get accumulated signed 32-bit biases */
			acc_bias[j] += (float)accel_temp[j];
			gyro_bias[j] += (float)gyro_temp[j];
			j++;
		}
	}

	for(uint8_t i = 0; i < 3; i++) {
		/* Normalize sums to get average count biases */
		acc_bias[i] /= (float)packet_count;
		gyro_bias[i] /= (float)packet_count;
	}
	/* Remove gravity from the z-axis accelerometer bias calculation */
	if (acc_bias[2] > 0L) { acc_bias[2] -= (float)CALIB_ACCEL_SENSITIVITY; }
	else { acc_bias[2] += (float)CALIB_ACCEL_SENSITIVITY; }
	/* Construct the accelerometer biases for push to the hardware accelerometer bias registers. These registers contain
	 * factory trim values which must be added to the calculated accelerometer biases; on boot up these registers will hold
	 * non-zero values. In addition, bit 0 of the lower byte must be preserved since it is used for temperature
	 * compensation calculations. Accelerometer bias registers expect bias input as 2048 LSB per g, so that
	 * the accelerometer biases calculated above must be divided by 8. */
	j = 0;
	/* A place to hold the factory accelerometer trim biases */
	int16_t acc_bias_reg[3] = {0, 0, 0};
	/* Define array to hold mask bit for each accelerometer bias axis */
	int16_t mask_bit[3] = {1, 1, 1};
	for (uint8_t i = XA_OFFSET_H; i < 0x7f; i += 0x03) {
		/* Read factory accelerometer trim values */
		read(mpu_i2c_addr, XA_OFFSET_H, data, 2);
		/* Form signed 16-bit integer */
		acc_bias_reg[j] = ((int16_t)data[0] << 8) | data[1];

		if (acc_bias_reg[j] % 2) {
			mask_bit[j] = 0;
		}
		/* Subtract calculated averaged accelerometer bias scaled to 2048 LSB/g */
		acc_bias_reg[j] -= (int16_t)acc_bias[j] >> 3;
		/* Preserve temperature compensation bit */
		if (mask_bit[j]) { acc_bias_reg[j] = acc_bias_reg[j] & ~mask_bit[j]; }
		/* Preserve temperature compensation bit */
		else { acc_bias_reg[j] = acc_bias_reg[j] | 0x0001; }
		j++;
	}

	j = 0;
	for(int i = 0; i < 3; i += 2) {
		/* Form unsigned 8-bit velues for accel and gyro biases */
		data[i] = (acc_bias_reg[j] >> 8) & 0xFF;
		data[i + 1] = (acc_bias_reg[j]) & 0xFF;
		/* Divide by 4 to get 32.9 LSB per deg/s to conform to expected bias input format */
		data[i + 2] = (-(int16_t)gyro_bias[j] / 4 >> 8) & 0xFF;
		/* Biases are additive, so change sign on calculated average gyro biases */
		data[i + 3] = (-(int16_t)gyro_bias[j] / 4) & 0xFF;
		j++;
	}

	j = 0;
	/* Push accelerometer biases to hardware registers */
	for(uint8_t reg = XA_OFFSET_H; reg < 0x7f; reg += 0x03) {
		/* Start with XA_OFFSET_H finish with ZA_OFFSET_L */
		write(mpu_i2c_addr, reg, data[j]);
		write(mpu_i2c_addr, reg + 0x01, data[j + 1]);
		j += 2;
	}

	j = 0;
	/* Push gyro biases to hardware registers */
	for(uint8_t reg = XG_OFFSET_H; reg < 0x19; reg++) {
		/* Start with XG_OFFSET_H finish with ZG_OFFSET_L */
		write(mpu_i2c_addr, reg, data[j]);
		j++;
	}

	/* Initialize MPU9250 */
	delay(100);
	initMPU9250();
	delay(1000);
}

/*!
 * @brief MPU9250 calibrateMag private method.

 * Performs the AK8963 calibations for the given settings.
 *
 * @param None.
 *
 * @retval None.
 */
void MPU9250::calibrateMag(void) {

	/* Initialize AK8963 */
	initAK8963();
	delay(4000);

	/* Shoot for ~fifteen seconds of mag data */
	uint16_t sample_count = 0;
	/* At 8 Hz ODR, new mag data is available every 125 ms */
	if (MAG_MODE == 0x02) { sample_count = 128; }
	/* t 100 Hz ODR, new mag data is available every 10 ms */
	else if (MAG_MODE == 0x06)  { sample_count = 1500; }

	/* Arrays for bias, scale, auxiliar and min max values*/
	int32_t bias[3] = {0, 0, 0}, scale[3] = {0, 0, 0};
	int16_t mag_max[3] = {-32767, -32767, -32767};
	int16_t mag_min[3] = {32767, 32767, 32767};
	int16_t mag_temp[3] = {0, 0, 0};

	for (uint16_t i = 0; i < sample_count; i++) {
		/* Read the mag data */
		read_mag(mag_temp);
		for (uint8_t j = 0; j < 3; j++) {
			if (mag_temp[j] > mag_max[j]) { mag_max[j] = mag_temp[j];  }
			if (mag_temp[j] < mag_min[j]) { mag_min[j] = mag_temp[j]; }
		}
		/* At 8 Hz ODR, new mag data is available every 125 ms */
		if (MAG_MODE == 0x02) { delay(135); }
		/* t 100 Hz ODR, new mag data is available every 10 ms */
		else if (MAG_MODE == 0x06) { delay(12); }
	}

	float bias_resolution = get_mag_resolution(MAG_OUTPUT_BITS::M16BITS);
	for(uint8_t i = 0; i < 3; i++) {
		/* Get hard iron correction */
		/* Get average x mag bias in counts */
		bias[i] = (mag_max[i] + mag_min[i]) / 2;
		/* Save mag biases */
		mag_bias[i] = (float)bias[i] * bias_resolution * mag_bias_factory[i];
		/* Get soft iron correction estimate */
		/* Get average x axis max chord length in counts */
		scale[i] = (float)(mag_max[i] - mag_min[i]) * mag_bias_factory[i] / 2;
	}

	/* Multiplication by mag_bias_factory added in accordance with the following comment
	 * https://github.com/kriswiner/MPU9250/issues/456#issue-836657973 */
	float avg_rad = scale[0] + scale[1] + scale[2];
	avg_rad /= 3.0;
	for(uint8_t i = 0; i < 3; i++) {
		mag_scale[i] = avg_rad / ((float)scale[i]);
	}
}

/*!
 * @brief MPU9250 initMPU9250 private method.

 * Initializes the MPU9250 with given settings.
 *
 * @param None.
 *
 * @retval None.
 */
void MPU9250::initMPU9250(void) {

	/* Get the accel and gyro resolutions */
	acc_resolution = get_acc_resolution(setting.accel_fs_sel);
	gyro_resolution = get_gyro_resolution(setting.gyro_fs_sel);
	/* Write a one to bit 7 reset bit; toggle reset device */
	write(mpu_i2c_addr, PWR_MGMT_1, 0x80);
	delay(100);
	/* Wake up device. Clear sleep mode bit (6), enable all sensors */
	write(mpu_i2c_addr, PWR_MGMT_1, 0x00);
	/* Wait for all registers to reset */
	delay(100);
	/* Auto select clock source to be PLL gyroscope reference if ready else */
	write(mpu_i2c_addr, PWR_MGMT_1, 0x01);
	delay(200);
	/* Configure Gyro and Thermometer
	 * Disable FSYNC and set thermometer and gyro bandwidth to 41 and 42 Hz, respectively;
	 * minimum delay time for this setting is 5.9 ms, which means sensor fusion update rates cannot
	 * be higher than 1 / 0.0059 = 170 Hz
	 * GYRO_DLPF_CFG = bits 2:0 = 011; this limits the sample rate to 1000 Hz for both
	 * With the MPU9250, it is possible to get gyro sample rates of 32 kHz (!), 8 kHz, or 1 kHz */
	uint8_t mpu_config = (uint8_t)setting.gyro_dlpf_cfg;
	write(mpu_i2c_addr, MPU_CONFIG, mpu_config);
	/* Set sample rate = gyroscope output rate/(1 + SMPLRT_DIV)
	 * Use a 200 Hz rate; a rate consistent with the filter update rate */
	uint8_t sample_rate = (uint8_t)setting.fifo_sample_rate;
	write(mpu_i2c_addr, SMPLRT_DIV, sample_rate);
	/* Set gyroscope full scale range. Range selects FS_SEL and GFS_SEL are 0 - 3,
	 * so 2-bit values are left-shifted into positions 4:3 */

	/* Get current GYRO_CONFIG register value */
	uint8_t c = read(mpu_i2c_addr, GYRO_CONFIG);
	/* Clear self-test bits [7:5] */
	c = c & ~0xE0;
	/* Clear Fchoice bits [1:0] */
	c = c & ~0x03;
	/* Clear GYRO_FS_SEL bits [4:3] */
	c = c & ~0x18;
	/* Set full scale range for the gyro */
	c = c | (uint8_t(setting.gyro_fs_sel) << 3);
	/* Set Fchoice for the gyro */
	c = c | (uint8_t(~setting.gyro_fchoice) & 0x03);
	/* Write new GYRO_CONFIG value to register */
	write(mpu_i2c_addr, GYRO_CONFIG, c);
	/* Set accelerometer full-scale range configuration
	 * Get current ACCEL_CONFIG register value */
	c = read(mpu_i2c_addr, ACCEL_CONFIG);
	/* Clear self-test bits [7:5] */
	c = c & ~0xE0;
	/* Clear ACCEL_FS_SEL bits [4:3] */
	c = c & ~0x18;
	/* Set full scale range for the accelerometer */
	c = c | (uint8_t(setting.accel_fs_sel) << 3);
	/* Write new ACCEL_CONFIG register value */
	write(mpu_i2c_addr, ACCEL_CONFIG, c);
	/* Set accelerometer sample rate configuration
	 * It is possible to get a 4 kHz sample rate from
	 * the accelerometer by choosing 1 for accel_fchoice_b
	 * bit [3]; in this case the bandwidth is 1.13 kHz */

	/* Get current ACCEL_CONFIG2 register value */
	c = read(mpu_i2c_addr, ACCEL_CONFIG2);
	/* Clear accel_fchoice_b (bit 3) and A_DLPFG (bits [2:0]) */
	c = c & ~0x0F;
	/* Set accel_fchoice_b to 1 */
	c = c | (~(setting.accel_fchoice << 3) & 0x08);
	/* Set accelerometer rate to 1 kHz and bandwidth to 41 Hz */
	c = c | (uint8_t(setting.accel_dlpf_cfg) & 0x07);
	/* Write new ACCEL_CONFIG2 register value */
	write(mpu_i2c_addr, ACCEL_CONFIG2, c);
	/* The accelerometer, gyro, and thermometer are set to 1 kHz sample rates,
	 * but all these rates are further reduced by a factor of 5 to 200 Hz because
	 * of the SMPLRT_DIV setting */

	/* Configure Interrupts and Bypass Enable
	 * Set interrupt pin active high, push-pull, hold interrupt pin level HIGH
	 * until interrupt cleared, clear on read of INT_STATUS, and enable I2C_BYPASS_EN
	 * so additional chips can join the I2C bus and all can be controlled by the LPC as master */
	write(mpu_i2c_addr, INT_PIN_CFG, 0x22);
	/* Enable data ready (bit 0) interrupt */
	write(mpu_i2c_addr, INT_ENABLE, 0x01);
	delay(100);
}

/*!
 * @brief MPU9250 initAK8963 private method.

 * Initializes the AK8963 with given settings.
 *
 * @param None.
 *
 * @retval None.
 */
void MPU9250::initAK8963(void) {

	/* First extract the factory calibration for
	 * each magnetometer axis stored hete*/
	uint8_t raw_data[3];
	/* Power down magnetometer */
	write(AK8963_ADDRESS, AK8963_CNTL, 0x00);
	delay(10);
	/* Enter Fuse ROM access mode */
	write(AK8963_ADDRESS, AK8963_CNTL, 0x0F);
	delay(10);
	/* Read the x-, y-, and z-axis calibration values */
	read(AK8963_ADDRESS, AK8963_ASAX, raw_data, 3);

	for(uint8_t i = 0; i < 3; i++) {
		/* Return x-axis sensitivity adjustment values, etc. */
		mag_bias_factory[i] = (float)(raw_data[i] - 128) / 256. + 1.;
	}
	/* Power down magnetometer */
	write(AK8963_ADDRESS, AK8963_CNTL, 0x00);
	delay(10);
	/* Configure the magnetometer for continuous read and highest resolution
	 * set Mscale bit 4 to 1 (0) to enable 16 (14) bit resolution in CNTL register,
	 * and enable continuous mode data acquisition MAG_MODE (bits [3:0]), 0010 for
	 * 8 Hz and 0110 for 100 Hz sample rates */

	/* Set magnetometer data resolution and sample ODR */
	write(AK8963_ADDRESS, AK8963_CNTL, (uint8_t)setting.mag_output_bits << 4 | MAG_MODE);
	delay(10);
}

/*!
 * @brief MPU9250 read_mag private method.

 * Reads the magnetometer.
 *
 * @param destination pointer where the result will be stored.
 *
 * @retval true if data was read, false if there was an error.
 */
bool MPU9250::read_mag(int16_t *destination) {

	uint8_t st1;
	/* Wait for magnetometer data ready bit to be set */
	do { st1 = read(AK8963_ADDRESS, AK8963_ST1); }
	while(!(st1 & 0x01));

	/* x/y/z magnetometer register data, ST2 register stored here,
	 * must read ST2 at end of data acquisition */
	uint8_t raw_data[7];
	/* Read the six raw data and ST2 registers sequentially into data array */
	read(AK8963_ADDRESS, AK8963_XOUT_L, raw_data, 7);
	/* Continuous or external trigger read mode */
	if (MAG_MODE == 0x02 || MAG_MODE == 0x04 || MAG_MODE == 0x06) {
		/* Check if data is not skipped */
		if ((st1 & 0x02) != 0)
			return false;
	}

	/* End data read by reading ST2 register */
	uint8_t st2 = raw_data[6];
	/* Check if magnetic sensor overflow set, if not then report data */
	if (!(st2 & 0x08)) {
		uint8_t j = 0;
		for(uint8_t i = 0; i < 6; i += 2) {
			/* Turn the MSB and LSB into a signed 16-bit value */
			destination[j] = ((int16_t)raw_data[i + 1] << 8) | raw_data[i];
			j++;
		}
		return true;
	}
	return false;
}

/*!
 * @brief MPU9250 read_temperature_data private method.

 * Reads the temperature registers.
 *
 * @param None.
 *
 * @retval 16-bit temperature raw value.
 */
int16_t MPU9250::read_temperature_data(void) {

	/* High and low temperature register data stored here */
	uint8_t raw_data[2];
	/* Read the two raw data registers sequentially into data array */
	read(mpu_i2c_addr, TEMP_OUT_H, raw_data, 2);
	/* Turn the MSB and LSB into a 16-bit value */
	return ((int16_t)raw_data[0] << 8) | raw_data[1];
}

/*!
 * @brief MPU9250 get_acc_resolution private method.

 * Returns the accelerometer resolution.
 *
 * @param accel_af_sel resolution selected in settings.
 *
 * @retval Possible accelerometer scales (and their register bit settings) are:
 * - 2 Gs (00)
 * - 4 Gs (01)
 * - 8 Gs (10)
 * - 16 Gs (11)
 */
float MPU9250::get_acc_resolution(const ACCEL_FS_SEL accel_af_sel) {

	switch (accel_af_sel) {
		/* Here's a bit of an algorith to calculate
		 * DPS/(ADC tick) based on that 2-bit value: */
		case ACCEL_FS_SEL::A2G:
			return 2.0 / 32768.0;
		case ACCEL_FS_SEL::A4G:
			return 4.0 / 32768.0;
		case ACCEL_FS_SEL::A8G:
			return 8.0 / 32768.0;
		case ACCEL_FS_SEL::A16G:
			return 16.0 / 32768.0;
		default:
			return 0.;
	}
}

/*!
 * @brief MPU9250 get_gyro_resolution private method.

 * Returns the gyroscope resolution.
 *
 * @param gyro_fs_sel resolution selected in settings.
 *
 * @retval Possible gyro scales (and their register bit settings) are:
 * - 250 DPS (00)
 * - 500 DPS (01)
 * - 1000 DPS (10)
 * - 2000 DPS  (11).
 */
float MPU9250::get_gyro_resolution(const GYRO_FS_SEL gyro_fs_sel) {

	switch (gyro_fs_sel) {
		/* Here's a bit of an algorith to calculate
		 * DPS/(ADC tick) based on that 2-bit value: */
		case GYRO_FS_SEL::G250DPS:
			return 250.0 / 32768.0;
		case GYRO_FS_SEL::G500DPS:
			return 500.0 / 32768.0;
		case GYRO_FS_SEL::G1000DPS:
			return 1000.0 / 32768.0;
		case GYRO_FS_SEL::G2000DPS:
			return 2000.0 / 32768.0;
		default:
			return 0.;
	}
}

/*!
 * @brief MPU9250 get_mag_resolution private method.

 * Returns the magnetometer resolution.
 *
 * @param mag_output_bits resolution selected in settings.
 *
 * @retval Possible magnetometer scales (and their register bit settings) are:
 * - 14 bit resolution (0)
 * - 16-bit resolution (1)
 */
float MPU9250::get_mag_resolution(const MAG_OUTPUT_BITS mag_output_bits) {

	switch (mag_output_bits) {
		/* Proper scale to return milliGauss */
		case MAG_OUTPUT_BITS::M14BITS:
			return 10. * 4912. / 8190.0;
		case MAG_OUTPUT_BITS::M16BITS:
			return 10. * 4912. / 32760.0;
		default:
			return 0.;
	}
}

/*!
 * @brief Low level MPU9250 write private method.

 * Writes a single byte to the register indicated.
 *
 * @param address 7-bit address of the device.
 * @param reg address of the register to be written.
 * @param data to be written.
 *
 * @retval None.
 */
void MPU9250::write(uint8_t address, uint8_t reg, uint8_t data) {

	/* Form a two byte array with the arguments */
	uint8_t buf[] = { reg, data };
	/* Write two bytes to said address */
	write(address, buf, 2);
}

/*!
 * @brief Low level MPU9250 write private method.

 * Writes a specified buffer to the device indicated.
 *
 * @param address 7-bit address of the device.
 * @param buf pointer to the data to be sent.
 * @param size number of bytes to be written.
 *
 * @retval None.
 */
void MPU9250::write(uint8_t address, uint8_t *buf, uint8_t size) {

	/* Write Address and RW bit to data register */
	i2c->MSTDAT = ((uint32_t)address << 1) | ((uint32_t)kI2C_Write & 1u);
	/* Start the transfer */
	i2c->MSTCTL = I2C_MSTCTL_MSTSTART_MASK;
	/* Write register and data */
	I2C_MasterWriteBlocking(i2c, buf, size, kI2C_TransferDefaultFlag);
	/* Stop transfer */
	i2c->MSTCTL = I2C_MSTCTL_MSTSTOP_MASK;
}

/*!
 * @brief Low level MPU9250 read private method.

 * Reads a single byte from the register indicated.
 *
 * @param address 7-bit address of the device.
 * @param reg address of the register to be read.
 *
 * @retval data read from the register.
 */
uint8_t MPU9250::read(uint8_t address, uint8_t reg) {

	/* Byte to store the read value */
	uint8_t ret;
	/* Write Address and RW bit to data register */
	i2c->MSTDAT = ((uint32_t)address << 1) | ((uint32_t)kI2C_Write & 1u);
	/* Start the transfer */
	i2c->MSTCTL = I2C_MSTCTL_MSTSTART_MASK;
	/* Access register */
	I2C_MasterWriteBlocking(i2c, &reg, 1, kI2C_TransferDefaultFlag);
	/* Stop transfer */
	i2c->MSTCTL = I2C_MSTCTL_MSTSTOP_MASK;
	/* Write Address and RW bit to data register */
	i2c->MSTDAT = ((uint32_t)address << 1) | ((uint32_t)kI2C_Read & 1u);
	/* Start the transfer */
	i2c->MSTCTL = I2C_MSTCTL_MSTSTART_MASK;
	/* Read from slave */
	I2C_MasterReadBlocking(i2c, &ret, 1, kI2C_TransferDefaultFlag);
	/* Stop transfer */
	i2c->MSTCTL = I2C_MSTCTL_MSTSTOP_MASK;
	/* Return read byte */
	return ret;
}

/*!
 * @brief Low level MPU9250 read private method.

 * Reads a stream of data from the register indicated.
 *
 * @param address 7-bit address of the device.
 * @param reg address of the register where the reading will start.
 * @param buf pointer to the array to store the data read.
 * @param size number of bytes to read.
 *
 * @retval None.
 */
void MPU9250::read(uint8_t address, uint8_t reg, uint8_t *buf, uint8_t size) {

	/* Write Address and RW bit to data register */
	i2c->MSTDAT = ((uint32_t)address << 1) | ((uint32_t)kI2C_Write & 1u);
	/* Start the transfer */
	i2c->MSTCTL = I2C_MSTCTL_MSTSTART_MASK;
	/* Access register */
	I2C_MasterWriteBlocking(i2c, &reg, 1, kI2C_TransferDefaultFlag);
	/* Stop transfer */
	i2c->MSTCTL = I2C_MSTCTL_MSTSTOP_MASK;
	/* Write Address and RW bit to data register */
	i2c->MSTDAT = ((uint32_t)address << 1) | ((uint32_t)kI2C_Read & 1u);
	/* Start the transfer */
	i2c->MSTCTL = I2C_MSTCTL_MSTSTART_MASK;
	/* Read from slave */
	I2C_MasterReadBlocking(i2c, buf, size, kI2C_TransferDefaultFlag);
	/* Stop transfer */
	i2c->MSTCTL = I2C_MSTCTL_MSTSTOP_MASK;
}