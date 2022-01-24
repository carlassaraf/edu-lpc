/*
 * MPU9250.h
 *
 *  Created on: Jan 18, 2022
 *      Author: fabri
 *      Based on Arduino MPU9250 library v0.4.8
 */

#ifndef MPU9250_H
#define MPU9250_H

/* Includes */
#include "I2C.h"
#include "delay.h"

/* Register definitions */

/* Magnetometer registers */
/* Should return 0x48 */
#define WHO_AM_I_AK8963  0x00
#define INFO             0x01
/* Data ready status bit 0 */
#define AK8963_ST1       0x02
/* Data */
#define AK8963_XOUT_L    0x03
#define AK8963_XOUT_H    0x04
#define AK8963_YOUT_L    0x05
#define AK8963_YOUT_H    0x06
#define AK8963_ZOUT_L    0x07
#define AK8963_ZOUT_H    0x08
/* Data overflow bit 3 and data read error status bit 2 */
#define AK8963_ST2       0x09
/* Power down (0000),
 * single-measurement (0001),
 * self-test (1000)
 * and Fuse ROM (1111) modes on bits 3:0 */
#define AK8963_CNTL      0x0A
/* Self test control */
#define AK8963_ASTC      0x0C
/* I2C disable */
#define AK8963_I2CDIS    0x0F
/* Fuse ROM x-axis sensitivity adjustment value */
#define AK8963_ASAX      0x10
/* Fuse ROM y-axis sensitivity adjustment value */
#define AK8963_ASAY      0x11
/* Fuse ROM z-axis sensitivity adjustment value */
#define AK8963_ASAZ      0x12

/* Accel Gyro registers */
/* Self test control */
#define SELF_TEST_X_GYRO 0x00
#define SELF_TEST_Y_GYRO 0x01
#define SELF_TEST_Z_GYRO 0x02
#define SELF_TEST_X_ACCEL 0x0D
#define SELF_TEST_Y_ACCEL 0x0E
#define SELF_TEST_Z_ACCEL 0x0F
#define SELF_TEST_A       0x10
/* User-defined trim values for gyro */
#define XG_OFFSET_H       0x13
#define XG_OFFSET_L       0x14
#define YG_OFFSET_H       0x15
#define YG_OFFSET_L       0x16
#define ZG_OFFSET_H       0x17
#define ZG_OFFSET_L       0x18
/* Configuration registers */
#define SMPLRT_DIV        0x19
#define MPU_CONFIG        0x1A
#define GYRO_CONFIG       0x1B
#define ACCEL_CONFIG      0x1C
#define ACCEL_CONFIG2     0x1D
#define LP_ACCEL_ODR      0x1E
#define WOM_THR           0x1F
#define MOT_DUR           0x20
#define ZMOT_THR          0x21
#define ZRMOT_DUR         0x22
#define FIFO_EN            0x23
#define I2C_MST_CTRL       0x24
#define I2C_SLV0_ADDR      0x25
#define I2C_SLV0_REG       0x26
#define I2C_SLV0_CTRL      0x27
#define I2C_SLV1_ADDR      0x28
#define I2C_SLV1_REG       0x29
#define I2C_SLV1_CTRL      0x2A
#define I2C_SLV2_ADDR      0x2B
#define I2C_SLV2_REG       0x2C
#define I2C_SLV2_CTRL      0x2D
#define I2C_SLV3_ADDR      0x2E
#define I2C_SLV3_REG       0x2F
#define I2C_SLV3_CTRL      0x30
#define I2C_SLV4_ADDR      0x31
#define I2C_SLV4_REG       0x32
#define I2C_SLV4_DO        0x33
#define I2C_SLV4_CTRL      0x34
#define I2C_SLV4_DI        0x35
#define I2C_MST_STATUS     0x36
#define INT_PIN_CFG        0x37
#define INT_ENABLE         0x38
#define DMP_INT_STATUS     0x39
#define INT_STATUS         0x3A
/* Accel, Temperature and Gyro data */
#define ACCEL_XOUT_H       0x3B
#define ACCEL_XOUT_L       0x3C
#define ACCEL_YOUT_H       0x3D
#define ACCEL_YOUT_L       0x3E
#define ACCEL_ZOUT_H       0x3F
#define ACCEL_ZOUT_L       0x40
#define TEMP_OUT_H         0x41
#define TEMP_OUT_L         0x42
#define GYRO_XOUT_H        0x43
#define GYRO_XOUT_L        0x44
#define GYRO_YOUT_H        0x45
#define GYRO_YOUT_L        0x46
#define GYRO_ZOUT_H        0x47
#define GYRO_ZOUT_L        0x48
/* More configuration registers */
#define EXT_SENS_DATA_00   0x49
#define EXT_SENS_DATA_01   0x4A
#define EXT_SENS_DATA_02   0x4B
#define EXT_SENS_DATA_03   0x4C
#define EXT_SENS_DATA_04   0x4D
#define EXT_SENS_DATA_05   0x4E
#define EXT_SENS_DATA_06   0x4F
#define EXT_SENS_DATA_07   0x50
#define EXT_SENS_DATA_08   0x51
#define EXT_SENS_DATA_09   0x52
#define EXT_SENS_DATA_10   0x53
#define EXT_SENS_DATA_11   0x54
#define EXT_SENS_DATA_12   0x55
#define EXT_SENS_DATA_13   0x56
#define EXT_SENS_DATA_14   0x57
#define EXT_SENS_DATA_15   0x58
#define EXT_SENS_DATA_16   0x59
#define EXT_SENS_DATA_17   0x5A
#define EXT_SENS_DATA_18   0x5B
#define EXT_SENS_DATA_19   0x5C
#define EXT_SENS_DATA_20   0x5D
#define EXT_SENS_DATA_21   0x5E
#define EXT_SENS_DATA_22   0x5F
#define EXT_SENS_DATA_23   0x60
#define MOT_DETECT_STATUS  0x61
#define I2C_SLV0_DO        0x63
#define I2C_SLV1_DO        0x64
#define I2C_SLV2_DO        0x65
#define I2C_SLV3_DO        0x66
#define I2C_MST_DELAY_CTRL 0x67
#define SIGNAL_PATH_RESET  0x68
#define MOT_DETECT_CTRL    0x69
#define USER_CTRL          0x6A
#define PWR_MGMT_1         0x6B
#define PWR_MGMT_2         0x6C
#define DMP_BANK           0x6D
#define DMP_RW_PNT         0x6E
#define DMP_REG            0x6F
#define DMP_REG_1          0x70
#define DMP_REG_2          0x71
#define FIFO_COUNTH        0x72
#define FIFO_COUNTL        0x73
#define FIFO_R_W           0x74
/* Should return 0x73*/
#define WHO_AM_I_MPU9250   0x75
/* User-defined trim values for accel */
#define XA_OFFSET_H        0x77
#define XA_OFFSET_L        0x78
#define YA_OFFSET_H        0x7A
#define YA_OFFSET_L        0x7B
#define ZA_OFFSET_H        0x7D
#define ZA_OFFSET_L        0x7E

/* Enum setting classes */
enum class ACCEL_FS_SEL {
    A2G,
    A4G,
    A8G,
    A16G
};
enum class GYRO_FS_SEL {
    G250DPS,
    G500DPS,
    G1000DPS,
    G2000DPS
};
enum class MAG_OUTPUT_BITS {
    M14BITS,
    M16BITS
};

enum class FIFO_SAMPLE_RATE : uint8_t {
    SMPL_1000HZ,
    SMPL_500HZ,
    SMPL_333HZ,
    SMPL_250HZ,
    SMPL_200HZ,
    SMPL_167HZ,
    SMPL_143HZ,
    SMPL_125HZ,
};

enum class GYRO_DLPF_CFG : uint8_t {
    DLPF_250HZ,
    DLPF_184HZ,
    DLPF_92HZ,
    DLPF_41HZ,
    DLPF_20HZ,
    DLPF_10HZ,
    DLPF_5HZ,
    DLPF_3600HZ,
};

enum class ACCEL_DLPF_CFG : uint8_t {
    DLPF_218HZ_0,
    DLPF_218HZ_1,
    DLPF_99HZ,
    DLPF_45HZ,
    DLPF_21HZ,
    DLPF_10HZ,
    DLPF_5HZ,
    DLPF_420HZ,
};

/* MPU default values to compare */
static constexpr uint8_t MPU9250_WHOAMI_DEFAULT_VALUE {0x71};
static constexpr uint8_t MPU9255_WHOAMI_DEFAULT_VALUE {0x73};
static constexpr uint8_t MPU6500_WHOAMI_DEFAULT_VALUE {0x70};

/* MPU setting struct */
struct MPU9250Setting {
    ACCEL_FS_SEL accel_fs_sel {ACCEL_FS_SEL::A16G};
    GYRO_FS_SEL gyro_fs_sel {GYRO_FS_SEL::G2000DPS};
    MAG_OUTPUT_BITS mag_output_bits {MAG_OUTPUT_BITS::M16BITS};
    FIFO_SAMPLE_RATE fifo_sample_rate {FIFO_SAMPLE_RATE::SMPL_200HZ};
    uint8_t gyro_fchoice {0x03};
    GYRO_DLPF_CFG gyro_dlpf_cfg {GYRO_DLPF_CFG::DLPF_41HZ};
    uint8_t accel_fchoice {0x01};
    ACCEL_DLPF_CFG accel_dlpf_cfg {ACCEL_DLPF_CFG::DLPF_45HZ};
};

/* MPU data struct */
typedef struct {
	float acc[3] = { 0., 0., 0. };
	float gy[3] = { 0., 0., 0. };
	float mg[3] = {0., 0., 0. };
	float temp = 0.;
} mpuData_t;

/* Class definition */
class MPU9250 {

	public:
		/* Class constructor */
		MPU9250(uint8_t address = MPU9250_DEFAULT_ADDRESS,
				uint32_t i2cn = 1,
				uint32_t frequency = 400000U,
				const MPU9250Setting mpu_setting = MPU9250Setting());

		/* Updating MPU data*/
		bool update(void);
		/* Get methods to output data to main program */
		/* Accelerometer data */
		float getAccX(void);
		float getAccY(void);
		float getAccZ(void);
		/* Gyroscope data */
		float getGyroX(void);
		float getGyroY(void);
		float getGyroZ(void);
		/* Magnetometer data */
		float getMagX(void);
		float getMagY(void);
		float getMagZ(void);
		/* Temperature data */
		float getTemperature(void);

	private:

		/* I2C peripherial associated */
		I2C_Type *i2c;
		/* LSB/degrees/sec */
		static constexpr uint16_t CALIB_GYRO_SENSITIVITY {131};
		/* LSB/g */
		static constexpr uint16_t CALIB_ACCEL_SENSITIVITY {16384};
		/* Device address when ADO = 0 */
		static constexpr uint8_t MPU9250_DEFAULT_ADDRESS {0x68};
		/* Address of magnetometer */
		static constexpr uint8_t AK8963_ADDRESS {0x0C};
		static constexpr uint8_t AK8963_WHOAMI_DEFAULT_VALUE {0x48};
		/* MPU address */
		uint8_t mpu_i2c_addr {MPU9250_DEFAULT_ADDRESS};
		/* MPU connected flag*/
		bool has_connected {false};
		/* MPU settings */
		MPU9250Setting setting;
		/* 0x02 for 8 Hz, 0x06 for 100 Hz continuous magnetometer data read */
		static constexpr uint8_t MAG_MODE {0x06};
		/* Scale resolutions per LSB for the sensors */
		float acc_resolution {0.f};
		float gyro_resolution {0.f};
		float mag_resolution {0.f};
		/* Calibration Parameters */
		float acc_bias[3] {0., 0., 0.};
		float gyro_bias[3] {0., 0., 0.};
		float mag_bias_factory[3] {0., 0., 0.};
		float mag_bias[3] {0., 0., 0.};
		float mag_scale[3] {1., 1., 1.};
		/* MPU data for main program */
		mpuData_t mpuData;

		/* Methods */
		/* Conection related */
		bool isConnectedMPU9250(void);
		bool isConnectedAK8963(void);
		bool available(void);
		/* Calibration */
		void calibrateAccelGyro(void);
		void calibrateMag(void);
		/* Initialization */
		void initMPU9250(void);
		void initAK8963(void);
		/* Reading */
		bool read_mag(int16_t *destination);
		int16_t read_temperature_data(void);
		/* Resolution related */
		float get_acc_resolution(const ACCEL_FS_SEL accel_af_sel);
		float get_gyro_resolution(const GYRO_FS_SEL gyro_fs_sel);
		float get_mag_resolution(const MAG_OUTPUT_BITS mag_output_bits);
		/* Low level I2C communication */
		void write(uint8_t address, uint8_t reg, uint8_t data);
		void write(uint8_t address, uint8_t *buf, uint8_t size);
		uint8_t read(uint8_t address, uint8_t reg);
		void read(uint8_t address, uint8_t reg, uint8_t *buf, uint8_t size);
};

/* Inline class methods */

/*!
 * @brief MPU9250 getAccX method.

 * Gets the last stored acceleration value of X axis.
 *
 * @param None.
 *
 * @retval X axis acceleration value.
 */
inline float MPU9250::getAccX(void) { return mpuData.acc[0]; }

/*!
 * @brief MPU9250 getAccY method.

 * Gets the last stored acceleration value of Y axis.
 *
 * @param None.
 *
 * @retval Y axis acceleration value.
 */
inline float MPU9250::getAccY(void) { return mpuData.acc[1]; }

/*!
 * @brief MPU9250 getAccZ method.

 * Gets the last stored acceleration value of Z axis.
 *
 * @param None.
 *
 * @retval Z axis acceleration value.
 */
inline float MPU9250::getAccZ(void) { return mpuData.acc[2]; }

/*!
 * @brief MPU9250 getGyroX method.

 * Gets the last stored gyroscope value of X axis.
 *
 * @param None.
 *
 * @retval X axis gyroscope value.
 */
inline float MPU9250::getGyroX(void) { return mpuData.gy[0]; }

/*!
 * @brief MPU9250 getGyroY method.

 * Gets the last stored gyroscope value of Y axis.
 *
 * @param None.
 *
 * @retval Y axis gyroscope value.
 */
inline float MPU9250::getGyroY(void) { return mpuData.gy[1]; }

/*!
 * @brief MPU9250 getGyroZ method.

 * Gets the last stored gyroscope value of Z axis.
 *
 * @param None.
 *
 * @retval Z axis gyroscope value.
 */
inline float MPU9250::getGyroZ(void) { return mpuData.gy[2]; }

/*!
 * @brief MPU9250 getMagX method.

 * Gets the last stored magnetometer value of X axis.
 *
 * @param None.
 *
 * @retval X axis magnetometer value.
 */
inline float MPU9250::getMagX(void) { return mpuData.mg[0]; }

/*!
 * @brief MPU9250 getMagY method.

 * Gets the last stored magnetometer value of Y axis.
 *
 * @param None.
 *
 * @retval Y axis magnetometer value.
 */
inline float MPU9250::getMagY(void) { return mpuData.mg[1]; }

/*!
 * @brief MPU9250 getMagZ method.

 * Gets the last stored magnetometer value of Z axis.
 *
 * @param None.
 *
 * @retval Z axis magnetometer value.
 */
inline float MPU9250::getMagZ(void) { return mpuData.mg[2]; }

/*!
 * @brief MPU9250 getTemperature method.

 * Gets the last stored temperature value.
 *
 * @param None.
 *
 * @retval temperature value.
 */
inline float MPU9250::getTemperature(void) { return mpuData.temp; }

/*!
 * @brief MPU9250 isConnectedMPU9250 private method.

 * Checks whether the MPU9250 is connected or not.
 *
 * @param None.
 *
 * @retval true if it's connected, false if it's not.
 */

#endif  /* MPU9250_H */