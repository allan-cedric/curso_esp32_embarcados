/******************************************************************************
* Notification
* This example shows how to send a push notification to app
* This code is part of the course "Academia ESP32 Profissional" by Fábio Souza
* The course is available on https://cursos.embarcados.com.br
*
* This example code Creative Commons Attribution 4.0 International License.
* When using the code, you must keep the above copyright notice,
* this list of conditions and the following disclaimer in the source code.
* (http://creativecommons.org/licenses/by/4.0/)

* Author: Fábio Souza
* This code is for teaching purposes only.
* No warranty of any kind is provided.
*******************************************************************************/

#include <string.h>                // Include standard library for string manipulation functions
#include <inttypes.h>              // Include standard library for integer type definitions (fixed-width integers)
#include <freertos/FreeRTOS.h>     // Include FreeRTOS library for real-time operating system functions
#include <freertos/task.h>         // Include FreeRTOS task management functions for multitasking
#include <esp_log.h>               // Include ESP logging library for debugging and logging purposes
#include <esp_event.h>             // Include ESP event library for handling and dispatching system events
#include <nvs_flash.h>             // Include NVS (Non-Volatile Storage) flash library for storing data in flash memory

#include <esp_rmaker_core.h>       // Include ESP RainMaker core library for connecting devices to RainMaker cloud
#include <esp_rmaker_standard_types.h> // Include standard types for ESP RainMaker devices
#include <esp_rmaker_standard_params.h> // Include standard parameters for ESP RainMaker devices (e.g., power, name)
#include <esp_rmaker_standard_devices.h> // Include standard devices (e.g., switch, light) for RainMaker
#include <esp_rmaker_schedule.h>   // Include RainMaker scheduling functionalities
#include <esp_rmaker_scenes.h>     // Include RainMaker scene management for device group control
#include <esp_rmaker_console.h>    // Include console features for ESP RainMaker for command-line interface (CLI)
#include <esp_rmaker_ota.h>        // Include Over-The-Air (OTA) update functionalities for RainMaker devices

#include <esp_rmaker_common_events.h> // Include common event definitions for ESP RainMaker

#include <app_network.h>           // Include custom application networking functionalities (e.g., Wi-Fi setup)
#include <app_insights.h>          // Include application insights for monitoring device metrics and analytics

#include "app_priv.h"              // Include application-specific private definitions and functions

static const char *TAG = "app_main"; // Define a logging tag to identify log messages related to this application's main functionality

esp_rmaker_device_t *sensor_device;

/* Callback to handle commands received from the RainMaker cloud */
static esp_err_t write_cb(const esp_rmaker_device_t *device, const esp_rmaker_param_t *param,
            const esp_rmaker_param_val_t val, void *priv_data, esp_rmaker_write_ctx_t *ctx)
{
    if (ctx) {
        ESP_LOGI(TAG, "Received write request via : %s", esp_rmaker_device_cb_src_to_str(ctx->src));
    }
    
    //update your device here

    return ESP_OK;
}

/* Event handler for catching RainMaker events */
static void event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data)
{
    /* Check if the event is from the RainMaker base event */
    if (event_base == RMAKER_EVENT) {
        switch (event_id) {
            /* RainMaker initialization completed */
            case RMAKER_EVENT_INIT_DONE:
                ESP_LOGI(TAG, "RainMaker Initialised.");
                break;
            /* Device claim process has started */
            case RMAKER_EVENT_CLAIM_STARTED:
                ESP_LOGI(TAG, "RainMaker Claim Started.");
                break;
            /* Device claim process was successful */
            case RMAKER_EVENT_CLAIM_SUCCESSFUL:
                ESP_LOGI(TAG, "RainMaker Claim Successful.");
                break;
            /* Device claim process failed */
            case RMAKER_EVENT_CLAIM_FAILED:
                ESP_LOGI(TAG, "RainMaker Claim Failed.");
                break;
            /* Local control has been started */
            case RMAKER_EVENT_LOCAL_CTRL_STARTED:
                ESP_LOGI(TAG, "Local Control Started.");
                break;
            /* Local control has been stopped */
            case RMAKER_EVENT_LOCAL_CTRL_STOPPED:
                ESP_LOGI(TAG, "Local Control Stopped.");
                break;
            /* Catch-all for unhandled RainMaker events */
            default:
                ESP_LOGW(TAG, "Unhandled RainMaker Event: %"PRIi32, event_id);
        }
    /* Check if the event is from RainMaker common events */
    } else if (event_base == RMAKER_COMMON_EVENT) {
        switch (event_id) {
            /* Device is about to reboot */
            case RMAKER_EVENT_REBOOT:
                ESP_LOGI(TAG, "Rebooting in %d seconds.", *((uint8_t *)event_data));
                break;
            /* Wi-Fi credentials have been reset */
            case RMAKER_EVENT_WIFI_RESET:
                ESP_LOGI(TAG, "Wi-Fi credentials reset.");
                break;
            /* Device reset to factory defaults */
            case RMAKER_EVENT_FACTORY_RESET:
                ESP_LOGI(TAG, "Node reset to factory defaults.");
                break;
            /* MQTT connection established */
            case RMAKER_MQTT_EVENT_CONNECTED:
                ESP_LOGI(TAG, "MQTT Connected.");
                break;
            /* MQTT connection disconnected */
            case RMAKER_MQTT_EVENT_DISCONNECTED:
                ESP_LOGI(TAG, "MQTT Disconnected.");
                break;
            /* MQTT message published successfully */
            case RMAKER_MQTT_EVENT_PUBLISHED:
                ESP_LOGI(TAG, "MQTT Published. Msg id: %d.", *((int *)event_data));
                break;
            /* Catch-all for unhandled RainMaker common events */
            default:
                ESP_LOGW(TAG, "Unhandled RainMaker Common Event: %"PRIi32, event_id);
        }
    /* Check if the event is from the application network */
    } else if (event_base == APP_NETWORK_EVENT) {
        switch (event_id) {
            /* QR code for provisioning has been generated */
            case APP_NETWORK_EVENT_QR_DISPLAY:
                ESP_LOGI(TAG, "Provisioning QR : %s", (char *)event_data);
                break;
            /* Provisioning has timed out */
            case APP_NETWORK_EVENT_PROV_TIMEOUT:
                ESP_LOGI(TAG, "Provisioning Timed Out. Please reboot.");
                break;
            /* Provisioning has restarted after failures */
            case APP_NETWORK_EVENT_PROV_RESTART:
                ESP_LOGI(TAG, "Provisioning has restarted due to failures.");
                break;
            /* Catch-all for unhandled App Wi-Fi events */
            default:
                ESP_LOGW(TAG, "Unhandled App Wi-Fi Event: %"PRIi32, event_id);
                break;
        }
    /* Check if the event is from RainMaker OTA (Over-the-Air) updates */
    } else if (event_base == RMAKER_OTA_EVENT) {
        switch(event_id) {
            /* OTA update process has started */
            case RMAKER_OTA_EVENT_STARTING:
                ESP_LOGI(TAG, "Starting OTA.");
                break;
            /* OTA update is currently in progress */
            case RMAKER_OTA_EVENT_IN_PROGRESS:
                ESP_LOGI(TAG, "OTA is in progress.");
                break;
            /* OTA update completed successfully */
            case RMAKER_OTA_EVENT_SUCCESSFUL:
                ESP_LOGI(TAG, "OTA successful.");
                break;
            /* OTA update failed */
            case RMAKER_OTA_EVENT_FAILED:
                ESP_LOGI(TAG, "OTA Failed.");
                break;
            /* OTA update was rejected by the server */
            case RMAKER_OTA_EVENT_REJECTED:
                ESP_LOGI(TAG, "OTA Rejected.");
                break;
            /* OTA update delayed */
            case RMAKER_OTA_EVENT_DELAYED:
                ESP_LOGI(TAG, "OTA Delayed.");
                break;
            /* Firmware downloaded, requesting a reboot to apply the update */
            case RMAKER_OTA_EVENT_REQ_FOR_REBOOT:
                ESP_LOGI(TAG, "Firmware image downloaded. Please reboot your device to apply the upgrade.");
                break;
            /* Catch-all for unhandled OTA events */
            default:
                ESP_LOGW(TAG, "Unhandled OTA Event: %"PRIi32, event_id);
                break;
        }
    /* Catch-all for invalid or unknown events */
    } else {
        ESP_LOGW(TAG, "Invalid event received!");
    }
}


void app_main()
{
    /* Initialize Application specific hardware drivers and
     * set initial state.
     */
    app_driver_init();

    /* Initialize NVS. */
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

    /* Initialize Wi-Fi. Note that, this should be called before esp_rmaker_node_init()
     */
    app_network_init();

    /* Register an event handler to catch RainMaker events */
    ESP_ERROR_CHECK(esp_event_handler_register(RMAKER_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(RMAKER_COMMON_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(APP_NETWORK_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(RMAKER_OTA_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));

     /* Initialize the ESP RainMaker Agent.
	 * This configures the RainMaker service for the device.
	 * Note that this should be called after app_network_init() but before app_network_start()
	 */
	esp_rmaker_config_t rainmaker_cfg = {
	    .enable_time_sync = false, // Disable automatic time synchronization (optional, can be enabled if needed)
	};
	
	// Initialize the RainMaker node with the specified configuration, device name, and device type (Change the device type according to your project)
	esp_rmaker_node_t *node = esp_rmaker_node_init(&rainmaker_cfg, "ESP RainMaker Device", "GPIO-Device");
	if (!node) {
	    // Log an error message and abort the program if the node could not be initialized
	    ESP_LOGE(TAG, "Could not initialise node. Aborting!!!");
	    vTaskDelay(5000 / portTICK_PERIOD_MS); // Delay for 5 seconds before aborting
	    abort();  // Terminate the program if the node initialization fails
	}

    /* Create a device and add the relevant parameters to it */
     
    sensor_device = esp_rmaker_device_create("Sensor", "esp.device.motion-sensor", NULL);

    //Add the write callback for the device.
	//esp_rmaker_device_add_cb(switch_device, write_cb, NULL);

    //parameters
    esp_rmaker_device_add_param(sensor_device, esp_rmaker_name_param_create(ESP_RMAKER_DEF_OTA_STATUS_NAME, "Motion Sensor"));
	esp_rmaker_param_t *motion_state_param = esp_rmaker_param_create("Motion State", "esp.param.motion-detection-state", esp_rmaker_bool(false), PROP_FLAG_READ);
	esp_rmaker_device_add_param(sensor_device,motion_state_param );
	esp_rmaker_device_assign_primary_param(sensor_device, motion_state_param);

    /* Add this switch device to the node */
    esp_rmaker_node_add_device(node, sensor_device);

    /* Enable OTA */
    esp_rmaker_ota_enable_default();

    /* Enable timezone service which will be require for setting appropriate timezone
     * from the phone apps for scheduling to work correctly.
     * For more information on the various ways of setting timezone, please check
     * https://rainmaker.espressif.com/docs/time-service.html.
     */
    esp_rmaker_timezone_service_enable();

    /* Enable scheduling. */
    esp_rmaker_schedule_enable();

    /* Enable Scenes */
    esp_rmaker_scenes_enable();

    /* Enable Insights. Requires CONFIG_ESP_INSIGHTS_ENABLED=y */
    app_insights_enable();

    /* Start the ESP RainMaker Agent */
    esp_rmaker_start();

    /* Start the Wi-Fi.
     * If the node is provisioned, it will start connection attempts,
     * else, it will start Wi-Fi provisioning. The function will return
     * after a connection has been successfully established
     */
    err = app_network_start(POP_TYPE_RANDOM);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not start Wifi. Aborting!!!");
        vTaskDelay(5000/portTICK_PERIOD_MS);
        abort();
    }
}
