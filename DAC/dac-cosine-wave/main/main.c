#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/dac_cosine.h"
#include "esp_log.h"

static const char* TAG = "dac_cosine_example";

void app_main(void)
{
    dac_cosine_handle_t chan0_handle;
    dac_cosine_config_t cos0_cfg = {
        .chan_id = DAC_CHAN_0,
        .freq_hz = 1000,
        .clk_src = DAC_COSINE_CLK_SRC_DEFAULT,
        .offset = 0,
        .phase = DAC_COSINE_PHASE_0,
        .atten = DAC_COSINE_ATTEN_DEFAULT,
        .flags.force_set_freq = false,
    };

    ESP_LOGI(TAG, "Initializing DAC cosine wave generator...");

    ESP_ERROR_CHECK(dac_cosine_new_channel(&cos0_cfg, &chan0_handle));
    ESP_ERROR_CHECK(dac_cosine_start(chan0_handle));

    ESP_LOGI(TAG, "DAC cosine wave generator initialized.");
}