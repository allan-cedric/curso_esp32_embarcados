#include <stdio.h>
#include <string.h>

#include "esp_heap_caps.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"

static const char *TAG = "RAM Memory";

int x = 42;
char y[50];

void app_main(void)
{
    printf("x = %d y = %s", x, y);

    ESP_LOGI(TAG, "DRAM(xPortGetFreeHeapSize) = %d bytes", xPortGetFreeHeapSize());

    int DRAM = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    ESP_LOGI(TAG, "DRAM = %d bytes", DRAM);

    int IRAM = heap_caps_get_free_size(MALLOC_CAP_32BIT) - heap_caps_get_free_size(MALLOC_CAP_8BIT);
    ESP_LOGI(TAG, "IRAM = %d bytes", IRAM);
}
