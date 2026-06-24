#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_random.h"
#include "esp_system.h"

#include "nvs.h"
#include "nvs_flash.h"

#define DEFAULT_SETPOINT 25

const char *TAG = "NVS Test";

uint8_t setpoint;

void app_main(void)
{
    ESP_LOGI(TAG, "\n\nInitializing NVS...");
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    nvs_handle_t nvs_handle;
    err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
    }
    else
    {
        ESP_LOGI(TAG, "Reading setpoint from NVS...");
        err = nvs_get_u8(nvs_handle, "setpoint", &setpoint);

        switch (err)
        {
            case ESP_OK:
                ESP_LOGI(TAG, "Done");
                ESP_LOGI(TAG, "Setpoint = %" PRIu8, setpoint);
                break;

            case ESP_ERR_NVS_NOT_FOUND:
                ESP_LOGE(TAG, "The value is not initialized yet!");
                ESP_LOGI(TAG, "Initializing setpoint to %d", DEFAULT_SETPOINT);
                setpoint = DEFAULT_SETPOINT;

                ESP_LOGI(TAG, "Writing setpoint to NVS...");
                err = nvs_set_u8(nvs_handle, "setpoint", setpoint);
                ESP_LOGI(TAG, "%s", (err != ESP_OK) ? "Failed!" : "Done");

                err = nvs_commit(nvs_handle);
                ESP_LOGI(TAG, "%s", (err != ESP_OK) ? "Failed!" : "Done");
            
            default:
                ESP_LOGE(TAG, "Error (%s) reading!", esp_err_to_name(err));
                break;
        }
    }

    nvs_close(nvs_handle);

    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(5000));

        setpoint = (esp_random() % 25) + 25;
        ESP_LOGI(TAG, "New setpoint = %" PRIu8, setpoint);

        err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        }
        else
        {
            ESP_LOGI(TAG, "Writing setpoint to NVS...");
            err = nvs_set_u8(nvs_handle, "setpoint", setpoint);
            ESP_LOGI(TAG, "%s", (err != ESP_OK) ? "Failed!" : "Done");

            err = nvs_commit(nvs_handle);
            ESP_LOGI(TAG, "%s", (err != ESP_OK) ? "Failed!" : "Done");

            nvs_close(nvs_handle);
        }

        esp_restart();
    }
}
