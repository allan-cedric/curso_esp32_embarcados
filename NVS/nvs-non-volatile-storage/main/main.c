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

typedef struct
{
    float kp;
    float ki;
    float kd;
} pid_controller_t;

char ssid[32];
char password[32];

void app_main(void)
{
    /* NVS initialization */

    ESP_LOGI(TAG, "\n\nInitializing NVS...");
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    /* Store 'setpoint' */

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
                break;
            
            default:
                ESP_LOGE(TAG, "Error (%s) reading!", esp_err_to_name(err));
                break;
        }
    }

    nvs_close(nvs_handle);

    /* Store 'PID' constants */

    pid_controller_t pid;

    err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
    }
    else
    {
        ESP_LOGI(TAG, "Reading PID values from NVS...");
        size_t required_size = sizeof(pid);
        err = nvs_get_blob(nvs_handle, "pid", &pid, &required_size);

        switch (err)
        {
            case ESP_OK:
                ESP_LOGI(TAG, "Done");
                ESP_LOGI(TAG, "PID values: Kp = %f, Ki = %f, Kd = %f", pid.kp, pid.ki, pid.kd);
                break;

            case ESP_ERR_NVS_NOT_FOUND:
                ESP_LOGE(TAG, "The value is not initialized yet!");
                ESP_LOGI(TAG, "Initializing PID values");

                pid.kp = 1.0;
                pid.ki = 0.8;
                pid.kd = 0.2;
                ESP_LOGI(TAG, "Writing PID values to NVS...");
                err = nvs_set_blob(nvs_handle, "pid", &pid, sizeof(pid));
                ESP_LOGI(TAG, "%s", (err != ESP_OK) ? "Failed!" : "Done");
                err = nvs_commit(nvs_handle);
                ESP_LOGI(TAG, "%s", (err != ESP_OK) ? "Failed!" : "Done");
                break;
            
            default:
                ESP_LOGE(TAG, "Error (%s) reading!", esp_err_to_name(err));
                break;
        }

        nvs_close(nvs_handle);
    }

    /* Store 'SSID' */

    err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
    }
    else
    {
        ESP_LOGI(TAG, "Reading SSID from NVS...");
        size_t required_size = sizeof(ssid);
        err = nvs_get_str(nvs_handle, "ssid", ssid, &required_size);

        switch (err)
        {
            case ESP_OK:
                ESP_LOGI(TAG, "Done");
                ESP_LOGI(TAG, "SSID = %s", ssid);
                break;

            case ESP_ERR_NVS_NOT_FOUND:
                ESP_LOGE(TAG, "The value is not initialized yet!");
                ESP_LOGI(TAG, "Initializing SSID");

                strncpy(ssid, "MySSID", required_size);

                ESP_LOGI(TAG, "Writing SSID to NVS...");
                err = nvs_set_str(nvs_handle, "ssid", ssid);
                ESP_LOGI(TAG, "%s", (err != ESP_OK) ? "Failed!" : "Done");
                err = nvs_commit(nvs_handle);
                ESP_LOGI(TAG, "%s", (err != ESP_OK) ? "Failed!" : "Done");
                break;
            
            default:
                ESP_LOGE(TAG, "Error (%s) reading!", esp_err_to_name(err));
                break;
        }

        nvs_close(nvs_handle);
    }

    /* Store 'password' */

    err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
    }
    else
    {
        ESP_LOGI(TAG, "Reading password from NVS...");
        size_t required_size = sizeof(password);
        err = nvs_get_str(nvs_handle, "password", password, &required_size);

        switch (err)
        {
            case ESP_OK:
                ESP_LOGI(TAG, "Done");
                ESP_LOGI(TAG, "Password = %s", password);
                break;

            case ESP_ERR_NVS_NOT_FOUND:
                ESP_LOGE(TAG, "The value is not initialized yet!");
                ESP_LOGI(TAG, "Initializing password");

                strncpy(password, "MyPassword", required_size);

                ESP_LOGI(TAG, "Writing password to NVS...");
                err = nvs_set_str(nvs_handle, "password", password);
                ESP_LOGI(TAG, "%s", (err != ESP_OK) ? "Failed!" : "Done");
                err = nvs_commit(nvs_handle);
                ESP_LOGI(TAG, "%s", (err != ESP_OK) ? "Failed!" : "Done");
                break;
            
            default:
                ESP_LOGE(TAG, "Error (%s) reading!", esp_err_to_name(err));
                break;
        }

        nvs_close(nvs_handle);
    }

    /* Update 'setpoint' value */

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
