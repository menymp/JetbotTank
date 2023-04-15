/*
 * HAL_STM32_USB.h
 *
 *  Created on: 8 jul. 2019
 *      Author: TOSHIBA
 */

#ifndef HAL_STM32_USB_H_
#define HAL_STM32_USB_H_

#include "usb_device.h"
#include "usbd_cdc_if.h"

void HAL_STM32_USB_DEVICE_Init(void);
uint8_t HAL_STM32_CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);
uint8_t HAL_STM32_VCP_retrieveInputData(uint8_t* Buf, uint32_t *Len);

#endif /* HAL_STM32_USB_H_ */
