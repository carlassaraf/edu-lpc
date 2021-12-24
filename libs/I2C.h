/*
 * I2C.h
 *
 *  Created on: Oct 20, 2021
 *      Author: fabri
 */

#ifndef I2C_H_
#define I2C_H_

/*  I2C includes  */

#include <fsl_i2c.h>
#include <fsl_swm.h>

/*  Class definition  */

class I2C {

	public:

		I2C(uint32_t i2cn);
		I2C(uint32_t i2cn, uint32_t frequency);
		void assignPins(uint32_t sda, uint32_t scl);
		status_t write(uint8_t address, uint8_t *buff, uint32_t size);
		status_t read(uint8_t address, uint8_t *buff, uint32_t size);

	private:

		I2C_Type *i2c;
        uint32_t i2cx;

		void init(uint32_t i2cn, uint32_t frequency);
        I2C_Type* getInstance(uint32_t i2cn);
};

#endif /* I2C_H_ */
