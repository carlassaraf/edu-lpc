/*
 * SPI.h
 *
 *  Created on: Dec 24, 2021
 *      Author: fabri
 */

#ifndef SPI_H_
#define SPI_H_

/*  SPI includes  */

#include <fsl_swm.h>
#include <fsl_spi.h>

/*  Class definition  */

class SPI {

	public:

		SPI(uint32_t spin, uint32_t mosi = 26, uint32_t miso = 27, uint32_t sck = 28, uint32_t ssel = 29);
		void transfer(uint8_t *tx, uint8_t *rx, uint8_t size);

	private:

		SPI_Type *spi;
};

#endif /* SPI_H_ */
