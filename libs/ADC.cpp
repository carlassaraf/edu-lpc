/*
 * ADC.cpp
 *
 *  Created on: Jun 14, 2021
 *      Author: fabri
 */

#include <ADC.h>

/*  ADC IRQ handler pointer  */
static void (*irq_ptr)(void);

/*!
 * @brief ADC constructor.

 * Creates an ADC object.
 *
 * @param channel index of the channel to be used.
 * Defaults to 0.
 *
 * @retval None.
 */
ADC::ADC(uint8_t channel) {
	/* Enable clock for SWM */
	CLOCK_EnableClock(kCLOCK_Swm);
	/* Set fixed function on the pin associated with the channel */
	SWM_SetFixedPinSelect(SWM0, (swm_select_fixed_pin_t)(kSWM_ADC_CHN0 << channel), true);
	/* Disable clock for SWM */
	CLOCK_DisableClock(kCLOCK_Swm);
	/* Enable clock for ADC0 */
	CLOCK_Select(kADC_Clk_From_Fro);
	/* ADC0 clock same as FRO */
	CLOCK_SetClkDivider(kCLOCK_DivAdcClk, 1U);
	/* Enable ADC0 power */
	POWER_DisablePD(kPDRUNCFG_PD_ADC0);
	/* Get ADC0 frequency */
	uint32_t freq = CLOCK_GetFreq(kCLOCK_Fro) / CLOCK_GetClkDivider(kCLOCK_DivAdcClk);
	/* Calibrate ADC0 */
	ADC_DoSelfCalibration(ADC0, freq);
	/* Update selected channel in settings */
	settings.channel = channel;
	settings.sequence.channelMask = 1U << channel;
	/* Initialize ADC0 with given configuration */
	ADC_Init(ADC0, &settings.config);
	/* Configure Sequence A */
	ADC_SetConvSeqAConfig(ADC0, &settings.sequence);
	/* Enable conversion for Sequenve A */
	ADC_EnableConvSeqA(ADC0, true);
}

/*!
 * @brief ADC read method.

 * Returns the conversion value if interrupts
 * are enabled or starts a new one and returns
 * the value when ready if not.
 *
 * @param None.
 *
 * @retval conversion result.
 */
uint16_t ADC::read(void) {
	/* Check if an interrupt is enabled */
	if(ADC0->INTEN) {
		/* Return if there is */
		return ((ADC0->DAT[settings.channel] & ADC_DAT_RESULT_MASK)) >> 4;
	}
	/* Select channel */
	select();
	/* Start conversion */
	start();
	/* If not, get the result when is ready */
	return getResult();
}

/*!
 * @brief ADC getResult method.

 * Returns the result of the conversion if it's ready.
 *
 * @param None.
 *
 * @retval conversion result.
 */
uint16_t ADC::getResult(void) {
	/* Wait for the conversion to be ready */
	while(!ready());
	/* Return data when ready */
	return (uint16_t)(((ADC0->DAT[settings.channel] & ADC_DAT_RESULT_MASK)) >> 4);
}

/*!
 * @brief ADC attachInterrupt method.

 * Attach a function as a ADC interrupt handler.
 * Only supports Sequence A interrupt.
 *
 * @param f function to be called.
 *
 * @retval None.
 */
void ADC::attachInterrupt(void (*f)(void), adc_irq_type_t type) {
	/* Store function in pointer */
	irq_ptr = f;
	/* Enable Sequence A interrupt */
	ADC0->INTEN |= 1 << SEQA;
	NVIC_EnableIRQ(ADC0_SEQA_IRQn);
}

/* ADC IRQ handlers */
extern "C" {
	/* Overwrite default IRQHandler */
	void ADC0_SEQA_IRQHandler(void) {
		/* Check if there is a function associated */
		if(*irq_ptr) {
			/* Call the function */
			(*irq_ptr)();
		}
	}
}
