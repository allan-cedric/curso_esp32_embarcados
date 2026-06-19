#include <stdio.h>

#include "esp_log.h"

static const char *TAG = "main test binary data";

void app_main(void)
{
    ESP_LOGI(TAG, "Page.html");

    extern const uint8_t page_html_start[] asm("_binary_page_html_start");
    extern const uint8_t page_html_end[] asm("_binary_page_html_end");

    ESP_LOGI(TAG, "START Address: %p, END Address: %p", page_html_start, page_html_end);

    ESP_LOGI(TAG, "SIZE: %d", page_html_end - page_html_start);

    ESP_LOGI(TAG, "CONTENT: \n\n%s", page_html_start);

    /*****************************************************/

    ESP_LOGI(TAG, "Sample.txt");

    extern const uint8_t sample_txt_start[] asm("_binary_sample_txt_start");
    extern const uint8_t sample_txt_end[] asm("_binary_sample_txt_end");

    ESP_LOGI(TAG, "START Address: %p, END Address: %p", sample_txt_start, sample_txt_end);

    ESP_LOGI(TAG, "SIZE: %d", sample_txt_end - sample_txt_start);

    ESP_LOGI(TAG, "CONTENT: \n\n%s", sample_txt_start);

    /*****************************************************/

    ESP_LOGI(TAG, "Favicon.ico");

    extern const uint8_t favicon_ico_start[] asm("_binary_favicon_ico_start");
    extern const uint8_t favicon_ico_end[] asm("_binary_favicon_ico_end");

    ESP_LOGI(TAG, "START Address: %p, END Address: %p", favicon_ico_start, favicon_ico_end);

    ESP_LOGI(TAG, "SIZE: %d", favicon_ico_end - favicon_ico_start);

    // ESP_LOGI(TAG, "CONTENT: \n\n%s", favicon_ico_start);
}
