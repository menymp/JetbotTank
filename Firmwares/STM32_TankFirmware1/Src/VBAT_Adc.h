/*
 * VBAT_Adc.h
 *
 *  Created on: 18/08/2023
 *      Author: menymp
 */

#ifndef VBAT_ADC_H_
#define VBAT_ADC_H_

#include "adc.h"

#define VBAT_ADC_REF_VOLT	3.3
#define VBAT_SCALE_DIV		3.2 /*ToDo: read resistors and get the conversion factor*/

#define VBAT_MAX_VOLTAGE	12.6
#define VBAT_MIN_VOLTAGE	3.4

void readBatteryVoltage();
void getBatteryCharge();

#endif /* VBAT_ADC_H_ */
