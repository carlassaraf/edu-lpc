/*
 * ADC.cpp
 *
 *  Created on: Jun 14, 2021
 *      Author: fabri
 */

#include <ADC.h>

/*  ADC IRQ handler pointer  */
void (*adc_irq_ptr[4])(void);

/*  Array with the different USART IRQs  */
IRQn_Type irq_arr[4] = {
	ADC0_SEQA_IRQn,
	ADC0_SEQB_IRQn,
	ADC0_THCMP_IRQn,
	ADC0_OVR_IRQn
};

/*!
 * @brief ADC constructor.

 * Creates an ADC object. Default channel is 0.
 *
 * @param None.
 *
 * @retval None.
 */
ADC::ADC(void) {
	init(0);
}

/*!
 * @brief ADC constructor.

 * Creates an ADC object.
 *
 * @param channel index of the channel to be used.
 *
 * @retval None.
 */
ADC::ADC(uint32_t channel) {

	init(channel);
}

/*!
 * @brief ADC read method.

 * Initiates a new conversion and returns the
 * value if no interrupt is enabled.
 *
 * @param None.
 *
 * @retval conversion result if read via polling.
 * Returns 0 otherwise.
 */
uint32_t ADC::read(void) {

	ADC0->SEQ_CTRL[0] |= 1 << selected_channel;
	ADC0->SEQ_CTRL[0] |= 0x04000000;

	if(ADC0->INTEN) {
		return 0;
	}

	uint32_t result;

	return getResult();
}

/*!
 * @brief ADC ready method.

 * Checks if the conversion is ready to be read.
 *
 * @param None.
 *
 * @retval Returns 1 if ready, 0 otherwise.
 */
uint32_t ADC::ready(void) {

	return ADC0->DAT[selected_channel] & ADC_DAT_DATAVALID_MASK;
}

/*!
 * @brief ADC getResult method.

 * Returns the result of the conversion if it's ready.
 *
 * @param None.
 *
 * @retval conversion result.
 */
uint32_t ADC::getResult(void) {

	uint32_t result;

	while(!ready());
	result = ((ADC0->DAT[selected_channel] & ADC_DAT_RESULT_MASK)) >> 4;

    return result;
}

/*!
 * @brief ADC attachInterrupt method.

 * Attach a function as a ADC interrupt handler.
 * Only supports Sequence A and B interrupts.
 *
 * @param f function to be called.
 * @param type interrupt to be enabled.
 *
 * @retval None.
 */
void ADC::attachInterrupt(void (*f)(void), adc_irq_type_t type) {

	adc_irq_ptr[type] = f;

	ADC0->INTEN |= 1 << type;
	NVIC_EnableIRQ(irq_arr[type]);
}

/*!
 * @brief ADC init private method.

 * Deals with most of the ADC initialization.
 *
 * @param channel index of the channel to be used.
 *
 * @retval None.
 */
void ADC::init(uint32_t channel) {

	selected_channel = channel;

	SYSCON->SYSAHBCLKCTRL0 	|= SYSCON_SYSAHBCLKCTRL0_SWM_MASK;		// Activo la matriz de conmutacion (SWM)
	SWM0->PINENABLE0 &= ~(1 << (channel + SWM_PINENABLE0_ADC_0_SHIFT));
	SYSCON->SYSAHBCLKCTRL0	&= ~(SYSCON_SYSAHBCLKCTRL0_SWM_MASK);	// Desactivo el SWM para que tome efecto el cambio

	SYSCON->SYSAHBCLKCTRL0	|= SYSCON_SYSAHBCLKCTRL0_ADC_MASK;		// Activo el clock del ADC
	SYSCON->ADCCLKSEL = 0;	// FRO
	SYSCON->ADCCLKDIV = 1;	// FRO / 1
	SYSCON->PDRUNCFG &= ~(SYSCON_PDAWAKECFG_ADC_PD_MASK);			// Power ADC

	ADC0->TRM &= ~(ADC_TRM_VRANGE_MASK);		// V range 2.7V~3.6V
    ADC0->CTRL |= 0x40000000;					// calibracion del adc
    ADC0->CTRL &= ~0xFF;						// Synchronous mode.
    ADC0->CTRL |= 23;							// (FRO / 500KHz) - 1
    ADC0->CTRL &= ~0x400;						// low-power disabled
	for(uint32_t i = 0 ; i < 4000 ; i++);

	ADC0->INTEN	= 0;							// No interrupt
	ADC0->CTRL |= 1;							// FRO / 1
	ADC0->SEQ_CTRL[0] |= 0xC0040000;			// Seq A configuration
	ADC0->SEQ_CTRL[0] |= 1 << selected_channel;
}

/*  ADC IRQ handlers  */

extern "C" {

	void ADC0_SEQA_IRQHandler(void) {

		if((ADC0->FLAGS) & kADC_ConvSeqAInterruptFlag) {
			if(adc_irq_ptr[0]) {
				adc_irq_ptr[0]();
			}
			ADC0->FLAGS |= kADC_ConvSeqAInterruptFlag;
		}
	}

	void ADC0_SEQB_IRQHandler(void) {

		if((ADC0->FLAGS) & kADC_ConvSeqBInterruptFlag) {
			if(adc_irq_ptr[1]) {
				adc_irq_ptr[1]();
			}
			ADC0->FLAGS |= kADC_ConvSeqBInterruptFlag;
		}
	}

	void ADC0_THCMP_IRQHandler(void) {

		if(adc_irq_ptr[2]) {
			adc_irq_ptr[2]();
		}
	}

	void ADC0_OVR_IRQHandler(void) {

		if(adc_irq_ptr[3]) {
			adc_irq_ptr[3]();
		}
	}
}
