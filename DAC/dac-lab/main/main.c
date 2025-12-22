#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#ifdef CONFIG_DAC_WAVE_ONESHOT
#include "driver/dac_oneshot.h"
#endif

#ifdef CONFIG_DAC_WAVE_COS
#include "driver/dac_cosine.h"
#endif

#ifdef CONFIG_DAC_CHANNEL_0
#define DAC_CHANNEL DAC_CHAN_0
#endif

#ifdef CONFIG_DAC_CHANNEL_1
#define DAC_CHANNEL DAC_CHAN_1
#endif

const static char* TAG = "DAC-LAB";

void app_main(void)
{
    #ifdef CONFIG_DAC_WAVE_ONESHOT
        uint8_t val = 0;

        dac_oneshot_handle_t chan0_handle;
        dac_oneshot_config_t chan0_cfg = {
            .chan_id = DAC_CHANNEL,
        };
        ESP_ERROR_CHECK(dac_oneshot_new_channel(&chan0_cfg, &chan0_handle));

        ESP_LOGI(TAG, "DAC oneshot example start.");

        #ifdef CONFIG_DAC_WAVE_SAW
            ESP_LOGI(TAG, "Generating SAW Wave");
        #endif

        #ifdef CONFIG_DAC_WAVE_TRIANGULAR
            ESP_LOGI(TAG, "Generating TRIANGULAR Wave");
        #endif

        while(1)
        {
            #ifdef CONFIG_DAC_WAVE_SAW
                for(val = 0; val < 255; val++){
                    ESP_ERROR_CHECK(dac_oneshot_output_voltage(chan0_handle, val));
                    vTaskDelay(pdMS_TO_TICKS(1));
                }
            #endif
            
            #ifdef CONFIG_DAC_WAVE_TRIANGULAR
                for(val = 255; val > 0; val--){
                    ESP_ERROR_CHECK(dac_oneshot_output_voltage(chan0_handle, val));
                    vTaskDelay(pdMS_TO_TICKS(1));
                }
            #endif
        }
    #endif

    #ifdef CONFIG_DAC_WAVE_COS
        dac_cosine_handle_t chan0_handle;
        dac_cosine_config_t cos0_cfg = {
            .chan_id = DAC_CHANNEL,
            .freq_hz = CONFIG_DAC_WAVE_COS_FREQ_HZ,
            .clk_src = DAC_COSINE_CLK_SRC_DEFAULT,
            .offset = CONFIG_DAC_WAVE_COS_OFFSET,
            .phase = DAC_COSINE_PHASE_0,
            .atten = DAC_COSINE_ATTEN_DEFAULT,
            .flags.force_set_freq = false,
        };

        ESP_LOGI(TAG, "Initializing DAC cosine wave generator...");

        ESP_ERROR_CHECK(dac_cosine_new_channel(&cos0_cfg, &chan0_handle));
        ESP_ERROR_CHECK(dac_cosine_start(chan0_handle));

        ESP_LOGI(TAG, "DAC cosine wave generator initialized.");
    #endif
}