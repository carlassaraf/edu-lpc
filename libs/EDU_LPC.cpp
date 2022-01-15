/*
 * EDU_LPC.cpp
 *
 *  Created on: Jan 11, 2022
 *      Author: fabri
 */

#include <EDU_LPC.h>

uint8_t rxBuffer[5];
uint8_t rxIndex = 0;
uint8_t	rxSize;
bool rxDone = false;

USART serial(1, 115200);

Pin *gpio[] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
ADC *adc[] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
DAC *dac[] = { nullptr, nullptr };

PWM *pwm[] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

LM35 lm(1);
DAC analogOut(1);

void init(void) {

	serial.assignPins(0, 1);
	serial.attachInterrupt(cmd_reception);
	dac[1] = &analogOut;
}

void com_handler(void) {

	if(rxDone) {

		uint8_t checksum = calculate_checksum(rxBuffer, rxIndex - 1);
		uint16_t val;
		uint32_t freq, duty;

		if(checksum == rxBuffer[rxIndex - 1]) {

			switch (rxBuffer[CMD_INDEX]) {

				case kcmd_config:
					cmd_config(rxBuffer[DATA1_INDEX], rxBuffer[DATA2_INDEX]);
					break;

				case kcmd_gpio_clear:
					cmd_gpio_clear(rxBuffer[DATA1_INDEX]);
					break;

				case kcmd_gpio_set:
					cmd_gpio_set(rxBuffer[DATA1_INDEX]);
					break;

				case kcmd_gpio_toggle:
					cmd_gpio_toggle(rxBuffer[DATA1_INDEX]);
					break;

				case kcmd_adc_read:
					cmd_adc_read(rxBuffer[DATA1_INDEX]);
					break;

				case kcmd_lm35_c:
					cmd_lm35(kcmd_lm35_c, lm.getTemperatureCelsius());
					break;

				case kcmd_lm35_f:
					cmd_lm35(kcmd_lm35_f, lm.getTemperatureFahrenheit());
					break;

				case kcmd_dac_set:
					val = ((uint16_t)rxBuffer[DATA2_INDEX] << 2) + rxBuffer[DATA3_INDEX];
					cmd_dac_set(rxBuffer[DATA1_INDEX], val);
					break;

				case kcmd_dac_sine:
					val = _8BIT_TO_16BIT_(rxBuffer[DATA2_INDEX], rxBuffer[DATA3_INDEX]);
					cmd_dac_sine(rxBuffer[DATA1_INDEX], freq);
					break;

				case kcmd_dac_triangular:
					val = _8BIT_TO_16BIT_(rxBuffer[DATA2_INDEX], rxBuffer[DATA3_INDEX]);
					cmd_dac_triangular(rxBuffer[DATA1_INDEX], freq);
					break;

				case kcmd_dac_wave:
					cmd_dac_wave(rxBuffer[DATA1_INDEX], (bool)rxBuffer[DATA2_INDEX]);
					break;

				case kcmd_pwm_config:
					freq = _8BIT_TO_16BIT_(rxBuffer[DATA2_INDEX], rxBuffer[DATA3_INDEX]);
					duty = rxBuffer[DATA4_INDEX];
					cmd_pwm_config(rxBuffer[DATA1_INDEX], freq, duty);
					break;
			}
		}
		rxIndex = 0;
		rxDone = false;
	}
}

void cmd_config(uint8_t pin, uint8_t function) {

	int index = pin - GPIO_START_INDEX;
	int channel = ADC_LAST_CHANNEL - index;

	if(function == kconfig_input || function == kconfig_output) {

		// Free memory from other objects
		delete adc[index];
		delete pwm[index];
		adc[index] = nullptr;
		pwm[index] = nullptr;
		if(pin == DAC0_PIN_INDEX) {
			delete dac[0];
			dac[0] = nullptr;
		}

		// Allocate new memory
		gpio[index] = new Pin(pin, function);
		return;
	}

	if(function == kconfig_adc) {

		// Free memory from other objects
		delete gpio[index];
		delete pwm[index];
		gpio[index] = nullptr;
		pwm[index] = nullptr;
		if(pin == DAC0_PIN_INDEX) {
			delete dac[0];
			dac[0] = nullptr;
		}

		// Allocate new memory
		adc[index] = new ADC(channel);
		return;
	}

	if(function == kconfig_dac) {

		// Free memory from other objects
		delete gpio[index];
		delete adc[index];
		delete pwm[index];
		gpio[index] = nullptr;
		adc[index] = nullptr;
		pwm[index] = nullptr;

		// Allocate new memory
		dac[0] = new DAC(0);
		return;
	}

	if(function == kconfig_pwm) {
		
		// Free memory from other objects
		delete gpio[index];
		delete adc[index];
		gpio[index] = nullptr;
		adc[index] = nullptr;
		if(pin == DAC0_PIN_INDEX) {
			delete dac[0];
			dac[0] = nullptr;
		}
		
		// Allocate new memory
		pwm[index] = new PWM(pin);		
		return;
	}
}

void cmd_gpio_clear(uint8_t pin) {
	gpio[pin - 16]->clear();
}

void cmd_gpio_set(uint8_t pin) {
	gpio[pin - 16]->set();
}

void cmd_gpio_toggle(uint8_t pin) {
	gpio[pin - 16]->toggle();
}

void cmd_adc_read(uint8_t channel) {

	uint32_t value = adc[ADC_LAST_CHANNEL - channel]->read();
	uint8_t low = value & 0xff;
	uint8_t high = value >> 8;

	uint8_t buff[] = {
			kcmd_adc_read,
			high,
			low
	};
	dataHandler(buff, 3);
}

void cmd_lm35(cmd_codes cmd, float temp) {

	uint8_t high = (uint8_t)temp;
	uint8_t low = (temp - high) * 100;

	uint8_t buff[] = {
		cmd,
		high,
		low
	};
	dataHandler(buff, 3);
}

void cmd_dac_set(uint8_t channel, uint32_t value) {

	if(dac[channel] == nullptr) {
		return;
	}

	dac[channel]->set(value);
}

void cmd_dac_sine(uint8_t channel, uint32_t frequency) {

	if(dac[channel] == nullptr) {
		return;
	}

	dac[channel]->sine(frequency);
	dac[channel]->getTimer()->stop();
}

void cmd_dac_triangular(uint8_t channel, uint32_t frequency) {

	if(dac[channel] == nullptr) {
		return;
	}

	dac[channel]->triangular(frequency);
	dac[channel]->getTimer()->stop();
}

void cmd_dac_wave(uint8_t channel, bool enable) {

	if(dac[channel] == nullptr) {
		return;
	}
	if(enable) {
		dac[channel]->getTimer()->reset();
		dac[channel]->getTimer()->start();
	}
	else {
		dac[channel]->getTimer()->stop();
	}
}

void cmd_pwm_config(uint32_t channel, uint32_t frequency, uint32_t duty) {

	if(pwm[channel] == nullptr) {
		return;
	}

	if(pwm[channel]->getFrequency() != frequency) {

		int i = 0;
		while(pwm[i] != nullptr) {
			pwm[i]->setFrequency(frequency);
			i++;
		}
	}

	if(pwm[channel]->getDuty() != duty) { pwm[channel]->setDuty(duty); }
}

uint8_t calculate_checksum(uint8_t *data, uint8_t size) {

	uint16_t checksum = 0;
    // Checksum calculation starts after the length byte
	for(int i = 1; i < size; i++) {

		checksum += data[i];
	}
	checksum &= 0xff;
	return 0xff - checksum;
}

void dataHandler(uint8_t *data, uint8_t size) {

	uint8_t header = size + 1;
	uint8_t buff[header + 1];

	buff[0] = header;
	for(int i = 0; i < size; i++) {
		buff[i + 1] = data[i];
	}
	buff[header] = calculate_checksum(buff, header);

	serial.print(buff);
}

void cmd_reception(void) {

	rxBuffer[rxIndex++] = serial.read();

	// Check if communication is done
	if(rxIndex > rxBuffer[0]) {
		rxDone = true;
	}
}