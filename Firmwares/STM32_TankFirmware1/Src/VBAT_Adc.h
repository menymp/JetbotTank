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
#define VBAT_SCALE_DIV		91.266

#define VBAT_MAX_VOLTAGE	12.3 /*acorded maximun voltage for a 4.1v x3*/
#define VBAT_MIN_VOLTAGE	9.6	/*acorded minimun safe voltage 3.2v x3*/

#define ADC_FILTER_LEN		3

float readBatteryVoltage(ADC_HandleTypeDef * adcHandle);
int getBatteryCharge(float voltage);
void swapSamples(float * array, uint16_t len, float newSample);
float updateVbatVoltage(ADC_HandleTypeDef * adcHandle);

#endif /* VBAT_ADC_H_ */
