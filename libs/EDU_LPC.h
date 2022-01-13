/*
 * EDU_LPC.h
 *
 *  Created on: Jan 11, 2022
 *      Author: fabri
 */

#ifndef EDU_LPC_H_
#define EDU_LPC_H_

#include <board.h>

#include "GPIO.h"
#include "ADC.h"
#include "USART.h"
#include "DAC.h"
#include "LM35.h"

#define SIZE_INDEX		0
#define CMD_INDEX		1
#define DATA1_INDEX		2
#define DATA2_INDEX		3
#define DATA3_INDEX		4

#define GPIO_START_INDEX	16
#define ADC_LAST_CHANNEL	10
#define DAC0_PIN_INDEX		17

#define _8BIT_TO_16BIT_(high, low)	((uint16_t)high << 8) + low

typedef enum {
	kcmd_config = 0x01,
	kcmd_gpio_clear = 0x02,
	kcmd_gpio_set = 0x03,
	kcmd_gpio_toggle = 0x04,
	kcmd_gpio_sstatus = 0x05,
	kcmd_adc_read = 0x06,
	kcmd_lm35_c = 0x07,
	kcmd_lm35_f = 0x08,
	kcmd_dac_set = 0x09,
	kcmd_dac_sine = 0x0a,
	kcmd_dac_triangular = 0x0b,
	kcmd_dac_wave = 0x0c
} cmd_codes;

typedef enum {
	kconfig_input = 0x00,
	kconfig_output = 0x01,
	kconfig_adc = 0x02,
	kconfig_dac = 0x03
} config_codes;

/*  Function prototypes  */

// EDU LPC initialization function
void init(void);

// Communication functions
void comHandler(void);
void cmd_reception(void);
void cmd_config(uint8_t pin, uint8_t function);
void cmd_gpio_clear(uint8_t pin);
void cmd_gpio_set(uint8_t pin);
void cmd_gpio_toggle(uint8_t pin);
void cmd_adc_read(uint8_t channel);
void cmd_lm35(cmd_codes cmd, float temp);
void cmd_dac_sine(uint8_t channel, uint32_t frequency);
void cmd_dac_triangular(uint8_t channel, uint32_t frequency);
void cmd_dac_wave(uint8_t channel, bool enable);
uint8_t calculate_checksum(uint8_t *data, uint8_t size);
void dataHandler(uint8_t *data, uint8_t size);


#endif /* EDU_LPC_H_ */