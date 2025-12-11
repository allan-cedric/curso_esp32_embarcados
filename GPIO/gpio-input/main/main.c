#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"

#include "esp_log.h"

#define LED_PIN_1 14
#define LED_PIN_2 13
#define BUTTON_PIN 2

static const char *TAG = "BUTTON TEST";

void app_main(void)
{
    int button_state = 1;
    bool i = 0;
    gpio_reset_pin(LED_PIN_1);
    gpio_set_direction(LED_PIN_1, GPIO_MODE_OUTPUT);

    gpio_reset_pin(BUTTON_PIN);
    gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_PIN, GPIO_PULLUP_ONLY);

    ESP_LOGI(TAG, "INIT");

    while (1)
    {
        int new_state = gpio_get_level(BUTTON_PIN);

        if (new_state != button_state)
        {
            button_state = new_state;

            if (button_state == 0)
            {
                gpio_set_level(LED_PIN_1, i ^= 1);
                ESP_LOGI(TAG, "BUTTON PRESSED");
            }
            else
            {
                ESP_LOGI(TAG, "BUTTON RELEASED");
            }
        }
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}