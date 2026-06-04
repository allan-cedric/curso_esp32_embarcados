#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_sleep.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "driver/gpio.h"

#include "uart_wakeup.h"

#define GPIO_WAKEUP_PIN 4
#define TIMER_WAKEUP_US ( 5000000 )

static const char *TAG = "Light Sleep Test";

void app_main(void)
{
    gpio_config_t config = {
        .pin_bit_mask = BIT64(GPIO_WAKEUP_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_down_en = true,
        .pull_up_en = false,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&config);

    // ESP_LOGI(TAG, "Enabling GPIO wakeup");
    // gpio_wakeup_enable(GPIO_WAKEUP_PIN, GPIO_INTR_LOW_LEVEL);
    // esp_sleep_enable_gpio_wakeup();

    // ESP_LOGI(TAG, "Enabling timer wakeup, %d us", TIMER_WAKEUP_US);
    // esp_sleep_enable_timer_wakeup(TIMER_WAKEUP_US);

    ESP_LOGI(TAG, "Enabling UART wakeup");
    example_register_uart_wakeup();

    ESP_LOGI(TAG, "Waiting for GPIO%d to go high...", GPIO_WAKEUP_PIN);
    while (gpio_get_level(GPIO_WAKEUP_PIN) == 0)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    ESP_LOGI(TAG, "gpio wakeup source is ready");
    vTaskDelay(pdMS_TO_TICKS(100));

    while (1)
    {
        /* code */
        ESP_LOGI(TAG, "Entering light sleep");
        vTaskDelay(pdMS_TO_TICKS(100));

        int64_t t_before_us = esp_timer_get_time();
        esp_light_sleep_start();
        int64_t t_after_us = esp_timer_get_time();
        
        const char *wakeup_reason;
        switch (esp_sleep_get_wakeup_cause())
        {
            case ESP_SLEEP_WAKEUP_UART:
                wakeup_reason = "uart";
                vTaskDelay(pdMS_TO_TICKS(1000));
                break;

            case ESP_SLEEP_WAKEUP_TIMER:
                wakeup_reason = "timer";
                break;

            case ESP_SLEEP_WAKEUP_GPIO:
                wakeup_reason = "gpio";
                break;
            
            default:
                wakeup_reason = "other";
                break;
        }

        ESP_LOGI(TAG, "Returned from light sleep, reason: %s, t=%lld ms, slept for %lld ms\n",
                    wakeup_reason, t_after_us/1000, (t_after_us-t_before_us)/1000);

        switch (esp_sleep_get_wakeup_cause())
        {
            case ESP_SLEEP_WAKEUP_TIMER:
                ESP_LOGI(TAG, "Waiting for 5 seconds to allow user to measure current consumption...");
                vTaskDelay(pdMS_TO_TICKS(5000));
                break;

            case ESP_SLEEP_WAKEUP_GPIO:
                ESP_LOGI(TAG, "Waiting for GPIO%d to go high...", GPIO_WAKEUP_PIN);
                while (gpio_get_level(GPIO_WAKEUP_PIN) == 0)
                {
                    vTaskDelay(pdMS_TO_TICKS(10));
                }
                break;
            
            default:
                break;
        }
    }
}
