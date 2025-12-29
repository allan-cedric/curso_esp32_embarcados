#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"

#include "esp_log.h"

static const char *TAG1 = "TASK APP MAIN: ";
static const char *TAG2 = "TASK BLINK: ";
static const char *TAG3 = "TASK COUNTER: ";

#define LED_PIN 14

void vTaskBlink(void *pvParameters);
void vTaskCounter(void *pvParameters);
TaskHandle_t xTaskBlinkHandle = NULL;
TaskHandle_t xTaskCounterHandle = NULL;

void app_main(void)
{
    ESP_LOGI(TAG1, "Iniciando o programa");

    xTaskCreate(
        vTaskBlink, // funcao da task
        "TaskBlink",
        2048,
        NULL,
        1,
        &xTaskBlinkHandle
    );

    xTaskCreate(
        vTaskCounter, // funcao da task
        "TaskCounter",
        2048,
        NULL,
        2,
        &xTaskCounterHandle
    );

    int i = 0;
    while(1)
    {
        ESP_LOGI(TAG1, "TASK Main - Counter: %d", i);
        i++;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void vTaskBlink(void *pvParameters)
{
    bool status = false;

    ESP_LOGI(TAG2, "Iniciando a task Blink");
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    while(1)
    {
        gpio_set_level(LED_PIN, status ^= 1);
        ESP_LOGI(TAG2, "TASK Blink LED[%d]: %d", LED_PIN, status);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void vTaskCounter(void *pvParameters)
{
    int i = 1000;

    ESP_LOGI(TAG3, "Iniciando a task Counter");

    while(1)
    {
        ESP_LOGI(TAG3, "Counter: %d", i++);

        if(i == 1010)
        {
            // vTaskDelete(xTaskBlinkHandle);
            vTaskSuspend(xTaskBlinkHandle);
            gpio_set_level(LED_PIN, 0);
            ESP_LOGI(TAG3, "TASK Blink suspensa");
        }
        else if (i == 1015)
        {
            // xTaskCreate(
            //     vTaskBlink, // funcao da task
            //     "TaskBlink",
            //     2048,
            //     NULL,
            //     1,
            //     &xTaskBlinkHandle
            // );

            // i = 1000;
            // vTaskDelete(NULL);

            vTaskResume(xTaskBlinkHandle);
            i = 1000;
            ESP_LOGI(TAG3, "TASK Blink resumed");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}