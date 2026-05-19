#include <stdio.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_now.h"
#include "esp_mac.h"

#include "driver/gpio.h"
#include "esp_log.h"

#define BUTTON_PIN 2

#define TAG "ESP-NOW Sender"

uint8_t receiver_mac[6] = {0x68, 0x67, 0x25, 0x2e, 0x55, 0xce};

// void espnow_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status)
void espnow_send_cb(const wifi_tx_info_t *wsend_data, esp_now_send_status_t status)
{
    char msg_buffer[50];

    if (status == ESP_NOW_SEND_SUCCESS)
    {
        sprintf(msg_buffer, "Message sent to " MACSTR, MAC2STR(wsend_data->des_addr));
        ESP_LOGI(TAG, "%s", msg_buffer);
    }
    else
    {
        sprintf(msg_buffer, "Error sending the message to " MACSTR, MAC2STR(wsend_data->des_addr));
        ESP_LOGE(TAG, "%s", msg_buffer);
    }
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    // init WiFi
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    // init ESP-NOW
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_send_cb(espnow_send_cb));

    // add peer
    esp_now_peer_info_t peer;
    memset(&peer, 0, sizeof(esp_now_peer_info_t));
    memcpy(peer.peer_addr, receiver_mac, 6);
    esp_now_add_peer(&peer);

    // init gpio
    gpio_reset_pin(BUTTON_PIN);
    gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_PIN, GPIO_PULLUP_ONLY);

    char send_msg_buffer[100];
    uint8_t ledStatus = 0;
    uint8_t button_state = 1;

    while (1)
    {
        int new_state = gpio_get_level(BUTTON_PIN);

        if (new_state != button_state)
        {
            button_state = new_state;
            if (button_state == 0)
            {
                ESP_LOGI(TAG, "Button pressed");
                ledStatus = !ledStatus;
                sprintf(send_msg_buffer, "LED %s", ledStatus ? "ON" : "OFF");
                esp_now_send(receiver_mac, (uint8_t*) send_msg_buffer, strlen(send_msg_buffer));
            }
            else
            {
                ESP_LOGI(TAG, "Button released");
            }
        }
        vTaskDelay(100/portTICK_PERIOD_MS);
    }

    ESP_ERROR_CHECK(esp_now_deinit());
    ESP_ERROR_CHECK(esp_wifi_stop());
}
