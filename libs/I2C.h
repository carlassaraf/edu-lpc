/*
 * I2C.h
 *
 *  Created on: Oct 20, 2021
 *      Author: fabri
 */

#ifndef I2C_H_
#define I2C_H_

/* I2C includes */
#include <fsl_i2c.h>
#include <fsl_swm.h>

/* I2C configuration struct */
typedef struct {
	i2c_master_config_t config = {
		true,		/* Master mode */
		100000U,	/* 100KHz SCL */
		false		/* No timeout enabled */
	};
	swm_select_movable_t swm_function[2];
	swm_port_pin_type_t swm_pins[2] = {
		kSWM_PortPin_P0_1,
		kSWM_PortPin_P0_0
	};
} i2c_config_t;

/* I2C constants */
static constexpr uint8_t SDA_INDEX = 0;
static constexpr uint8_t SCL_INDEX = 1;

/* Class definition */
class I2C {

	public:
		/* Constructors */
		I2C(uint8_t i2c);
		I2C(uint8_t i2c, uint32_t frequency);
		/* Public methods */
		void assignPins(uint8_t sda, uint8_t scl);
		void write(uint8_t address, uint8_t *buff, uint8_t size);
		void read(uint8_t address, uint8_t *buff, uint8_t size);
		I2C_Type* getI2C(void)

	private:
		/* I2C peripherial pointer */
		I2C_Type *i2c_base;
		/* I2C index */
        uint32_t i2c_index;
		/* I2C settings */
		i2c_config_t settings;
		/* Private methods */
		void init(uint8_t i2cn, uint32_t frequency);
};

/*!
 * @brief I2C getI2C method.

 * Returns the I2C register associated with the instance.
 *
 * @param None.
 *
 * @retval I2C peripherial pointer.
 */
inline I2C_Type* I2C::getI2C(void) {
	return i2c_base;
}

#endif /* I2C_H_ */
