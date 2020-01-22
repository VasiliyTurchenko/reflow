/** @file temerature_measurement.c
 *  @brief
 *
 *  @author Vasiliy Turchenko
 *  @bug
 *  @date 20-Jan-2020
 */

#include "cmsis_os.h"
#include "adc.h"
#include "temerature_measurement.h"
#include "framebuffer.h"
#include "kbd_task.h"
#include "ftoa.h"
#include "parameters_storage.h"
#include "logging.h"
#include "sys_helpers.h"

/* samples array size for one channel */
#define ADC_CHANNEL_ARRAY_SIZE 64U
#define ADC_NUM_CHANNELS 2U

static tc_cal_data_t *tc1_cal_data = NULL;
static tc_cal_data_t *tc2_cal_data = NULL;

/* array to hold adc sampled data
 * odd indexes are for channel 0 samples
 * even indexes are for channel 1 samples
*/
static const size_t DMA_arr_size = ADC_CHANNEL_ARRAY_SIZE * ADC_NUM_CHANNELS;
uint16_t samples[ADC_NUM_CHANNELS * ADC_CHANNEL_ARRAY_SIZE] __aligned(4);

/**
 * @brief get_average_adc_meas
 * @return
 */
averaged_data_t get_average_adc_meas(void)
{
	averaged_data_t retVal = { 0U, 0U };
	uint32_t data0 = 0U;
	uint32_t data1 = 0U;
	size_t i = 0U;
	while (i < DMA_arr_size) {
		data0 += samples[i];
		data1 += samples[i + 1];
		i += 2U;
	}

	retVal.average_val0 = (uint16_t)(data0 / ADC_CHANNEL_ARRAY_SIZE);
	retVal.average_val1 = (uint16_t)(data1 / ADC_CHANNEL_ARRAY_SIZE);

	return retVal;
}

/**
 * @brief get_tc1_temperature
 * @return
 */
averaged_data_t get_temperature(void)
{
	averaged_data_t temp = get_average_adc_meas();
	float t0 = 0.0F;
	float t1 = 0.0F;

	if (tc1_cal_data != NULL) {
		if (tc1_cal_data->valid) {
			/* first sensor */
			t0 = ((float)temp.average_val0 - tc1_cal_data->B) *
			     tc1_cal_data->A;
		} else {
			t0 = (float)temp.average_val0;
		}
	}
	if (tc2_cal_data != NULL) {
		if (tc2_cal_data->valid) {
			/* second sensor */
			t1 = ((float)temp.average_val1 - tc2_cal_data->B) *
			     tc2_cal_data->A;
		} else {
			t1 = (float)temp.average_val1;
		}
	}
	temp.average_val0 = (uint16_t)t0;
	temp.average_val1 = (uint16_t)t1;

	return temp;
}

ErrorStatus start_ADC(void)
{
	ErrorStatus retVal = ERROR;
	//	SET_BIT(hadc1.Instance->CR2, ADC_CR2_TSVREFE);

	if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&samples, DMA_arr_size) ==
	    HAL_OK) {
		retVal = SUCCESS;
	}
	tc1_cal_data = get_cfg(CFG_TYPE_TC_CAL1);
	tc2_cal_data = get_cfg(CFG_TYPE_TC_CAL2);

	return retVal;
}

/**
 * @brief calibrate_TC Thermocouple calibration wizard
 * @param p_tc
 * @return
 */
static ErrorStatus calibrate_TC(tc_cal_data_t *p_tc)
{
	ErrorStatus retVal = ERROR;
	fast_clear_screen();
	uint8_t sens_num = 0U;
	gotoXY(0U, 8U);
	zprint("Ready to start\n", NORM);
	if (p_tc == tc1_cal_data) {
		zprint("TC1", NORM);
		sens_num = 1U;
	} else {
		zprint("TC2", NORM);
		sens_num = 2U;
	}
	zprint(" calibration?\n", NORM);
	zprint("Press ENTER\n", NORM);
	zprint("or ESC\n", NORM);

	clear_key_buffer();
	key_code_t key;
	while (true) {
		key = get_key();
		if ((key == ESC_KEY_RELEASED) || (key == ENTER_KEY_RELEASED)) {
			break;
		}
		vTaskDelay(pdMS_TO_TICKS(10U));
	}

	if (key == ESC_KEY_PRESSED) {
		goto fExit;
	}

	/* next step */
	fast_clear_screen();
	gotoXY(0U, 8U);
	zprint("Cool TC to 0C\n", NORM);
	zprint("Press ENTER\n", NORM);
	zprint("when ready..\n", NORM);
	wait_for_key(ENTER_KEY_RELEASED);

	/* measure at 0C */
	averaged_data_t meas0;
	sys_helpers_delay_ms(200U);
	meas0 = get_average_adc_meas();

	fast_clear_screen();
	gotoXY(0U, 8U);
	zprint("Heat TC to 100C\n", NORM);
	zprint("Press ENTER\n", NORM);
	zprint("when ready..\n", NORM);
	wait_for_key(ENTER_KEY_RELEASED);

	/* measure at 100C */
	averaged_data_t meas100;
	sys_helpers_delay_ms(200U);
	meas100 = get_average_adc_meas();

	/* y = A*X + B */
	float B;
	float A;

	fast_clear_screen();
	gotoXY(0U, 8U);

	if (sens_num == 1U) {
		B = (float)(meas0.average_val0);
		A = (float)(100.0F / meas100.average_val0 - meas0.average_val0);
		zprint("TC1", NORM);
	} else {
		B = (float)(-meas0.average_val1);
		A = (float)(100.0F / meas100.average_val1 - meas0.average_val1);
		zprint("TC2", NORM);
	}
	zprint(" calibrated.\n", NORM);

	p_tc->A = A;
	p_tc->B = B;
	p_tc->valid = true;

	char buf[10];
	log_xprintf(MSG_LEVEL_INFO, "A = %s", ftoa((double)A, (char *)&buf, 5));
	log_xprintf(MSG_LEVEL_INFO, "B = %s", ftoa((double)B, (char *)&buf, 5));

	if (save_cfg_pool() == SUCCESS) {
		sys_helpers_delay_ms_and_reboot(1000U);
	}

fExit:
	return retVal;
}

/**
 * @brief calibrate_TC1 starts thermocouple 1 calibration
 * @return ERROR if unsuccessfull, restart if OK
 */
ErrorStatus calibrate_TC1(void)
{
	return calibrate_TC(tc1_cal_data);
}

/**
 * @brief calibrate_TC1 starts thermocouple 1 calibration
 * @return ERROR if unsuccessfull, restart if OK
 */
ErrorStatus calibrate_TC2(void)
{
	return calibrate_TC(tc2_cal_data);
}
