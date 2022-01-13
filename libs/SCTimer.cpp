/*
 * SCTimer.cpp
 *
 *  Created on: Jan 13, 2022
 *      Author: fabri
 */

#include <SCTimer.h>

/*!
 * @brief SCTimer constructor.

 * Creates a SCTimer object. Default timmer is SCT0.
 *
 * @param index SCTimer index.
 *
 * @retval None.
 */
SCTimer::SCTimer(uint32_t index) {

	sctimer_config_t sctimerConfig;

	if(index == 0) {
		timer = SCT0;
	}

	SCTIMER_Deinit(timer);				// Reset any prevous SCTimer configuration
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
	SCTIMER_Init(timer, &sctimerConfig);
}

/*!
 * @brief SCTimer getSCTimer method.

 * Returns a pointer to the SCTimer type.
 *
 * @param None.
 *
 * @retval SCT pointer.
 */
SCT_Type* SCTimer::getSCTimer(void) {
	return timer;
}
