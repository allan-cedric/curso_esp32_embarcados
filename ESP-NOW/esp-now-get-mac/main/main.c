#include <stdio.h>
#include "esp_mac.h"
#include "esp_log.h"

/* MAC Address example: 68:67:25:2e:55:ce */

const char *TAG = "ESP MAC Address";

void app_main(void)
{
    uint8_t mac_address[6];

    esp_efuse_mac_get_default(mac_address);

    ESP_LOGI(TAG, "MAC Address: %02x:%02x:%02x:%02x:%02x:%02x", 
        mac_address[0],
        mac_address[1],
        mac_address[2],
        mac_address[3],
        mac_address[4],
        mac_address[5]
    );
}
