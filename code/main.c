#include <stdio.h>
#include "gpio_util.h"

#define LED_GPIO 27
#define BUTTON_GPIO 21

int main()
{
	if (map_peripheral(&gpio) == -1) {
		printf("Fail\n");
	}

	INPUT_GPIO(BUTTON_GPIO);
	OUTPUT_GPIO(LED_GPIO);

	int button = 0;
	int ledOn = 0;
	
	while (1) {

		button = GPIO_READ(BUTTON_GPIO);

		if (button && !ledOn) {
			GPIO_SET = 1 << LED_GPIO;
			ledOn = 1;
		} else if (!button && ledOn) {
			GPIO_CLR = 1 << LED_GPIO;
			ledOn = 0;
		}
	}

	return 0;
}