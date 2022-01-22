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
 * I2C1-3 default pins: PIO0_0 SCL / PIO0_1 SDA.
 *
 * @param i2cn is the I2C module to use (0-3).
 *
 * @retval None.
 */
I2C::I2C(uint32_t i2cn) {
	/* Call initialization */
	init(i2cn, 12000000U);
}

/*!
 * @brief I2C constructor.

 * Creates an I2C object.
 * I2C0 default pins: PIO0_10 SCL / PIO0_11 SDA.
 * I2C1-3 default pins: PIO0_0 SCL / PIO0_1 SDA.
 *
 * @param i2cn is the I2C module to use (0-3).
 * @param frequency the frequency of the I2C communication
 *
 * @retval None.
 */
I2C::I2C(uint32_t i2cn, uint32_t frequency) {
	/* Call initialization */
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
void I2C::assignPins(uint8_t sda, uint8_t scl) {
	/* Update pins in settings */
	settings.swm_pins[SDA_INDEX] = (swm_port_pin_type_t)sda;
	settings.swm_pins[SCL_INDEX] = (swm_port_pin_type_t)scl;
	/* Enable SWM CLK */
	SYSCON->SYSAHBCLKCTRL0 	|= SYSCON_SYSAHBCLKCTRL0_SWM_MASK;
	/* Change I2C pins */
	SWM_SetMovablePinSelect(SWM0, settings.swm_function[SDA_INDEX], settings.swm_pins[SDA_INDEX]);
	SWM_SetMovablePinSelect(SWM0, settings.swm_function[SCL_INDEX], settings.swm_pins[SCL_INDEX]);
	/* Disable SWM CLK */
	SYSCON->SYSAHBCLKCTRL0	&= ~(SYSCON_SYSAHBCLKCTRL0_SWM_MASK);
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
void I2C::write(uint8_t address, uint8_t *buff, uint8_t size) {
	/* Write Address and RW bit to data register */
	i2c_base->MSTDAT = ((uint32_t)address << 1) | ((uint32_t)kI2C_Write & 1u);
	/* Start the transfer */
	i2c_base->MSTCTL = I2C_MSTCTL_MSTSTART_MASK;
	/* Write register and data */
	I2C_MasterWriteBlocking(i2c_base, buff, size, kI2C_TransferDefaultFlag);
	/* Stop transfer */
	i2c_base->MSTCTL = I2C_MSTCTL_MSTSTOP_MASK;
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
void I2C::read(uint8_t address, uint8_t *buff, uint8_t size) {
	/* Write Address and RW bit to data register */
	i2c_base->MSTDAT = ((uint32_t)address << 1) | ((uint32_t)kI2C_Read & 1u);
	/* Start the transfer */
	i2c_base->MSTCTL = I2C_MSTCTL_MSTSTART_MASK;
	/* Read from slave */
	I2C_MasterReadBlocking(i2c_base, buff, size, kI2C_TransferDefaultFlag);
	/* Stop transfer */
	i2c_base->MSTCTL = I2C_MSTCTL_MSTSTOP_MASK;
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
void I2C::init(uint8_t i2cn, uint32_t frequency) {
	/* Store the index of the I2C */
	i2c_index = i2c;
	/* Calculate the I2C peripherial address */
	i2c_base = (i2c < 2)? (I2C_Type*)(0x40050000 + i2c * 0x4000) : (I2C_Type*)(0x40030000 * (i2c - 2) * 0x4000);
	/* Select clock from the FRO */
	CLOCK_Select((clock_select_t)CLK_MUX_DEFINE(FCLKSEL[5U + i2c], 0U));
	/* Update swm functions according to the I2C peripherial selected*/
	settings.swm_function[SDA_INDEX] = (swm_select_movable_t)(kSWM_I2C1_SDA + 2 * (i2c_index - 1));
	settings.swm_function[SCL_INDEX] = (swm_select_movable_t)(kSWM_I2C1_SCL + 2 * (i2c_index - 1));
	/* For I2C0 enable the set pin function */
	if (i2c_index == 0) {
		/* Enable clock for SWM */
	    CLOCK_EnableClock(kCLOCK_Swm);
	    /* SDA on PIO0_11 */
	    SWM_SetFixedPinSelect(SWM0, kSWM_I2C0_SDA, true);
	    /* SCL on PIO0_10 */
	    SWM_SetFixedPinSelect(SWM0, kSWM_I2C0_SCL, true);
	    /* Disable clock for SWM */
	    CLOCK_DisableClock(kCLOCK_Swm);
	}
	/* For I2C2-3 assign pins PIO0_1 and PIO0_0 */
	else if (i2c_index > 0) {
		/* SDA in PIO0_1 and SCL in PIO0_0 */
		assignPins(1, 0);
	}
	/* Update baud rate in settings */
	settings.config.baudRate_Bps = frequency;
	/* Initialize I2C peripherial */
	I2C_MasterInit(i2c_base, &settings.config, 12000000U);
}