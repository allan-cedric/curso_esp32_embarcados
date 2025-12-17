#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/ledc.h"

#include "esp_log.h"

const static char* TAG = "LEDC-PWM-FADE-LAB";

void app_main(void)
{
    ledc_timer_config_t timer_config = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 5000,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    esp_err_t ret = ledc_timer_config(&timer_config);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error (%i) to configure ledc_timer: %s", ret, esp_err_to_name(ret));
    }
    ESP_LOGI(TAG, "Success to configure ledc_timer");

    ledc_channel_config_t channel_config = {
        .channel = LEDC_CHANNEL_0,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = GPIO_NUM_14,
        .duty = 0,
    };
    ret = ledc_channel_config(&channel_config);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error (%i) to configure ledc_channel: %s", ret, esp_err_to_name(ret));
    }
    ESP_LOGI(TAG, "Success to configure ledc_channel");

    ledc_channel_config_t channel_config_2 = {
        .channel = LEDC_CHANNEL_1,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = GPIO_NUM_21,
        .duty = 0,
    };
    ret = ledc_channel_config(&channel_config_2);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error (%i) to configure ledc_channel: %s", ret, esp_err_to_name(ret));
    }
    ESP_LOGI(TAG, "Success to configure ledc_channel");

    ret = ledc_fade_func_install(0);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error (%i) to install ledc_fade_func: %s", ret, esp_err_to_name(ret));
    }
    ESP_LOGI(TAG, "Success to install ledc_fade_func");

    while (1)
    {
        // ********************** 
        // ****** BLOCKING ******
        // **********************
        // ESP_LOGI(TAG, "(LEDC_CHANNEL_0) Starting fade IN...");
        // ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 1023, 2000, LEDC_FADE_WAIT_DONE);
        // ESP_LOGI(TAG, "(LEDC_CHANNEL_0) Done");

        // ESP_LOGI(TAG, "(LEDC_CHANNEL_1) Starting fade IN...");
        // ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 1023, 1000, LEDC_FADE_WAIT_DONE);
        // ESP_LOGI(TAG, "(LEDC_CHANNEL_1) Done");

        // vTaskDelay(500 / portTICK_PERIOD_MS);

        // ESP_LOGI(TAG, "(LEDC_CHANNEL_0) Starting fade OUT...");
        // ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0, 1000, LEDC_FADE_WAIT_DONE);
        // ESP_LOGI(TAG, "(LEDC_CHANNEL_0) Done");

        // ESP_LOGI(TAG, "(LEDC_CHANNEL_1) Starting fade OUT...");
        // ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 0, 3000, LEDC_FADE_WAIT_DONE);
        // ESP_LOGI(TAG, "(LEDC_CHANNEL_1) Done");

        // vTaskDelay(500 / portTICK_PERIOD_MS);

        // ************************* 
        // ****** NO-BLOCKING ******
        // *************************
        ESP_LOGI(TAG, "(LEDC_CHANNEL_0) Starting fade IN...");
        ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 1023, 2000, LEDC_FADE_NO_WAIT);

        ESP_LOGI(TAG, "(LEDC_CHANNEL_1) Starting fade IN...");
        ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 1023, 1000, LEDC_FADE_NO_WAIT);

        vTaskDelay(2500 / portTICK_PERIOD_MS);

        ESP_LOGI(TAG, "(LEDC_CHANNEL_0) Starting fade OUT...");
        ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0, 1000, LEDC_FADE_NO_WAIT);

        ESP_LOGI(TAG, "(LEDC_CHANNEL_1) Starting fade OUT...");
        ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 0, 3000, LEDC_FADE_NO_WAIT);

        vTaskDelay(3500 / portTICK_PERIOD_MS);
    }
}