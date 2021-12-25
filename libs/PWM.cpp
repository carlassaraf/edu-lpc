/*
 * PWM.cpp
 *
 *  Created on: Dec 24, 2021
 *      Author: fabri
 */

#include <PWM.h>

/*!
 * @brief PWM constructor.

 * Creates a PWM object.
 *
 * @param pwm_output SCTimer channel to be used (0-6).
 * @param frequency PWM signal frequency in Hz.
 * @param outputPin index of the pin to be used.
 * @param duty PWM duty cycle in percentage. Defaults to 50%.
 *
 * @retval None.
 */
PWM::PWM(uint32_t pwm_output, uint32_t frequency, uint32_t outputPin, uint32_t duty) {

	pwmParam.output	= (sctimer_out_t)pwm_output;
	pwmParam.level = kSCTIMER_HighTrue;
	pwmParam.dutyCyclePercent = duty;

	pwmFrequency = frequency;

    CLOCK_EnableClock(kCLOCK_Swm);
    SWM_SetMovablePinSelect(SWM0, (swm_select_movable_t)(pwm_output + kSWM_SCT_OUT0), (swm_port_pin_type_t)outputPin);
    CLOCK_DisableClock(kCLOCK_Swm);

	init(frequency, kSCTIMER_HighTrue);
}

/*!
 * @brief PWM setDuty method.

 * Sets the PWM duty cycle.
 *
 * @param duty PWM duty cycle in percentage.
 *
 * @retval None.
 */
void PWM::setDuty(uint32_t duty) {

	SCTIMER_UpdatePwmDutycycle(SCT0, pwmParam.output, duty, event);
}

/*!
 * @brief PWM getDuty method.

 * Gets the current PWM duty cycle.
 *
 * @param None.
 * 
 * @retval PWM duty cycle in percentage.
 */
uint32_t PWM::getDuty(void) {

	return pwmParam.dutyCyclePercent;
}

/*!
 * @brief PWM setFrequency method.

 * Sets the PWM frequency.
 *
 * @param frequency PWM frequency in Hz.
 *
 * @retval None.
 */
void PWM::setFrequency(uint32_t frequency) {

	pwmFrequency = frequency;
	init(frequency, pwmParam.level);
}

/*!
 * @brief PWM getFrequency method.

 * Gets the current PWM frequency.
 *
 * @param None.
 * 
 * @retval PWM frequency in percentage.
 */
uint32_t PWM::getFrequency(void) {

	return pwmFrequency;
}

/*!
 * @brief PWM logicLevel method.

 * Sets the PWM logic level.
 *
 * @param logic polarity of the PWM.
 * - false: Low level PWM
 * - true: High level PWM
 *
 * @retval None.
 */
void PWM::logicLevel(bool logic) {

	init(pwmFrequency, logic);
}

/*!
 * @brief PWM setOutputPin method.

 * Sets the PWM output pin.
 *
 * @param pin index of the pin to be used.
 *
 * @retval None.
 */
void PWM::setOutputPin(uint32_t pin) {

    CLOCK_EnableClock(kCLOCK_Swm);
    SWM_SetMovablePinSelect(SWM0, (swm_select_movable_t)(pwmParam.output + kSWM_SCT_OUT0), (swm_port_pin_type_t)pin);
    CLOCK_DisableClock(kCLOCK_Swm);
}

/*!
 * @brief PWM init private method.

 * Deals with the initialization of the SCTimer and PWM.
 *
 * @param frequency PWM signal frequency in Hz.
 * @param logic polarity of the PWM.
 * - false: Low level PWM
 * - true: High level PWM
 *
 * @retval None.
 */
void PWM::init(uint32_t frequency, bool logic) {

	if(pwmParam.level != (sctimer_pwm_level_select_t)logic) {
		pwmParam.level = (sctimer_pwm_level_select_t)logic;
	}
	if(pwmFrequency != frequency) {
		pwmFrequency = frequency;
	}

	sctimer_config_t sctimerConfig;
	SCTIMER_Deinit(SCT0);
	SCTIMER_GetDefaultConfig(&sctimerConfig);
		/*
		 *  config->enableCounterUnify = true;
		 *  config->clockMode = kSCTIMER_System_ClockMode;
		 *  config->clockSelect = kSCTIMER_Clock_On_Rise_Input_0;
		 *  config->enableBidirection_l = false;
		 *  config->enableBidirection_h = false;
		 *  config->prescale_l = 0U;
		 *  config->prescale_h = 0U;
		 *  config->outInitState = 0U;
		 *  config->inputsync  = 0xFU;
		 */
	SCTIMER_Init(SCT0, &sctimerConfig);

	uint32_t sctimerClock = CLOCK_GetFreq(kCLOCK_Fro);
	SCTIMER_SetupPwm(SCT0, &pwmParam, kSCTIMER_CenterAlignedPwm, pwmFrequency, sctimerClock, &event);
	SCTIMER_StartTimer(SCT0, kSCTIMER_Counter_U);
}