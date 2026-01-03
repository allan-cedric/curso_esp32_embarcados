#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"

#include "esp_log.h"

#define LED 14

QueueHandle_t xQueue;

TaskHandle_t xTask1Handle;
TaskHandle_t xTask2Handle;

void vTask1(void* pvParameters);
void vTask2(void* pvParameters);

static const char* TAG = "TESTE FILA";

void app_main(void)
{
    bool status = false;

    xQueue = xQueueCreate(5, sizeof(int));

    xTaskCreate(
        vTask1,
        "Task1",
        configMINIMAL_STACK_SIZE + 1024,
        NULL,
        1,
        &xTask1Handle
    );

    xTaskCreate(
        vTask2,
        "Task2",
        configMINIMAL_STACK_SIZE + 1024,
        NULL,
        1,
        &xTask2Handle
    );

    gpio_reset_pin(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    while (1)
    {
        gpio_set_level(LED, status ^= 1);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void vTask1(void* pvParameters)
{
    int count = 0;

    while(1)
    {
        if(count < 10)
        {
            xQueueSend(xQueue, &count, portMAX_DELAY);
            count++;
        }
        else
        {
            count = 0;
            vTaskDelay(pdMS_TO_TICKS(5000));
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void vTask2(void* pvParameters)
{
    int received_count;

    while(1)
    {
        if(xQueueReceive(xQueue, &received_count, pdMS_TO_TICKS(1000)) == pdTRUE)
        {
            ESP_LOGI(TAG, "Received: %d", received_count);
        }
        else
        {
            ESP_LOGI(TAG, "TIMEOUT");
        }
    }
}
