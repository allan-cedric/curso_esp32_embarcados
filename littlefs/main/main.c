#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>

#include "esp_flash.h"
#include "esp_log.h"
#include "esp_littlefs.h"

static const char *TAG = "Test LittleFS";

void app_main(void)
{
    /* LittleFS initialization */

    ESP_LOGI(TAG, "This is a LittleFS example");

    uint32_t size_flash_chip = 0;
    esp_flash_get_size(NULL, &size_flash_chip);
    ESP_LOGI(TAG, "Flash chip size: %uMB", (unsigned int) size_flash_chip >> 20);

    esp_vfs_littlefs_conf_t conf = {
        .base_path = "/littlefs",
        .partition_label = "littlefs",
        .format_if_mount_failed = true,
        .dont_mount = false
    };

    esp_err_t ret = esp_vfs_littlefs_register(&conf);

    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        }
        else if (ret == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG, "Failed to find LittleFS partition");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize LittleFS (%s)", esp_err_to_name(ret));
        }
    }

    /* Get information about LittleFS partition */

    size_t total = 0, used = 0;

    ret = esp_littlefs_info(conf.partition_label, &total, &used);

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get LittleFS partition information (%s). Formatting...", esp_err_to_name(ret));
        esp_littlefs_format(conf.partition_label);
    }
    else
    {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    /* POSIX File API */

    ESP_LOGI(TAG, "Opening file");
    FILE *f = fopen("/littlefs/hello.txt", "w");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }
    fprintf(f, "Hello World from LittleFS\n");
    fclose(f);
    ESP_LOGI(TAG, "File written");

    struct stat st;
    if (stat("/littlefs/foo.txt", &st) == 0)
    {
        unlink("/littlefs/foo.txt");
    }

    ESP_LOGI(TAG, "Renaming file");
    if (rename("/littlefs/hello.txt", "/littlefs/foo.txt") != 0)
    {
        ESP_LOGE(TAG, "Rename failed");
        return;
    }

    f = fopen("/littlefs/foo.txt", "r");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return;
    }
    char line[128];
    while (fgets(line, sizeof(line), f) != NULL)
    {
        printf("%s", line);
    }
    printf("\n");
    fclose(f);

    /* Unmounting LittleFS partition */

    ESP_LOGI(TAG, "Unmounting");
    esp_vfs_littlefs_unregister(conf.partition_label);
    ESP_LOGI(TAG, "LittleFS Unmounted");
}
