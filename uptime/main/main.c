#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_timer.h"
#include "esp_log.h"

void app_main(void)
{
    int64_t uptime = esp_timer_get_time();

    ESP_LOGI(__func__, "Uptime: %lli ms",  uptime/1000);

    vTaskDelay(pdMS_TO_TICKS(5000));

    uptime = esp_timer_get_time();

    ESP_LOGI(__func__, "Uptime: %lli ms",  uptime/1000);
}
