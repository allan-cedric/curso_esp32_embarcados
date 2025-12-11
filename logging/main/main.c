#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "MyModule";

void foo(int error)
{
    ESP_LOGD(TAG, "Begin foo()");
    if (error == 0)
    {
        ESP_LOGI(TAG, "foo() success");
    }
    else if (error == 1)
    {
        ESP_LOGE(TAG, "foo() error");
        ESP_LOGW(TAG, "foo(): for more info. use verbose or debug log level");
    }
    ESP_LOGV(TAG, "foo() received error=%i", error);
    ESP_LOGD(TAG, "End foo()");
}

void app_main(void)
{
    esp_log_level_set(TAG, ESP_LOG_VERBOSE);

    int error = 0;
    while (1)
    {
        foo(error);
        error ^= 1;
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}