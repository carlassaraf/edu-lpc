/*
 * EDU_LPC.cpp
 *
 *  Created on: Jan 11, 2022
 *      Author: fabri
 */

#include <EDU_LPC.h>

/* Initialization of static variables */
uint8_t EDU_LPC::rxBuffer[8] = {0};
uint8_t EDU_LPC::rxIndex = 0;
uint8_t	EDU_LPC::rxSize = 0;
bool EDU_LPC::rxDone = false;

/*!
 * @brief EDU LPC constructor.

 * Creates a EDU LPC object and does the calibration
 * and handles the initialization of all the peripherials.
 *
 * @param None.
 *
 * @retval None.
 */
EDU_LPC::EDU_LPC(void) {
	/* Initialize USART instance */
	USART *serial = new USART(1);
	/* Attach the RX interrupt */
	serial->attachInterrupt(serialRx);
	/* Free memory */
	delete serial;
}

/*!
 * @brief EDU LPC handler method.

 * Checks if a command was received and calls
 * the proper method to execute it.
 *
 * @param None.
 *
 * @retval None.
 */
void EDU_LPC::handler(void) {
	/* Check if the communication is done */
	if(rxDone) {
		/* Auxiliary variables */
		uint16_t val;
		uint32_t freq, duty;
		/* Calculate checksum */
		uint8_t checksum = calculateChecksum(rxBuffer, rxIndex);
		/* Verify if data is valid*/
		if(checksum == rxBuffer[rxIndex]) {
			/* Check command received */
			switch ((EDU_LPC_CMD)rxBuffer[CMD_INDEX]) {
				/* GPIO config command */
				case EDU_LPC_CMD::config:
					/* Configure pin according to function */
					cmdConfig(rxBuffer[DATA1_INDEX], (EDU_LPC_CONFIG)rxBuffer[DATA2_INDEX]);
					break;
				/* GPIO clear command */
				case EDU_LPC_CMD::gpio_clear:
					/* Clear selected pin */
					cmdGpioClr(rxBuffer[DATA1_INDEX]);
					break;
				/* GPIO set command */
				case EDU_LPC_CMD::gpio_set:
					/* Set selected pin */
					cmdGpioSet(rxBuffer[DATA1_INDEX]);
					break;
				/* GPIO toggle command */
				case EDU_LPC_CMD::gpio_toggle:
					/* Toggle selected pin */
					cmdGpioToggle(rxBuffer[DATA1_INDEX]);
					break;
				/* GPIO status command */
				case EDU_LPC_CMD::gpio_read:
					/* Read pin state and send data */
					cmdGpioRead(rxBuffer[DATA1_INDEX]);
					break;
				/* ADC read command */
				case EDU_LPC_CMD::adc_read:
					/* Read ADC channel and send data*/
					cmdAdcRead(rxBuffer[DATA1_INDEX]);
					break;
				/* LM35 read temperature Celsius */
				case EDU_LPC_CMD::lm35_c:
					/* Update LM35 data and send data */
					cmdLm35(EDU_LPC_CMD::lm35_c);
					break;
				/* LM35 read temperature Fahrenheit */
				case EDU_LPC_CMD::lm35_f:
					/* Update LM35 data and send data */
					cmdLm35(EDU_LPC_CMD::lm35_f);
					break;
				/* DAC set value command */
				case EDU_LPC_CMD::dac_set:
					/* Get 10-bit value to set in DAC */
					val = _8bit_to_16bit(rxBuffer[DATA2_INDEX], rxBuffer[DATA3_INDEX]);
					/* Set DAC value */
					cmdDacSet(rxBuffer[DATA1_INDEX], val);
					break;
				/* DAC sine wave command */
				case EDU_LPC_CMD::dac_sine:
					/* Build 16-bit frequency value */
					freq = _8bit_to_16bit(rxBuffer[DATA2_INDEX], rxBuffer[DATA3_INDEX]);
					/* Generate sine wave with the given frequency */
					cmdDacSine(rxBuffer[DATA1_INDEX], freq);
					break;
				/* DAC triangular wave command */
				case EDU_LPC_CMD::dac_triangular:
					/* Build 16-bit frequency value */
					freq = _8bit_to_16bit(rxBuffer[DATA2_INDEX], rxBuffer[DATA3_INDEX]);
					/* Generate triangular wave with the given frequency */
					cmdDacTriangular(rxBuffer[DATA1_INDEX], freq);
					break;
				/* DAC wave enable command */
				case EDU_LPC_CMD::dac_wave:
					/* Start or stop DAC timer depending on the command */
					cmdDacWave(rxBuffer[DATA1_INDEX], (bool)rxBuffer[DATA2_INDEX]);
					break;
				/* PWM configuration command */
				case EDU_LPC_CMD::pwm_config:
					/* Build 16-bit frequency value */
					freq = _8bit_to_16bit(rxBuffer[DATA2_INDEX], rxBuffer[DATA3_INDEX]);
					/* Get duty cycle value */
					duty = rxBuffer[DATA4_INDEX];
					/* Configure PWM with given settings */
					cmdPwmConfig(rxBuffer[DATA1_INDEX], freq, duty);
					break;
				/* BMP180 read command */
				case EDU_LPC_CMD::bmp_read:
					/* Update BMP180 data and send */
					cmdBmpRead();
					break;
			}
		}
		/* Reset buffer index */
		rxIndex = 0;
		/* Clear flag */
		rxDone = false;
	}
}

/*!
 * @brief EDU LPC cmdConfig method.

 * Handles the configuration command and sets one
 * of the available GPIO to either INPUT, OUTPUT,
 * ADC channel, DAC output or PWM output.
 *
 * @param None.
 *
 * @retval None.
 */
void EDU_LPC::cmdConfig(uint8_t pin, EDU_LPC_CONFIG function) {
	/* Calculate ADC channel and array index */
	int index = pin - GPIO_START_INDEX;
	int channel = ADC_LAST_CHANNEL - index;
	/* Create a Pin instance */
	if(function == EDU_LPC_CONFIG::input || function == EDU_LPC_CONFIG::output) {
		/* Free memory of conflicting objects */
		delete adc[channel];
		/* Delete pointer */
		adc[channel] = nullptr;
		/* Only for PWM0-3 */
		if(index < 4) {
			/* Free memory and delete pointer */
			delete pwm[index];
			pwm[index] = nullptr;
		}
		/* Only for PIO0_17 */
		if(pin == DAC0_PIN_INDEX) {
			/* Free memory and delete pointer */
			delete dac[0];
			dac[0] = nullptr;
		}
		/* Allocate memory for a Pin instance */
		Pin *gpio = new Pin(pin, (uint8_t)function);
		/* Free memory */
		delete gpio;
		return;
	}
	/* Create an ADC instance */
	else if(function == EDU_LPC_CONFIG::adc) {
		/* Delete conflicting objects */
		delete pwm[index];
		/* Delete pointer */
		pwm[index] = nullptr;
		/* Only for PIO0_17 */
		if(pin == DAC0_PIN_INDEX) {
			/* Free memory and delete pointer */
			delete dac[0];
			dac[0] = nullptr;
		}
		/* Allocate memory for a new ADC instance */
		adc[channel] = new ADC(channel + 1);
	}
	/* Create a DAC instance */
	else if(function == EDU_LPC_CONFIG::dac) {
		/* Free memory of conflicting objects */
		delete adc[channel];
		/* Delete pointer */
		adc[channel] = nullptr;
		/* Only for PWM0-3 */
		if(index < 4) {
			/* Free memory and delete pointer */
			delete pwm[index];
			pwm[index] = nullptr;
		}
		/* Allocate memory for a new DAC instance */
		dac[0] = new DAC(0UL);
	}
	/* Create a PWM instance */
	else if(function == EDU_LPC_CONFIG::pwm) {
		/* Free memory of conflicting object */
		delete adc[channel];
		/* Delete pointer */
		adc[channel] = nullptr;
		/* Only for PIO0_17 */
		if(pin == DAC0_PIN_INDEX) {
			/* Free memory and delete pointer */
			delete dac[0];
			dac[0] = nullptr;
		}
		/* Allocate memory for a new PWM instance */
		pwm[index] = new PWM(pin);
	}
}

/*!
 * @brief EDU LPC cmdGpioClr method.

 * Clears one of the available GPIO.
 *
 * @param pin index of the pin used.
 *
 * @retval None.
 */
void EDU_LPC::cmdGpioClr(uint8_t pin) {
	/* Low level pin clear */
	GPIO->CLR[0] |= 1UL << pin;
}

/*!
 * @brief EDU LPC cmdGpioSet method.

 * Sets one of the available GPIO.
 *
 * @param pin index of the pin used.
 *
 * @retval None.
 */
void EDU_LPC::cmdGpioSet(uint8_t pin) {
	/* Low level pin set */
	GPIO->SET[0] |= 1UL << pin;
}

/*!
 * @brief EDU LPC cmdGpioToggle method.

 * Toggles one of the available GPIO.
 *
 * @param pin index of the pin used.
 *
 * @retval None.
 */
void EDU_LPC::cmdGpioToggle(uint8_t pin) {
	/* Low level pin toggle */
	GPIO->NOT[0] |= 1UL << pin;
}

/*!
 * @brief EDU LPC cmdGpioRead method.

 * Returns the value of one of the available
 * GPIO.
 *
 * @param pin index of the pin to be used.
 *
 * @retval None.
 */
void EDU_LPC::cmdGpioRead(uint8_t pin) {
	/* Build two byte buffer */
	uint8_t buff[] = {
		/* GPIO status command */
		(uint8_t)EDU_LPC_CMD::gpio_read,
		/* Selected pin state */
		GPIO->B[0][pin]
	};
	/* Build the full buffer and send */
	buildBuffer(buff, 2);
}

/*!
 * @brief EDU LPC cmdAdcRead method.

 * Reads the requested ADC channel and
 * sends the result via USART.
 *
 * @param channel index of the channel to be used.
 *
 * @retval None.
 */
void EDU_LPC::cmdAdcRead(uint8_t channel) {
	/* Fix channel value to fit array */
	if(channel > 1) { channel--; }
	/* Check if the ADC instance was created */
	if(adc[ADC_LAST_CHANNEL - channel] == nullptr) {
		return;
	}
	/* Get ADC value */
	uint16_t value = adc[ADC_LAST_CHANNEL - channel]->read();
	/* Get low value */
	uint8_t low = value & 0xff;
	/* Get high value */
	uint8_t high = value >> 8;
	/* Build buffer */
	uint8_t buff[] = {
			/* ADC read command */
			(uint8_t)EDU_LPC_CMD::adc_read,
			/* 4-bit high value */
			high,
			/* 8-bit low value */
			low
	};
	/* Build the full buffer and send */
	buildBuffer(buff, 3);
}

/*!
 * @brief EDU LPC cmdLm35 method.

 * Updates the LM35 data and sends the
 * temperature in Celsius or Fahrenheit
 * via USART.
 *
 * @param cmd whether the user asked the
 * temperature in Celsius or Fahrenheit.
 *
 * @retval None.
 */
void EDU_LPC::cmdLm35(EDU_LPC_CMD cmd) {
	/* Update LM35 data */
	lm->update();
	/* Temperature variable */
	float temp;
	/* Check command */
	if(cmd == EDU_LPC_CMD::lm35_c) {
		/* Get temperature in Celsius */
		temp = lm->getTempC();
	}
	else {
		/* Get temperature in Fahrenheit */
		temp = lm->getTempF();
	}
	/* Get the integer value of the temperature */
	uint8_t high = (uint8_t)temp;
	/* Get the decimal value of the temperature */
	uint8_t low = (temp - high) * 100;
	/* Build buffer */
	uint8_t buff[] = {
		/* LM35 read command */
		(uint8_t)cmd,
		/* Integer value */
		high,
		/* Decimal value */
		low
	};
	/* Build full buffer and send */
	buildBuffer(buff, 3);
}

/*!
 * @brief EDU LPC cmdDacSet method.

 * Sets a value to the requested DAC output.
 *
 * @param channel index of the output to be used.
 * @param value to be set.
 *
 * @retval None.
 */
void EDU_LPC::cmdDacSet(uint8_t channel, uint16_t value) {
	/* Check if there is a DAC instance to use */
	if(dac[channel] != nullptr) {
		/* Set DAC value */
		dac[channel]->set(value);
	}
}

/*!
 * @brief EDU LPC cmdDacSine method.

 * Configurates a sine wave to be outputted
 * in the requested DAC channel.
 *
 * @param channel index of the output to be used.
 * @param frequency of the sine wave.
 *
 * @retval None.
 */
void EDU_LPC::cmdDacSine(uint8_t channel, uint32_t frequency) {
	/* Check if there is a DAC instance to use */
	if(dac[channel] != nullptr) {
		/* Configure sine wave with given frequency */
		dac[channel]->sine(frequency);
		/* Stop DAC timer */
		dac[channel]->getTimer()->stop();
	}
}

/*!
 * @brief EDU LPC cmdDacTriangular method.

 * Configurates a triangular wave to be outputted
 * in the requested DAC channel.
 *
 * @param channel index of the output to be used.
 * @param frequency of the triangular wave.
 *
 * @retval None.
 */
void EDU_LPC::cmdDacTriangular(uint8_t channel, uint32_t frequency) {
	/* Check if there is a DAC instance to use */
	if(dac[channel] != nullptr) {
		/* Configure triangular wave with given frequency */
		dac[channel]->triangular(frequency);
		/* Stop DAC timer */
		dac[channel]->getTimer()->stop();
	}
}

/*!
 * @brief EDU LPC cndDacWave method.

 * Enables or disables the wave output in the
 * requested DAC channel.
 *
 * @param channel index of the output to be used.
 * @param enable whether the output is enabled or not.
 *
 * @retval None.
 */
void EDU_LPC::cmdDacWave(uint8_t channel, bool enable) {
	/* Check if there is a DAC instance to use */
	if(dac[channel] != nullptr) {
		/* Check if user wants to enable the wave output */
		if(enable) {
			/* Reset DAC timer counter */
			dac[channel]->getTimer()->reset();
			/* Start DAC timer counter */
			dac[channel]->getTimer()->start();
		}
		else {
			/* Stop DAC timer counter */
			dac[channel]->getTimer()->stop();
		}
	}
}

/*!
 * @brief EDU LPC cmdPwmConfig method.

 * Configurates a PWM output with the requested
 * settings in one of the available channels.
 *
 * @param channel index of the PWM output to be used.
 * @param frequency of the PWM signal.
 * @param duty cycle percentage of the PWM signal.
 *
 * @retval None.
 */
void EDU_LPC::cmdPwmConfig(uint8_t channel, uint32_t frequency, uint8_t duty) {
	/* Check if there is a PWM instance to use */
	if(pwm[channel] != nullptr) {
		/* Check if the current frequency is different */
		if(pwm[channel]->getFrequency() != frequency) {
			/* Change all the available PWM frequencies to the given one */
			int i = 0;
			while(pwm[i] != nullptr) {
				/* Set new frequency */
				pwm[i]->setFrequency(frequency);
				i++;
			}
		}
		/* Check if the current duty is different */
		if(pwm[channel]->getDuty() != duty) {
			/* Change duty cycle with the given one */
			pwm[channel]->setDuty(duty);
		}
	}
}

/*!
 * @brief EDU LPC cmdBmpRead method.

 * Updates the BMP180 values and sends the pressure
 * and temperature via USART.
 *
 * @param None.
 *
 * @retval None.
 */
void EDU_LPC::cmdBmpRead(void) {
	/* Update BMP180 values */
	bmp->read();
	/* Get temperature value */
	float temp = bmp->getTemperature();
	/* Get pressure value */
	int32_t pres = bmp->getPressure();
	/* Separate pressure in 3 bytes */
	uint8_t presHigh = (pres & 0xff0000) >> 16;
	uint8_t presMid = (pres & 0xff00) >> 8;
	uint8_t presLow = pres & 0xff;
	/* Get integer and decimal temperature values */
	uint8_t tempHigh = (uint8_t)temp;
	uint8_t tempLow = (temp - tempHigh) * 100;
	/* Build buffer */
	uint8_t buff[] = {
		/* BMP180 read command */
		(uint8_t)EDU_LPC_CMD::bmp_read,
		/* Upper 8-bit pressure value */
		presHigh,
		/* Middle 8-bit pressure value */
		presMid,
		/* Lower 8-bit pressure value */
		presLow,
		/* Integer temperature value */
		tempHigh,
		/* Decimal temperature value */
		tempLow
	};
	/* Build full buffer and send */
	buildBuffer(buff, 6);
}

/*!
 * @brief EDU LPC calculateChecksum method.

 * Calculates the checksum of the incoming buffer.
 *
 * @param data pointer to the received buffer.
 * @param size number of bytes to use in the calculation.
 *
 * @retval checksum value.
 */
uint8_t EDU_LPC::calculateChecksum(uint8_t *data, uint8_t size) {
	/* Checksum variable */
	uint16_t checksum = 0;
	/* Checksum calculation starts after the length byte */
	for(int i = 1; i < size; i++) {
		/* Add up every byte */
		checksum += data[i];
	}
	/* Get the lower 8-bit data */
	checksum &= 0xff;
	/* Return checksum value */
	return 0xff - checksum;
}

/*!
 * @brief EDU LPC buildBuffer method.

 * Copies the data to be sent in a new buffer adding
 * a header and checksum bytes and then sends it via USART.
 *
 * @param data pointer to the buffer to be sent.
 * @param size number of bytes to copy in the new buffer.
 *
 * @retval None.
 */
void EDU_LPC::buildBuffer(uint8_t *data, uint8_t size) {
	/* Get the header byte */
	uint8_t header = size + 1;
	/* Create new buffer */
	uint8_t buff[header + 1];
	/* Store header */
	buff[0] = header;
	/* Copy the data on the buffer */
	for(int i = 0; i < size; i++) {
		buff[i + 1] = data[i];
	}
	/* Get the checksum value */
	buff[header] = calculateChecksum(buff, header);
	/* Send buffer via polling */
	USART_WriteBlocking(USART1, buff, buff[0]);
}