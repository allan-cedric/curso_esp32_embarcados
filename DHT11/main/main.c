#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "dht.h"
#include "ssd1306.h"

#define OLED_SDA_GPIO        8
#define OLED_SCL_GPIO        9
#define OLED_RESET_GPIO     -1

static const dht_sensor_type_t sensor_type = DHT_TYPE_DHT11;
static const gpio_num_t dht_gpio = 15;

static const char *tag = "DHT11";

void app_main(void)
{
    int16_t temperature = 0;
    int16_t humidity = 0;

    char lineChar[20];

    SSD1306_t oled;

    ESP_LOGI(tag, "Panel is 128x64");
    ESP_LOGI(tag, "INTERFACE is i2c");
    ESP_LOGI(tag, "CONFIG_SDA_GPIO=%d", OLED_SDA_GPIO);
    ESP_LOGI(tag, "CONFIG_SCL_GPIO=%d", OLED_SCL_GPIO);
    ESP_LOGI(tag, "CONFIG_RESET_GPIO=%d", OLED_RESET_GPIO);

    // Initialize OLED SSD1306 display
    i2c_master_init(&oled, OLED_SDA_GPIO, OLED_SCL_GPIO, OLED_RESET_GPIO);
    ssd1306_init(&oled, 128, 64);
    ssd1306_clear_screen(&oled, false);
    ssd1306_contrast(&oled, 0xFF);

    // Write text on the display
    ssd1306_display_text_x3(&oled, 0, "DHT11", 5, false);
    vTaskDelay(pdMS_TO_TICKS(3000));

    // Clean the display
    ssd1306_clear_screen(&oled, false);
    ssd1306_contrast(&oled, 0xFF);

    while (1)
    {
        if (dht_read_data(sensor_type, dht_gpio, &humidity, &temperature) == ESP_OK)
        {
            ESP_LOGI(tag, "humidity: %d %%", humidity/10);
            ESP_LOGI(tag, "temperature: %d C", temperature/10);

            sprintf(lineChar, "Temp: %d C", temperature/10);
            ssd1306_display_text(&oled, 0, lineChar, strlen(lineChar), false);

            sprintf(lineChar, "Humi: %d %%", humidity/10);
            ssd1306_display_text(&oled, 1, lineChar, strlen(lineChar), false);
        }
        else
        {
            ESP_LOGE(tag, "Could not read data from sensor");
        }

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
