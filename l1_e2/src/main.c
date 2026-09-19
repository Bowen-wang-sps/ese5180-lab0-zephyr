/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
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


/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(led5180_NODE, gpios);
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(BUTTON_NODE, gpios);


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

