/*
 * HAL_STM32_USB.C
 *
 *  Created on: 8 jul. 2019
 *      Author: TOSHIBA
 */

#include "HAL_STM32_USB.h"

/*
 * name:		HAL_STM32_USB_DEVICE_Init
 *
 * description:	init usb descriptor as a USB serial cdc device
 *
 * globals:		NONE
 *
 * parameters:	NONE
 *
 * returns:		NONE
 *
 * Autor:		menymp
 */

void HAL_STM32_USB_DEVICE_Init(void)
{
	MX_USB_DEVICE_Init();
}

/*
 * name:		HAL_STM32_CDC_Transmit_FS
 *
 * description:	trasmit buffer over USB serial
 *
 * globals:		NONE
 *
 * parameters:	buf		output buffer
 * 				Len		len of output buffer
 *
 * returns:		status code
 *
 * Autor:		menymp
 */

uint8_t HAL_STM32_CDC_Transmit_FS(uint8_t* Buf, uint16_t Len)
{
	return CDC_Transmit_FS(Buf,Len);
}

/*
 * name:		HAL_STM32_VCP_retrieveInputData
 *
 * description:	retrive data if existing bytes to read
 *
 * globals:		NONE
 *
 * parameters:	buf		input buffer
 * 				Len		len of input buffer
 *
 * returns:		battery voltage
 *
 * Autor:		menymp
 */

uint8_t HAL_STM32_VCP_retrieveInputData(uint8_t* Buf, uint32_t *Len)
{
	return VCP_retrieveInputData(Buf, Len);
}
