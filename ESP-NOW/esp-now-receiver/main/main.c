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

#define LED_PIN 2

#define TAG "ESP-NOW Receiver"

uint8_t sender_mac[6] = {0x68, 0x67, 0x25, 0x2e, 0x55, 0xce};

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

void espnow_recv_cb(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int data_len)
{
    ESP_LOGI(TAG, "got message from " MACSTR, MAC2STR(esp_now_info->src_addr));
    printf("message: %.*s\n", data_len, data);

    if (strncmp((const char *) data, "LED ON", data_len) == 0)
    {
        gpio_set_level(LED_PIN, 1);
    }
    else if (strncmp((const char *) data, "LED OFF", data_len) == 0)
    {
        gpio_set_level(LED_PIN, 0);
    }

    char res[50];
    sprintf(res, "%.*s ACK", data_len, data);
    esp_now_send(
        (const uint8_t *) sender_mac, 
        (const uint8_t *) res, 
        strlen(res)
    );
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
    ESP_ERROR_CHECK(esp_now_register_recv_cb(espnow_recv_cb));

    // add peer
    esp_now_peer_info_t peer;
    memset(&peer, 0, sizeof(esp_now_peer_info_t));
    memcpy(peer.peer_addr, sender_mac, 6);
    esp_now_add_peer(&peer);

    // init LED
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    // ESP_ERROR_CHECK(esp_now_deinit());
    // ESP_ERROR_CHECK(esp_wifi_stop());
}
