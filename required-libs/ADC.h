/*
 * ADC.h
 *
 *  Created on: Jun 14, 2021
 *      Author: fabri
 */

#ifndef ADC_H_
#define ADC_H_

/* ADC includes */
#include <fsl_adc.h>
#include <fsl_power.h>
#include <fsl_swm.h>

/* ADC configuration struct */
typedef struct {

	adc_config_t config = {
		kADC_ClockSynchronousMode,	/* Synchronous clock source */
		1U,							/* Clock divider = 1 */
		false,						/* Low power mode disabled */
		kADC_HighVoltageRange, 		/* High voltage range */
	};
	adc_conv_seq_config_t sequence = {
		1U << 0,							/* Channel 0 */
		0U,									/* Trigger mask */
		kADC_TriggerPolarityPositiveEdge, 	/* Trigger on positive edge */
		false,								/* No sync bypass */
		false, 								/* No single step */
		kADC_InterruptForEachSequence		/* Interrupt mode */
	};
	uint8_t channel = 0U;	/* Selected channel */
} adc_user_config_t;

/* ADC constants */
static constexpr uint8_t SEQA = 0;

/* Class definition */
class ADC {

	public:
		/* Constructors */
		ADC(uint8_t channel = 0);
		void select(void);
		void start(void);
		bool ready(void);
		uint16_t read(void);
		uint16_t getResult(void);
		void attachInterrupt(void (*f)(void));

	private:
		/* Configuration data */
		adc_user_config_t settings;
};

/* Inline methods */

/*!
 * @brief ADC select method.

 * Selects the channel to be converted.
 *
 * @param None.
 *
 * @retval None.
 */
inline void ADC::select(void) {
	/* Disable Sequence A prior change */
	ADC0->SEQ_CTRL[SEQA] &= ~ADC_SEQ_CTRL_SEQ_ENA_MASK;
	/* Clear Sequence A channels */
	ADC0->SEQ_CTRL[SEQA] &= ~ADC_SEQ_CTRL_CHANNELS_MASK;
	/* Select the channel to convert*/
	ADC0->SEQ_CTRL[SEQA] |= 1 << settings.channel;
	/* Enable Sequence A */
	ADC0->SEQ_CTRL[SEQA] |= ADC_SEQ_CTRL_SEQ_ENA_MASK;
}

/*!
 * @brief ADC start method.

 * Starts the conversion.
 *
 * @param None.
 *
 * @retval None.
 */
inline void ADC::start(void) {
	/* Software trigger conversion */
	ADC0->SEQ_CTRL[SEQA] |= ADC_SEQ_CTRL_START_MASK;
}

/*!
 * @brief ADC ready method.

 * Checks if the conversion is ready to be read.
 *
 * @param None.
 *
 * @retval true if ready, false otherwise.
 */
inline bool ADC::ready(void) {
	/* Check for conversion ready flag and cast it to boolean */
	return (bool)(ADC0->DAT[settings.channel] & ADC_DAT_DATAVALID_MASK);
}

#endif /* ADC_H_ */
