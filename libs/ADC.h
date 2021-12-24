/*
 * ADC.h
 *
 *  Created on: Jun 14, 2021
 *      Author: fabri
 */

#ifndef ADC_H_
#define ADC_H_

/*  ADC includes  */

#include <fsl_adc.h>
#include <fsl_common.h>

/*  ADC IRQ indexes  */

#define ADC_SEQA_IRQ_INDEX	0
#define ADC_SEQB_IRQ_INDEX	1
#define ADC_THCMP_IRQ_INDEX	2
#define ADC_OVR_IRQ_INDEX	3

#define SYSCON_PDRUNCFG_ADC0_SHIFT	4UL

/*  Interrupts available  */

typedef enum {
	kSEQA_IRQ,
	kSEQB_IRQ,
	kTHCMP_IRQ,
	kOVR_IRQ
} adc_irq_type_t;

/*  Class definition  */

class ADC {

	public:
	
		ADC(void);
		ADC(uint32_t channel);
		uint32_t read(void);
		uint32_t ready(void);
		uint32_t getResult(void);
		void attachInterrupt(void (*f)(void), adc_irq_type_t type);

	private:
	
		uint32_t selected_channel;

		void init(uint32_t channel);
};

#endif /* ADC_H_ */
