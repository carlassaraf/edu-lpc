/*
 * I2C.cpp
 *
 *  Created on: Oct 20, 2021
 *      Author: fabri
 */

#include <I2C.h>

I2C::I2C(uint32_t i2cn) {

	init(i2cn, 12000000U);
}

I2C::I2C(uint32_t i2cn, uint32_t frequency) {

	init(i2cn, frequency);
}

I2C::I2C(uint32_t i2cn, uint32_t sda, uint32_t scl) {

	init(i2cn, 12000000U);
	swm_select_movable_t swm_sda, swm_scl;

	if (i2cn == 1) {
		swm_sda = kSWM_I2C1_SDA;
		swm_scl = kSWM_I2C1_SCL;
	}
	else if (i2cn == 2) {
		swm_sda = kSWM_I2C2_SDA;
		swm_scl = kSWM_I2C2_SCL;
	}
	else if (i2cn == 3) {
		swm_sda = kSWM_I2C3_SDA;
		swm_scl = kSWM_I2C3_SCL;
	}

	if (i2cn > 0) {
		SYSCON->SYSAHBCLKCTRL0 	|= SYSCON_SYSAHBCLKCTRL0_SWM_MASK;		// Enable SWM CLK
		SWM_SetMovablePinSelect(SWM0, swm_sda, (swm_port_pin_type_t)sda);
		SWM_SetMovablePinSelect(SWM0, swm_scl, (swm_port_pin_type_t)scl);
		SYSCON->SYSAHBCLKCTRL0	&= ~(SYSCON_SYSAHBCLKCTRL0_SWM_MASK);	// Disable SWM CLK
	}
}

void I2C::init(uint32_t i2cn, uint32_t frequency) {

	/*
	 * I2C0 default pins: PIO0_10 SCL / PIO0_11 SDA
	 * I2C1-3 default pins: PIO0_1 SCL / PIO0_0 SDA
	 *
	 * */

	swm_select_movable_t swm_sda, swm_scl;

	switch (i2cn) {

		case 0:
			i2c = I2C0;
			CLOCK_Select(kI2C0_Clk_From_MainClk);
		    CLOCK_EnableClock(kCLOCK_Swm);
		    SWM_SetFixedPinSelect(SWM0, kSWM_I2C0_SDA, true);		// PIO0_11
		    SWM_SetFixedPinSelect(SWM0, kSWM_I2C0_SCL, true);		// PIO0_10
		    CLOCK_DisableClock(kCLOCK_Swm);
			break;

		case 1:
			i2c = I2C1;
			CLOCK_Select(kI2C1_Clk_From_MainClk);
			swm_sda = kSWM_I2C1_SDA;
			swm_scl = kSWM_I2C1_SCL;
			break;

		case 2:
			i2c = I2C2;
			CLOCK_Select(kI2C2_Clk_From_MainClk);
			swm_sda = kSWM_I2C2_SDA;
			swm_scl = kSWM_I2C2_SCL;
			break;

		case 3:
			i2c = I2C3;
			CLOCK_Select(kI2C3_Clk_From_MainClk);
			swm_sda = kSWM_I2C3_SDA;
			swm_scl = kSWM_I2C3_SCL;
			break;
	}

	if (i2cn > 0) {
		SYSCON->SYSAHBCLKCTRL0 	|= SYSCON_SYSAHBCLKCTRL0_SWM_MASK;		// Enable SWM CLK
		SWM_SetMovablePinSelect(SWM0, swm_sda, kSWM_PortPin_P0_0);
		SWM_SetMovablePinSelect(SWM0, swm_scl, kSWM_PortPin_P0_1);
		SYSCON->SYSAHBCLKCTRL0	&= ~(SYSCON_SYSAHBCLKCTRL0_SWM_MASK);	// Disable SWM CLK
	}

	i2c_master_config_t masterConfig;
	I2C_MasterGetDefaultConfig(&masterConfig);
	masterConfig.baudRate_Bps = 400000;
	I2C_MasterInit(i2c, &masterConfig, frequency);
}


status_t I2C::write(uint8_t address, uint8_t *buff, uint32_t size) {

	I2C_MasterStart(i2c, address, kI2C_Write);
	I2C_MasterWriteBlocking(i2c, buff, size, kI2C_TransferDefaultFlag);
	return I2C_MasterStop(i2c);
}


status_t I2C::read(uint8_t address, uint8_t *buff, uint32_t size) {

	I2C_MasterStart(i2c, address, kI2C_Read);
	I2C_MasterReadBlocking(i2c, buff, size, kI2C_TransferDefaultFlag);
	return I2C_MasterStop(i2c);
}