#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"

#include "esp_log.h"

// pin mapping
#define LED1_PIN 21   // GPIO21
#define LED2_PIN 33   // GPIO33
#define LED3_PIN 14   // GPIO14
#define LED4_PIN 13   // GPIO13
#define LED5_PIN 12   // GPIO12
#define BUTTON1_PIN 2 // GPIO2
#define BUTTON2_PIN 3 // GPIO3
#define BUTTON3_PIN 4 // GPIO4
#define BUTTON4_PIN 5 // GPIO5
#define BUTTON5_PIN 6 // GPIO6
#define BUTTON6_PIN 7 // GPIO7

static const char *TAG = "QUEUE-ISR-LAB02"; // tag for ESP_LOGx

QueueHandle_t gpio_evt_queue = NULL;

void buttonTask(void *pvParameters);

static void IRAM_ATTR gpio_isr_handler(void *args)
{
    uint32_t gpio_num = (uint32_t)args;
    BaseType_t xHigherPriorityTaskWoken = pdTRUE;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, &xHigherPriorityTaskWoken);
}

void app_main(void)
{
    gpio_config_t io_conf = {}; // GPIO config structure

    // Configure LED
    io_conf.pin_bit_mask = (1ULL << LED1_PIN) | (1ULL << LED2_PIN) | (1ULL << LED3_PIN) | (1ULL << LED4_PIN) | (1ULL << LED5_PIN); // Bitmask to select the GPIOs you want to configure
    io_conf.mode = GPIO_MODE_OUTPUT;                                                                                               // Set the GPIO mode
    io_conf.pull_up_en = 0;                                                                                                        // Disable pull-up mode
    io_conf.pull_down_en = 0;                                                                                                      // Disable pull-down mode
    io_conf.intr_type = GPIO_INTR_DISABLE;                                                                                         // Disable interrupts
    gpio_config(&io_conf);                                                                                                         // Configure GPIO with the given settings

    // Configure button
    io_conf.pin_bit_mask = (1ULL << BUTTON1_PIN) | (1ULL << BUTTON2_PIN) | (1ULL << BUTTON3_PIN) | (1ULL << BUTTON4_PIN) | (1ULL << BUTTON5_PIN) | (1ULL << BUTTON6_PIN); // Bitmask to select the GPIOs you want to configure
    io_conf.mode = GPIO_MODE_INPUT;                                                                                                                                       // Set the GPIO mode
    io_conf.pull_up_en = 1;                                                                                                                                               // Enable pull-up mode
    io_conf.pull_down_en = 0;                                                                                                                                             // Disable pull-down mode
    io_conf.intr_type = GPIO_INTR_NEGEDGE;                                                                                                                                // Interrupt on negative edge
    gpio_config(&io_conf);                                                                                                                                                // Configure GPIO with the given settings

    // install gpio isr service and add isr handler for button1 and button2
    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);                           // install gpio isr service
    gpio_isr_handler_add(BUTTON1_PIN, gpio_isr_handler, (void *)BUTTON1_PIN); // add isr handler for button1
    gpio_isr_handler_add(BUTTON2_PIN, gpio_isr_handler, (void *)BUTTON2_PIN); // add isr handler for button2
    gpio_isr_handler_add(BUTTON3_PIN, gpio_isr_handler, (void *)BUTTON3_PIN); // add isr handler for button3
    gpio_isr_handler_add(BUTTON4_PIN, gpio_isr_handler, (void *)BUTTON4_PIN); // add isr handler for button4
    gpio_isr_handler_add(BUTTON5_PIN, gpio_isr_handler, (void *)BUTTON5_PIN); // add isr handler for button5
    gpio_isr_handler_add(BUTTON6_PIN, gpio_isr_handler, (void *)BUTTON6_PIN); // add isr handler for button6

    gpio_evt_queue = xQueueCreate(6, sizeof(uint32_t));
    xTaskCreate(
        buttonTask,
        "buttonTask",
        2048,
        NULL,
        2,
        NULL);

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(500)); // delay 500ms
    }
}

void buttonTask(void *pvParameters)
{
    uint32_t gpio_num;
    uint8_t led1 = 0, led2 = 0, led3 = 0;
    uint8_t led4 = 0, led5 = 0;
    uint8_t led_global = 0;
    TickType_t last_button_press = 0;

    while (1)
    {
        xQueueReceive(gpio_evt_queue, &gpio_num, portMAX_DELAY);
        ESP_LOGI(TAG, "GPIO[%li] intr \n", gpio_num);

        TickType_t current_time = xTaskGetTickCount();
        if (current_time - last_button_press >= pdMS_TO_TICKS(250))
        {
            last_button_press = current_time;
        
            switch (gpio_num)
            {
            case BUTTON1_PIN:
                gpio_set_level(LED1_PIN, led1 ^= 1);
                break;
            case BUTTON2_PIN:
                gpio_set_level(LED2_PIN, led2 ^= 1);
                break;
            case BUTTON3_PIN:
                gpio_set_level(LED3_PIN, led3 ^= 1);
                break;
            case BUTTON4_PIN:
                gpio_set_level(LED4_PIN, led4 ^= 1);
                break;
            case BUTTON5_PIN:
                gpio_set_level(LED5_PIN, led5 ^= 1);
                break;
            case BUTTON6_PIN:
                gpio_set_level(LED1_PIN, led1 = 1);
                gpio_set_level(LED2_PIN, led2 = 1);
                gpio_set_level(LED3_PIN, led3 = 1);
                gpio_set_level(LED4_PIN, led4 = 1);
                gpio_set_level(LED5_PIN, led5 = 1);
                break;
            default:
                ESP_LOGE(TAG, "Unknown BUTTON PIN: %li", gpio_num);
                break;
            }
        }
    }
}
