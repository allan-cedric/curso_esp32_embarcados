#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"

#include "esp_log.h"

static const char *TAG1 = "TASK APP MAIN: ";
static const char *TAG2 = "TASK BLINK: ";
static const char *TAG3 = "TASK COUNTER: ";

#define LED1_PIN 14
#define LED2_PIN 21
#define BLINK1_PERIOD 200
#define BLINK2_PERIOD 500

typedef struct 
{
    int led_pin;
    int blink_period;
}led_t;

led_t led1 = {
    LED1_PIN, BLINK1_PERIOD
};

led_t led2 = {
    LED2_PIN, BLINK2_PERIOD
};


void vTaskBlink(void *pvParameters);
void vTaskCounter(void *pvParameters);
TaskHandle_t xTaskBlink1Handle = NULL;
TaskHandle_t xTaskBlink2Handle = NULL;
TaskHandle_t xTaskCounterHandle = NULL;

int valor = 123;

void app_main(void)
{
    ESP_LOGI(TAG1, "Iniciando o programa");

    // xTaskCreate(
    //     vTaskBlink, // funcao da task
    //     "TaskBlink1",
    //     2048,
    //     (void *)&led1,
    //     1,
    //     &xTaskBlink1Handle
    // );
    xTaskCreatePinnedToCore(
        vTaskBlink, // funcao da task
        "TaskBlink1",
        2048,
        (void *)&led1,
        1,
        &xTaskBlink1Handle,
        0
    );

    // xTaskCreate(
    //     vTaskBlink, // funcao da task
    //     "TaskBlink2",
    //     2048,
    //     (void *)&led2,
    //     1,
    //     &xTaskBlink2Handle
    // );
    xTaskCreatePinnedToCore(
        vTaskBlink, // funcao da task
        "TaskBlink2",
        2048,
        (void *)&led2,
        1,
        &xTaskBlink2Handle,
        0
    );

    // xTaskCreate(
    //     vTaskCounter, // funcao da task
    //     "TaskCounter",
    //     1700,
    //     (void *)valor,
    //     2,
    //     &xTaskCounterHandle
    // );
    xTaskCreatePinnedToCore(
        vTaskCounter, // funcao da task
        "TaskCounter",
        2048,         // precisa ser 2048
        (void *)valor,
        2,
        &xTaskCounterHandle,
        1
    );

    ESP_LOGD(TAG1, "Iniciando a [%s]. CORE[%d]", pcTaskGetName(NULL), xPortGetCoreID());
    // int i = 0;
    while(1)
    {
        // ESP_LOGI(TAG1, "TASK Main - Counter: %d", i);
        // i++;
        // vTaskDelay(pdMS_TO_TICKS(1000));

        ESP_LOGI(TAG1, "Task Blink1 High Water Mark: %d", uxTaskGetStackHighWaterMark(xTaskBlink1Handle));
        ESP_LOGI(TAG1, "Task Blink2 High Water Mark: %d", uxTaskGetStackHighWaterMark(xTaskBlink2Handle));
        ESP_LOGI(TAG1, "Task Counter High Water Mark: %d", uxTaskGetStackHighWaterMark(xTaskCounterHandle));
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

void vTaskBlink(void *pvParameters)
{
    int led_pin = ((led_t*)pvParameters)->led_pin;
    int blink_period = ((led_t*)pvParameters)->blink_period;
    bool status = false;

    // ESP_LOGI(TAG2, "Iniciando a task Blink");
    ESP_LOGD(TAG1, "Iniciando a [%s]. CORE[%d]", pcTaskGetName(NULL), xPortGetCoreID());
    gpio_set_direction(led_pin, GPIO_MODE_OUTPUT);

    while(1)
    {
        gpio_set_level(led_pin, status ^= 1);
        ESP_LOGI(TAG2, "[%s] LED[%d]: %d", pcTaskGetName(NULL),led_pin, status);
        vTaskDelay(pdMS_TO_TICKS(blink_period));
    }
}

void vTaskCounter(void *pvParameters)
{
    int i = (int)pvParameters;

    // ESP_LOGI(TAG3, "Iniciando a task Counter");
    ESP_LOGD(TAG1, "Iniciando a [%s]. CORE[%d]", pcTaskGetName(NULL), xPortGetCoreID());

    while(1)
    {
        ESP_LOGI(TAG3, "Counter: %d", i++);

        // if(i == 1010)
        // {
        //     // vTaskDelete(xTaskBlinkHandle);
        //     vTaskSuspend(xTaskBlinkHandle);
        //     gpio_set_level(LED_PIN, 0);
        //     ESP_LOGI(TAG3, "TASK Blink suspensa");
        // }
        // else if (i == 1015)
        // {
        //     // xTaskCreate(
        //     //     vTaskBlink, // funcao da task
        //     //     "TaskBlink",
        //     //     2048,
        //     //     NULL,
        //     //     1,
        //     //     &xTaskBlinkHandle
        //     // );

        //     // i = 1000;
        //     // vTaskDelete(NULL);

        //     vTaskResume(xTaskBlinkHandle);
        //     i = 1000;
        //     ESP_LOGI(TAG3, "TASK Blink resumed");
        // }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}