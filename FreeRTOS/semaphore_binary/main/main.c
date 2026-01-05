#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "driver/gpio.h"

#include "esp_log.h"

// pin mapping
#define LED1_PIN 21   // GPIO21
#define LED2_PIN 33   // GPIO21
#define LED3_PIN 14   // GPIO14
#define LED4_PIN 13   // GPIO13
#define BUTTON1_PIN 2 // GPIO2
#define BUTTON2_PIN 3 // GPIO3

static const char *TAG = "Semaphore: "; // Logging tag

SemaphoreHandle_t xBinarySemaphore = NULL;

void led_task(void *pvParameters);

static void IRAM_ATTR gpio_isr_handler(void *args)
{
    BaseType_t xHigherPriorityTaskWoken = pdTRUE;
    if(BUTTON1_PIN == (uint32_t)args)
    {
        xSemaphoreGiveFromISR(xBinarySemaphore, &xHigherPriorityTaskWoken);
    }
}

void app_main(void)
{
    gpio_config_t io_conf = {}; // GPIO config structure

    // Configure LED
    io_conf.pin_bit_mask = (1ULL << LED1_PIN); // Bitmask to select the GPIOs you want to configure
    io_conf.mode = GPIO_MODE_OUTPUT;           // Set the GPIO mode
    io_conf.pull_up_en = 0;                    // Disable pull-up mode
    io_conf.pull_down_en = 0;                  // Disable pull-down mode
    io_conf.intr_type = GPIO_INTR_DISABLE;     // Disable interrupts
    gpio_config(&io_conf);                     // Configure GPIO with the given settings

    // Configure button
    io_conf.pin_bit_mask = (1ULL << BUTTON1_PIN); // Bitmask to select the GPIOs you want to configure
    io_conf.mode = GPIO_MODE_INPUT;               // Set the GPIO mode
    io_conf.pull_up_en = 1;                       // Enable pull-up mode
    io_conf.pull_down_en = 0;                     // Disable pull-down mode
    io_conf.intr_type = GPIO_INTR_NEGEDGE;        // Interrupt on negative edge
    gpio_config(&io_conf);                        // Configure GPIO with the given settings

    // install gpio isr service and add isr handler for button1 and button2
    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);                           // install gpio isr service
    gpio_isr_handler_add(BUTTON1_PIN, gpio_isr_handler, (void *)BUTTON1_PIN); // add isr handler for button1

    xBinarySemaphore = xSemaphoreCreateBinary();

    // Create a task to wait for the semaphore
    xTaskCreate(&led_task, "led_task", configMINIMAL_STACK_SIZE + 1024, NULL, 5, NULL);

    vTaskDelete(NULL);
}

void led_task(void *pvParameters)
{
    int led_status = 0;

    while (1)
    {
        if(xSemaphoreTake(xBinarySemaphore, portMAX_DELAY) == pdTRUE)
        {
            gpio_set_level(LED1_PIN, led_status ^= 1);
            ESP_LOGI(TAG, "LED: %d", led_status);
        }
    }
}
