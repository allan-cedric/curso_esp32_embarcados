#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// gpio definitions
#include "driver/gpio.h"

#include "esp_log.h"

#define LED_PIN_1 21
#define LED_PIN_2 33

static const char* MODULE = "gpio-output";

void app_main(void)
{
    // reset and configure pin as OUTPUT
    gpio_reset_pin(LED_PIN_1);
    gpio_set_direction(LED_PIN_1, GPIO_MODE_OUTPUT);

    gpio_reset_pin(LED_PIN_2);
    gpio_set_direction(LED_PIN_2, GPIO_MODE_OUTPUT);

    while (1)
    {
        // set level HIGH or LOW to the pin
        gpio_set_level(LED_PIN_1, 1);
        gpio_set_level(LED_PIN_2, 0);
        ESP_LOGI(MODULE, "LED %i ON; LED %i OFF", LED_PIN_1, LED_PIN_2);
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        gpio_set_level(LED_PIN_1, 0);
        gpio_set_level(LED_PIN_2, 1);
        ESP_LOGI(MODULE, "LED %i OFF; LED %i ON", LED_PIN_1, LED_PIN_2);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}