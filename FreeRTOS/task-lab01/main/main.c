#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"

#include "esp_log.h"

#define DEFAULT_COUNTER 0
#define DEFAULT_LIMIT 100

#define LED_PIN 21
#define BUTTON_PIN 2

static const char *TAG1 = "TASK APP_MAIN: ";

void vTaskCounter(void *pvParameters);
TaskHandle_t TaskCounterHandle = NULL;

void vTaskMonitor(void *pvParameters);
TaskHandle_t TaskMonitorHandle = NULL;

int g_counter = DEFAULT_COUNTER;

void app_main(void)
{
    ESP_LOGI(TAG1, "[%s] Starting task", pcTaskGetName(NULL));

    int button_state = 1;
    gpio_reset_pin(BUTTON_PIN);
    gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_PIN, GPIO_PULLUP_ONLY);

    xTaskCreate(
        vTaskCounter,
        "TaskCounter",
        2048,
        (void *) DEFAULT_COUNTER,
        1,
        &TaskCounterHandle
    );

    xTaskCreate(
        vTaskMonitor,
        "TaskMonitor",
        2048,
        (void *) DEFAULT_LIMIT,
        1,
        &TaskMonitorHandle
    );

    while(1)
    {
        int read_button = gpio_get_level(BUTTON_PIN);
        if(read_button != button_state)
        {
            button_state = read_button;
            if(button_state == 0)
            {
                if(TaskMonitorHandle != NULL) 
                {
                    ESP_LOGI(TAG1, "[%s] Deleting task: %s", pcTaskGetName(NULL), pcTaskGetName(TaskMonitorHandle));
                    vTaskDelete(TaskMonitorHandle);
                    TaskMonitorHandle = NULL;
                    ESP_LOGI(TAG1, "[%s] Done!", pcTaskGetName(NULL));
                }
                else
                {
                    ESP_LOGW(TAG1, "[%s] Task already deleted!", pcTaskGetName(NULL));
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void vTaskCounter(void *pvParameters)
{
    static const char *TAG2 = "TASK COUNTER: ";
    ESP_LOGI(TAG2, "[%s] Starting task", pcTaskGetName(NULL));

    g_counter = (int)pvParameters;

    while(1)
    {
        ESP_LOGI(TAG2, "[%s] Counter: %i", pcTaskGetName(NULL), g_counter);
        g_counter++;
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}

void vTaskMonitor(void *pvParameters)
{
    static const char *TAG3 = "TASK MONITOR: ";
    ESP_LOGI(TAG3, "[%s] Starting task", pcTaskGetName(NULL));

    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    int counter_limit = (int)pvParameters;

    while (1)
    {
        ESP_LOGI(TAG3, "[%s] Get counter status: %i", pcTaskGetName(NULL), g_counter);
        
        if(g_counter > counter_limit)
        {
            ESP_LOGI(TAG3, "[%s] Rebooting: [%s]", pcTaskGetName(NULL), pcTaskGetName(TaskCounterHandle));

            gpio_set_level(LED_PIN, 1);
            vTaskDelay(pdMS_TO_TICKS(500));
            gpio_set_level(LED_PIN, 0);
            vTaskDelay(pdMS_TO_TICKS(500));

            vTaskDelete(TaskCounterHandle);
            xTaskCreate(
                vTaskCounter,
                "TaskCounter",
                2048,
                (void *) DEFAULT_COUNTER,
                1,
                &TaskCounterHandle
            );
        }

        vTaskDelay(pdMS_TO_TICKS(250));
    }
}
