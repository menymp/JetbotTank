/*
 * HAL_STM32_USB.C
 *
 *  Created on: 8 jul. 2019
 *      Author: TOSHIBA
 */

#include "HAL_STM32_USB.h"

void HAL_STM32_USB_DEVICE_Init(void)
{
	MX_USB_DEVICE_Init();
}

uint8_t HAL_STM32_CDC_Transmit_FS(uint8_t* Buf, uint16_t Len)
{
	return CDC_Transmit_FS(Buf,Len);
}

uint8_t HAL_STM32_VCP_retrieveInputData(uint8_t* Buf, uint32_t *Len)
{
	return VCP_retrieveInputData(Buf, Len);
}
