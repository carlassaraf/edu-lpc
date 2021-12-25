/*
 * SPI.cpp
 *
 *  Created on: Dec 24, 2021
 *      Author: fabri
 */

#include <SPI.h>

/*!
 * @brief SPI constructor.

 * Creates an SPI object. 
 * 
 * @param spin index of the SPI to be used. Defaults to 0.
 * @param mosi index of the pin to be used. Defaults to 26.
 * @param miso index of the pin to be used. Defaults to 27.
 * @param sck index of the pin to be used. Defaults to 28.
 * @param ssel index of the pin to be used. Defaults to 29.
 *
 * @retval None.
 */
SPI::SPI(uint32_t spin, uint32_t mosi, uint32_t miso, uint32_t sck, uint32_t ssel) {

    CLOCK_EnableClock(kCLOCK_Swm);

    if(spin == 0) {
    	spi = SPI0;
        SWM_SetMovablePinSelect(SWM0, kSWM_SPI0_SCK, (swm_select_movable_t)sck);
        SWM_SetMovablePinSelect(SWM0, kSWM_SPI0_MOSI, (swm_select_movable_t)mosi);
        SWM_SetMovablePinSelect(SWM0, kSWM_SPI0_MISO, (swm_select_movable_t)miso);
        SWM_SetMovablePinSelect(SWM0, kSWM_SPI0_SSEL0, (swm_select_movable_t)ssel);
        CLOCK_Select(kSPI0_Clk_From_MainClk);
    }
    else if(spin == 1) {
    	spi = SPI1;
        SWM_SetMovablePinSelect(SWM0, kSWM_SPI1_SCK, (swm_select_movable_t)sck);
        SWM_SetMovablePinSelect(SWM0, kSWM_SPI1_MOSI, (swm_select_movable_t)mosi);
        SWM_SetMovablePinSelect(SWM0, kSWM_SPI1_MISO, (swm_select_movable_t)miso);
        SWM_SetMovablePinSelect(SWM0, kSWM_SPI1_SSEL0, (swm_select_movable_t)ssel);
        CLOCK_Select(kSPI0_Clk_From_MainClk);
    }

    CLOCK_DisableClock(kCLOCK_Swm);

    spi_master_config_t userConfig = {0};
    uint32_t srcFreq               = 0U;
    SPI_MasterGetDefaultConfig(&userConfig);
    userConfig.baudRate_Bps           = 500000;
    userConfig.sselNumber             = kSPI_Ssel0Assert;
    userConfig.clockPolarity		  = kSPI_ClockPolarityActiveHigh;
    userConfig.clockPhase             = kSPI_ClockPhaseFirstEdge;
    userConfig.delayConfig.frameDelay = 0x0U;
    srcFreq                           = CLOCK_GetFreq(kCLOCK_MainClk);
    SPI_MasterInit(spi, &userConfig, srcFreq);
}

/*!
 * @brief SPI transfer method.

 * Initiates a transfer through SPI. 
 * 
 * @param tx array with the bytes to be sent.
 * @param rx array to store the read bytes.
 * @param size number of bytes to be sent.
 * 
 * @retval None.
 */
void SPI::transfer(uint8_t *tx, uint8_t *rx, uint8_t size) {

    spi_transfer_t xfer = {0};
	xfer.txData      = tx;
	xfer.rxData      = rx;
	xfer.dataSize    = size;
	xfer.configFlags = kSPI_EndOfTransfer | kSPI_EndOfFrame;

	SPI_MasterTransferBlocking(spi, &xfer);
}
