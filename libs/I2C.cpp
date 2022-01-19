/*
 * I2C.cpp
 *
 *  Created on: Oct 20, 2021
 *      Author: fabri
 */

#include <I2C.h>

/*!
 * @brief I2C constructor.

 * Creates an I2C object. Default frequency is 12MHz.
 * I2C0 default pins: PIO0_10 SCL / PIO0_11 SDA.
 * I2C1-3 default pins: PIO0_1 SCL / PIO0_0 SDA.
 *
 * @param i2cn is the I2C module to use (0-3).
 *
 * @retval None.
 */
I2C::I2C(uint32_t i2cn) {

	init(i2cn, 12000000U);
}

/*!
 * @brief I2C constructor.

 * Creates an I2C object.
 * I2C0 default pins: PIO0_10 SCL / PIO0_11 SDA.
 * I2C1-3 default pins: PIO0_1 SCL / PIO0_0 SDA.
 *
 * @param i2cn is the I2C module to use (0-3).
 * @param frequency the frequency of the I2C communication
 *
 * @retval None.
 */
I2C::I2C(uint32_t i2cn, uint32_t frequency) {

	init(i2cn, frequency);
}

/*!
 * @brief I2C assignPins method.

 * Assigns the pins for the I2C communication. Only
 * works for I2C1-3
 *
 * @param sda index of the pin to use as SDA.
 * @param scl index of the pin to use as SCL.
 *
 * @retval None.
 */
void I2C::assignPins(uint32_t sda, uint32_t scl) {

	swm_select_movable_t swm_sda, swm_scl;

	if (i2cx == 1) {
		swm_sda = kSWM_I2C1_SDA;
		swm_scl = kSWM_I2C1_SCL;
	}
	else if (i2cx == 2) {
		swm_sda = kSWM_I2C2_SDA;
		swm_scl = kSWM_I2C2_SCL;
	}
	else if (i2cx == 3) {
		swm_sda = kSWM_I2C3_SDA;
		swm_scl = kSWM_I2C3_SCL;
	}

	SYSCON->SYSAHBCLKCTRL0 	|= SYSCON_SYSAHBCLKCTRL0_SWM_MASK;		// Enable SWM CLK
	SWM_SetMovablePinSelect(SWM0, swm_sda, (swm_port_pin_type_t)sda);
	SWM_SetMovablePinSelect(SWM0, swm_scl, (swm_port_pin_type_t)scl);
	SYSCON->SYSAHBCLKCTRL0	&= ~(SYSCON_SYSAHBCLKCTRL0_SWM_MASK);	// Disable SWM CLK
}

/*!
 * @brief I2C write method.

 * Writes a number of bytes to the slave addressed.
 *
 * @param address 7-bit address of the slave device.
 * @param buff array with the bytes to be sent.
 * @param size number of bytes to be sent.
 *
 * @retval status of the communication.
 */
status_t I2C::write(uint8_t address, uint8_t *buff, uint32_t size) {

	I2C_MasterStart(i2c, address, kI2C_Write);
	I2C_MasterWriteBlocking(i2c, buff, size, kI2C_TransferDefaultFlag);
	return I2C_MasterStop(i2c);
}

/*!
 * @brief I2C read method.

 * Reads a number of bytes from the slave addressed.
 *
 * @param address 7-bit address of the slave device.
 * @param buff array to store the data.
 * @param size number of bytes to be read.
 *
 * @retval status of the communication.
 */
status_t I2C::read(uint8_t address, uint8_t *buff, uint32_t size) {

	I2C_MasterStart(i2c, address, kI2C_Read);
	I2C_MasterReadBlocking(i2c, buff, size, kI2C_TransferDefaultFlag);
	return I2C_MasterStop(i2c);
}

/*!
 * @brief I2C getI2C method.

 * Returns the I2C register associated with the instance.
 *
 * @param None.
 *
 * @retval I2C peripherial pointer.
 */
I2C_Type* I2C::getI2C(void) {
	return i2c;
}

/*!
 * @brief I2C private init method.

 * Deals with the initialization of the I2C.
 *
 * @param i2cn index of the I2C module to be used.
 * @param frequency the frequency of the communication.
 *
 * @retval None.
 */
void I2C::init(uint32_t i2cn, uint32_t frequency) {

	i2cx = i2cn;
	i2c = getInstance(i2cn);

	if (i2cx == 0) {
	    CLOCK_EnableClock(kCLOCK_Swm);
	    SWM_SetFixedPinSelect(SWM0, kSWM_I2C0_SDA, true);		// PIO0_11
	    SWM_SetFixedPinSelect(SWM0, kSWM_I2C0_SCL, true);		// PIO0_10
	    CLOCK_DisableClock(kCLOCK_Swm);
	}

	else if (i2cx > 0) {
		assignPins(1, 0);
	}

	i2c_master_config_t masterConfig;
	I2C_MasterGetDefaultConfig(&masterConfig);
	masterConfig.baudRate_Bps = 400000;
	I2C_MasterInit(i2c, &masterConfig, frequency);
}

/*!
 * @brief I2C private getInstance method.

 * Gets the I2C module to be used and selects the clock.
 *
 * @param i2cn index of the I2C module to be used.
 *
 * @retval I2C module.
 */
I2C_Type* I2C::getInstance(uint32_t i2cn) {

	if(i2cn == 0) {
		CLOCK_Select(kI2C0_Clk_From_MainClk);
		return I2C0;
	}

	if(i2cn == 1) {
		CLOCK_Select(kI2C1_Clk_From_MainClk);
		return I2C1;
	}

	if(i2cn == 2) {
		CLOCK_Select(kI2C2_Clk_From_MainClk);
		return I2C2;
	}

	if(i2cn == 3) {
		CLOCK_Select(kI2C3_Clk_From_MainClk);
		return I2C3;
	}

	return I2C0;
}