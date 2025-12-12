#include <stdio.h>

#include "esp_log.h"

#include "math-ops.h"

static const char* TAG = "main";

void app_main(void)
{
    Math_Ops_t mops_1, mops_2, mops_3;

    math_ops_init(&mops_1, 2, 2);
    math_ops_init(&mops_2, 10, 40);
    math_ops_init(&mops_3, 5, 0);

    int r1 = sum(&mops_1);
    int r2 = sum(&mops_2);
    int r3 = sum(&mops_3);
    ESP_LOGI(TAG, "Sum 1: %i", r1);
    ESP_LOGI(TAG, "Sum 2: %i", r2);
    ESP_LOGI(TAG, "Sum 3: %i", r3);

    r1 = sub(&mops_1, true);
    r2 = sub(&mops_2, true);
    r3 = sub(&mops_3, true);
    ESP_LOGI(TAG, "Sub 1 (ab): %i", r1);
    ESP_LOGI(TAG, "Sub 2 (ab): %i", r2);
    ESP_LOGI(TAG, "Sub 3 (ab): %i", r3);

    r1 = sub(&mops_1, false);
    r2 = sub(&mops_2, false);
    r3 = sub(&mops_3, false);
    ESP_LOGI(TAG, "Sub 1 (ba): %i", r1);
    ESP_LOGI(TAG, "Sub 2 (ba): %i", r2);
    ESP_LOGI(TAG, "Sub 3 (ba): %i", r3);

    r1 = mult(&mops_1);
    r2 = mult(&mops_2);
    r3 = mult(&mops_3);
    ESP_LOGI(TAG, "Mult 1: %i", r1);
    ESP_LOGI(TAG, "Mult 2: %i", r2);
    ESP_LOGI(TAG, "Mult 3: %i", r3);

    r1 = divi(&mops_1, true);
    r2 = divi(&mops_2, true);
    r3 = divi(&mops_3, true);
    ESP_LOGI(TAG, "Div 1 (ab): %i", r1);
    ESP_LOGI(TAG, "Div 2 (ab): %i", r2);
    ESP_LOGI(TAG, "Div 3 (ab): %i", r3);

    r1 = divi(&mops_1, false);
    r2 = divi(&mops_2, false);
    r3 = divi(&mops_3, false);
    ESP_LOGI(TAG, "Div 1 (ba): %i", r1);
    ESP_LOGI(TAG, "Div 2 (ba): %i", r2);
    ESP_LOGI(TAG, "Div 3 (ba): %i", r3);

    ESP_LOGI(TAG, "The End");
}