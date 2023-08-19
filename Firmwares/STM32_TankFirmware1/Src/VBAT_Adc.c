/*
 * VBAT_Adc.c
 *
 *  Created on: 18/08/2023
 *      Author: menymp
 */
#include "VBAT_Adc.h"

/*
 * name:		readBatteryVoltage
 *
 * description:	reads and converts current battery voltage considering scaled factor
 *
 * globals:		NONE
 *
 * parameters:	adcHandle	handle for adc port
 *
 * returns:		battery voltage
 *
 * Autor:		menymp
 */

float readBatteryVoltage(ADC_HandleTypeDef * adcHandle)
{
	uint16_t raw;

	HAL_ADC_Start(adcHandle);
	HAL_ADC_PollForConversion(adcHandle, HAL_MAX_DELAY);
	raw = HAL_ADC_GetValue(adcHandle);
	/*convert to float value*/
	return raw * VBAT_SCALE_DIV * (VBAT_ADC_REF_VOLT / 65535);
}

/*
 * name:		getBatteryCharge
 *
 * description:	converts a li-ion 3s voltage to charge percent
 *
 * globals:		voltage	current voltage for the battery pack
 *
 * parameters:	NONE
 *
 * returns:		percent of charge left
 *
 * Autor:		menymp
 */

int getBatteryCharge(float voltage)
{
    /*read battery voltage per % */
    float output = 0.0;             /*output value*/

    /*for (int i = 0; i < 500; i++) //in the future, a timer can perform continuous samples
    {
        sum += analogRead(battery_pin);
        delayMicroseconds(1000);
    }
    voltage = sum / (float)500;
	//round value by two precision
    voltage = roundf(voltage * 100) / 100;
    */
    output = ((voltage - VBAT_MIN_VOLTAGE) / (VBAT_MAX_VOLTAGE - VBAT_MIN_VOLTAGE)) * 100;
    if (output < 100)
        return output;
    else
        return 100;
}
