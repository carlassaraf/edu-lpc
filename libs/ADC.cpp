/*
 * ADC.cpp
 *
 *  Created on: Jun 14, 2021
 *      Author: fabri
 */

#include <ADC.h>

void (*adc_irq_ptr[4])(void);

IRQn_Type irq_arr[4] = {
	ADC0_SEQA_IRQn,
	ADC0_SEQB_IRQn,
	ADC0_THCMP_IRQn,
	ADC0_OVR_IRQn
};

ADC::ADC(uint32_t channel) {

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

ADC::~ADC() {
	// TODO Auto-generated destructor stub
}

uint32_t ADC::read(void) {

	ADC0->SEQ_CTRL[0] |= 1 << selected_channel;
	ADC0->SEQ_CTRL[0] |= 0x04000000;

	if(ADC0->INTEN) {
		return 0;
	}

	uint32_t result;

	while(!ready());
	result = ((ADC0->DAT[selected_channel] & ADC_DAT_RESULT_MASK)) >> 4;

    return result;
}

uint32_t ADC::ready(void) {

	return ADC0->DAT[selected_channel] & ADC_DAT_DATAVALID_MASK;
}

void ADC::attachInterrupt(void (*f)(void), adc_irq_type_t type) {

	adc_irq_ptr[type] = f;

	ADC_EnableInterrupts(ADC0, kADC_ConvSeqAInterruptEnable);
	NVIC_EnableIRQ(irq_arr[type]);
//	ADC0->INTEN |= 1 << type;
}

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
