/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>

#ifdef CONFIG_SUM_PRINT
#include "sum_printk.h"
#endif
#ifdef CONFIG_SUM_LOG
#include "sum_log.h"
#endif

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   2000

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
#define led5180_NODE DT_ALIAS(led5180)
#define BUTTON_NODE DT_ALIAS(button5180)

#define BME280_NODE DT_NODELABEL(bme280)

static const struct i2c_dt_spec bme280 = I2C_DT_SPEC_GET(BME280_NODE);

#define BME280_CTRL_MEAS 0xF4
#define BME280_TEMP_MSB  0xFA

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(led5180_NODE, gpios);
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(BUTTON_NODE, gpios);

static int32_t compensate_temperature(int32_t adc_T, uint16_t dig_T1, int16_t dig_T2, int16_t dig_T3)
{
    int32_t var1;
    int32_t var2;
    int32_t t_fine;
    int32_t T;

    var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;

    t_fine = var1 + var2;

    T = (t_fine * 5 + 128) >> 8;

    return T;
}

int main(void)
{
	int ret;
	bool led_state = true;

	if (!gpio_is_ready_dt(&led)) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}

	if (!gpio_is_ready_dt(&button)) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
	if (ret < 0) {
		return 0;
	}

	int a = 2;
    int b = 3;
	#ifdef CONFIG_SUM_PRINT
		sum_printk(a, b);
	#endif
	#ifdef CONFIG_SUM_LOG
		sum_log(a, b);
	#endif

	bool last_state = false;

	uint8_t ctrl_meas[2] = {
		BME280_CTRL_MEAS,
		0x27
	};

	int i2c_ret = i2c_write_dt(&bme280, ctrl_meas, sizeof(ctrl_meas));


	if (i2c_ret != 0) {
		printk("fail\n");
	}

	uint8_t calib[6];

	i2c_ret = i2c_burst_read_dt(
		&bme280,
		0x88,
		calib,
		sizeof(calib)
	);

	if (i2c_ret != 0) {
		printk("failed calibration\n");
	}

	uint16_t dig_T1 = ((uint16_t)calib[1] << 8) | calib[0];
	int16_t dig_T2 = (int16_t)(((uint16_t)calib[3] << 8) | calib[2]);
	int16_t dig_T3 = (int16_t)(((uint16_t)calib[5] << 8) | calib[4]);

	uint8_t temp_data[3];

	i2c_ret = i2c_burst_read_dt(
		&bme280,
		BME280_TEMP_MSB,
		temp_data,
		sizeof(temp_data)
	);

	if (i2c_ret != 0) {
		printk("Failed to read temperature\n");
	}

	int32_t raw_temp = ((int32_t)temp_data[0] << 12) | ((int32_t)temp_data[1] << 4) | ((int32_t)temp_data[2] >> 4);
	int32_t temperature = compensate_temperature(raw_temp, dig_T1, dig_T2, dig_T3);

	printk("Temperature: %d.%02d C\n", temperature / 100, temperature % 100);

	while (1) {
		
		int button_state = gpio_pin_get_dt(&button);
		if (button_state < 0) {
			return 0;
		}

		if (button_state && !last_state) {

			ret = gpio_pin_toggle_dt(&led);
			if (ret < 0) {
				return 0;
			}

			led_state = !led_state;
			printf("LED state: %s\n", led_state ? "ON" : "OFF");
		}
		last_state = button_state;
		k_msleep(20);
	}
	return 0;
}

