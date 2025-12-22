#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/dac_oneshot.h"
#include "esp_log.h"

const static char* TAG = "dac_oneshot_example";

void app_main(void)
{
    uint8_t val = 0;

    dac_oneshot_handle_t chan0_handle;
    dac_oneshot_config_t chan0_cfg = {
        .chan_id = DAC_CHAN_0,
    };
    ESP_ERROR_CHECK(dac_oneshot_new_channel(&chan0_cfg, &chan0_handle));

    ESP_LOGI(TAG, "DAC oneshot example start.");

    while(1)
    {
        for(val = 0; val < 255; val++){
            ESP_ERROR_CHECK(dac_oneshot_output_voltage(chan0_handle, val));
            vTaskDelay(pdMS_TO_TICKS(1));
        }

        for(val = 255; val > 0; val--){
            ESP_ERROR_CHECK(dac_oneshot_output_voltage(chan0_handle, val));
            vTaskDelay(pdMS_TO_TICKS(1));
        }
    }
}