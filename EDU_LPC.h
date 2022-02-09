/*
 * EDU_LPC.h
 *
 *  Created on: Jan 11, 2022
 *      Author: fabri
 */

#ifndef EDU_LPC_H_
#define EDU_LPC_H_

/* EDU LPC includes */
#include "GPIO.h"
#include "USART.h"
#include "DAC.h"
#include "LM35.h"
#include "PWM.h"
#include "BMP180.h"
#include "MPU9250.h"

/* EDU LPC constant expressions */
static constexpr uint8_t SIZE_INDEX = 0;
static constexpr uint8_t CMD_INDEX = 1;
static constexpr uint8_t DATA1_INDEX = 2;
static constexpr uint8_t DATA2_INDEX = 3;
static constexpr uint8_t DATA3_INDEX = 4;
static constexpr uint8_t DATA4_INDEX = 5;

static constexpr uint8_t GPIO_START_INDEX = 16;
static constexpr uint8_t ADC_LAST_CHANNEL = 10;
static constexpr uint8_t DAC0_PIN_INDEX = 17;

/* Inline functions */
inline uint16_t _8bit_to_16bit(uint8_t high, uint8_t low) {
	/* Build a 16-bit value with two bytes */
	return ((uint16_t)high << 8) + low;
}

inline void _to_arr(float *from, uint8_t *to, uint8_t start, uint8_t finish) {
	/* Separate the high and low values */
	for(uint8_t i = 0; i < finish; i++) {
		/* Get high value */
		to[start + i] = (uint8_t)*from;
		/* Get low value */
		to[start + i + 1] = (uint8_t)(((*from) - (int8_t)(*from)) * 100);
		/* Increment second counter */
		start++;
	}
}

/* EDU LPC enum command class */
enum class EDU_LPC_CMD : uint8_t {
	config = 0x01,				/* Set GPIO function */
	gpio_clear = 0x02,			/* GPIO clear output */
	gpio_set = 0x03,			/* GPIO set output */
	gpio_toggle = 0x04,			/* GPIO toggle output */
	gpio_read = 0x05,			/* GPIO read value and send */
	adc_read = 0x06,			/* ADC read channel and send */
	lm35_c = 0x07,				/* LM35 read and send temperature in Celsius */
	lm35_f = 0x08,				/* LM35 read and send temperature in Fahrenheit */
	dac_set = 0x09,				/* DAC set output value */
	dac_sine = 0x0a,			/* DAC configure sine wave */
	dac_triangular = 0x0b,		/* DAC configure triangular wave */
	dac_wave = 0x0c,			/* DAC enable or disable wave output */
	pwm_config = 0x0d,			/* PWM configure signal */
	bmp_read = 0x0e,			/* BMP180 read values and send */
	mpu_read = 0x0f				/* MPU9250 read values and send */
};

/* EDU LPC enum GPIO configuration class */
enum class EDU_LPC_CONFIG : uint8_t {
	input = 0x00,		/* GPIO as input */
	output = 0x01,		/* GPIO as output */
	adc = 0x02,			/* GPIO as ADC */
	dac = 0x03,			/* GPIO as DAC */
	pwm = 0x04			/* GPIO as PWM */
};

/* Class definition */
class EDU_LPC {

	public:
		/* RGB LED constants */
		static constexpr uint8_t GLED {0};
		static constexpr uint8_t BLED {1};
		static constexpr uint8_t RLED {2};
		/* BTN constants */
		static constexpr uint8_t BTN1 {0};
		static constexpr uint8_t BTN2 {1};
		static constexpr uint8_t BTN3 {2};
		static constexpr uint8_t USER {3};
		/* Output constants */
		static constexpr uint8_t Relay {0};
		/* ADC constants */
		static constexpr uint8_t Pot {0};
		static constexpr uint8_t LM35Ch {1};
		static constexpr uint8_t ADCh3 {2};
		static constexpr uint8_t ADCh4 {3};
		static constexpr uint8_t ADCh5 {4};
		static constexpr uint8_t ADCh6 {5};
		static constexpr uint8_t ADCh7 {6};
		static constexpr uint8_t ADCh8 {7};
		static constexpr uint8_t ADCh9 {8};
		/* DAC constants */
		static constexpr uint8_t DACh0 {0};
		static constexpr uint8_t DACh1 {1};
		/* PWM constants */
		static constexpr uint8_t PWM1 {0};
		static constexpr uint8_t PWM2 {1};
		static constexpr uint8_t PWM3 {2};
		static constexpr uint8_t PWM4 {3};
		/* Constructors */
		EDU_LPC(void);
		/* Public methods */
		void handler(void);
		/* Get methods */
		Pin* getRGB(uint8_t led);
		Pin* getBtn(uint8_t bt);
		Pin* getOutput(uint8_t out);
		ADC* getADC(uint8_t channel);
		LM35* getLM35(void);
		DAC* getDAC(uint8_t channel);
		PWM* getPWM(uint8_t channel);
		BMP180* getBMP180(void);
		MPU9250* getMPU9250(void);

	private:
		/* Module instances */
		LM35 *lm { new LM35 };
		BMP180 *bmp { new BMP180 };
		MPU9250 *mpu { new MPU9250 };
		/* Board RGB LED */
		Pin *rgb[3] = { new Pin(32, Pin::OUTPUT, 1), new Pin(33, Pin::OUTPUT, 1), new Pin(34, Pin::OUTPUT, 1) };
		/* Button array */
		Pin *btn[4] = { new Pin(8, Pin::Input), new Pin(9, Pin::Input), new Pin(13, Pin::Input), new Pin(4, Pin::Input) };
		/* Outputs */
		Pin *outs[1] = { new Pin(14, Pin::Output, 0) };
		/* Array pointer to class instances */
		/* ADC0 (potentiometer), ADC1 (LM35), ADC3, ADC4, ADC5, ADC6, ADC7, ADC8, ADC9 */
		ADC *adc[9] = { new ADC(0), lm->getADC(), nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
		DAC *dac[2] = { nullptr, new DAC(1UL) };
		/* PWM0 to PWM3 in GPIO0 to GPIO3 */
		PWM *pwm[4] = { nullptr, nullptr, nullptr, nullptr };
		/* Command handlers */
		void cmdConfig(uint8_t pin, EDU_LPC_CONFIG function);
		void cmdGpioClr(uint8_t pin);
		void cmdGpioSet(uint8_t pin);
		void cmdGpioToggle(uint8_t pin);
		void cmdGpioRead(uint8_t pin);
		void cmdAdcRead(uint8_t channel);
		void cmdLm35(EDU_LPC_CMD cmd);
		void cmdDacSet(uint8_t channel, uint16_t value);
		void cmdDacSine(uint8_t channel, uint32_t frequency);
		void cmdDacTriangular(uint8_t channel, uint32_t frequency);
		void cmdDacWave(uint8_t channel, bool enable);
		void cmdPwmConfig(uint8_t channel, uint32_t frequency, uint8_t duty);
		void cmdBmpRead(void);
		void cmdMpuRead(void);
		/* Miscelaneus methods */
		uint8_t calculateChecksum(uint8_t *data, uint8_t size);
		void buildBuffer(uint8_t *data, uint8_t size);
		/* Static variables to handle the reception */
		static uint8_t rxBuffer[8];
		static uint8_t rxIndex;
		static uint8_t	rxSize;
		static bool rxDone;
		/* RX ready interrupt handler */
		static void serialRx(void);
};

/* Inline methods */

/*!
 * @brief EDU LPC getRGB method.

 * Returns a pointer to one of the Board RGB's LED.
 *
 * @param led to be retrieved. Possible values are:
 * - RLED
 * - GLED
 * - BLED
 *
 * @retval pointer to a Pin object.
 */
inline Pin* EDU_LPC::getRGB(uint8_t led) { return rgb[led]; }

/*!
 * @brief EDU LPC getBtn method.

 * Returns a pointer to a Button pointer.
 *
 * @param bt button to be retrieved. Posible values are:
 * - BTN1
 * - BTN2
 * - BTN3
 * - USER
 *
 * @retval pointer to a Pin object.
 */
inline Pin* EDU_LPC::getBtn(uint8_t bt) { return btn[bt]; }

/*!
 * @brief EDU LPC getOutputs method.

 * Returns a pointer to a Board output.
 *
 * @param out to be retrived. Posible values are:
 * - Relay
 *
 * @retval pointer to a Pin object.
 */
inline Pin* EDU_LPC::getOutput(uint8_t out) { return outs[out]; }

/*!
 * @brief EDU LPC getADC method.

 * Returns a pointer to the selected ADC.
 *
 * @param channel ADC to be retrieved. Possible values are:
 * - Pot
 * - LM35
 * - ADCh3
 * - ADCh4
 * - ADCh5
 * - ADCh6
 * - ADCh7
 * - ADCh8
 * - ADCh9
 *
 * @retval pointer to an ADC object.
 */
inline ADC* EDU_LPC::getADC(uint8_t channel) { return adc[channel]; }

/*!
 * @brief EDU LPC getLM35 method.

 * Returns a pointer to the LM35 object
 * in ADC channel 1.
 *
 * @param None.
 *
 * @retval pointer to a LM35 object.
 */
inline LM35* EDU_LPC::getLM35(void) { return lm; }

/*!
 * @brief EDU LPC getDAC method.

 * Returns a pointer to the requested DAC.
 *
 * @param channel of the DAC to be retrived. Possible
 * values are:
 * - DACh0
 * - DACh1
 *
 * @retval pointer to a DAC object.
 */
inline DAC* EDU_LPC::getDAC(uint8_t channel) { return dac[channel]; }

/*!
 * @brief EDU LPC getPWM method.

 * Returns a pointer to the requested PWM.
 *
 * @param channel of the PWM to be retrived. Possible
 * values are:
 * - PWM1
 * - PWM2
 * - PWM3
 * - PWM4
 *
 * @retval pointer to a PWM object.
 */
inline PWM* EDU_LPC::getPWM(uint8_t channel) { return pwm[channel]; }

/*!
 * @brief EDU LPC getBMP180 method.

 * Returns a pointer to the BMP180
 * object in I2C0.
 *
 * @param None.
 *
 * @retval pointer to a BMP180 object.
 */
inline BMP180* EDU_LPC::getBMP180(void) { return bmp; }

/*!
 * @brief EDU LPC serialRx method.

 * Stores the bytes received via USART1
 * interrupt and keeps track of the index
 * and flag.
 *
 * @param None.
 *
 * @retval None.
 */
inline void EDU_LPC::serialRx(void) {
	/* Store incoming byte and increment index */
	rxBuffer[rxIndex++] = USART0->RXDAT;
	/* Check if the communication is done */
	if(rxIndex > rxBuffer[0]) {
		/* Set flag */
		rxDone = true;
		/* Decrement rxIndex to get actual buffer size*/
		rxIndex--;
	}
}

#endif /* EDU_LPC_H_ */