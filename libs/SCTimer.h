/*
 * SCTimer.h
 *
 *  Created on: Jan 13, 2022
 *      Author: fabri
 */

#ifndef SCTIMER_H_
#define SCTIMER_H_

#include "fsl_sctimer.h"

class SCTimer {

	public:

		SCTimer(uint32_t index = 0);
		SCT_Type* getSCTimer(void);

	private:

		SCT_Type *timer;
};

#endif /* SCTIMER_H_ */
