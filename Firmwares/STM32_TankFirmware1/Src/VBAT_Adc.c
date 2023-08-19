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

	HAL_ADC_Start(adcHandle); /*suposedly on continuous conversion mode*/
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

/*
 * name:		updateVbatVoltage
 *
 * description:	updates voltage periodicaly and averages with previons measures
 * 				to reduce noise variation, this is supposed to happen each 300ms
 *
 * globals:		dc_motors		where to store motor register information
 *
 * parameters:	adcHandle	handle for adc port
 *
 * returns:		new voltage value
 *
 * Autor:		menymp
 */

float updateVbatVoltage(ADC_HandleTypeDef * adcHandle)
{
	static float samples[ADC_FILTER_LEN] = {0.0, 0.0, 0.0};
	int index = 0;
	float sum = 0.0;

	swapSamples(samples, sizeof(samples), readBatteryVoltage(adcHandle));

	for(index = 0; index < sizeof(samples); index ++)
		sum += samples[index];

	return (sum / sizeof(samples));
}

/*
 * name:		swapArraySamples
 *
 * description:	swaps every sample one place, discards the last measure and updates
 * 				the newer value sample
 *
 * globals:		dc_motors		where to store motor register information
 *
 * parameters:	array		address of the first element in array
 * 				len			len of the targer array
 * 				newSample	new value for the first position
 *
 * returns:		NOTHING
 *
 * Autor:		menymp
 */

void swapSamples(float * array, uint16_t len, float newSample)
{
	int index = 0;

	for(index = 0; index < len - 1; index ++)
	{
		array[index] = array[index + 1];
	}
	array[len - 1] = newSample;
}
