#include <stdio.h>
#include "math-ops.h"

#include "esp_log.h"

static const char *TAG = "math-ops";

void math_ops_init(Math_Ops_t *mops, int a, int b)
{
    mops->a = a;
    mops->b = b;
    ESP_LOGI(TAG, "Init Math_Ops_t {a=%i, b=%i}", mops->a, mops->b);
}

int sum(Math_Ops_t *mops)
{
    return mops->a + mops->b;
}

int sub(Math_Ops_t *mops, bool ab)
{
    if(ab)
    {
        return mops->a - mops->b;
    }
    return mops->b - mops->a;
}

int mult(Math_Ops_t *mops)
{
    return mops->a * mops->b;
}

int divi(Math_Ops_t *mops, bool ab)
{
    if(ab)
    {
        if(mops->b == 0)
        {
            ESP_LOGE(TAG, "Division by zero: b=%i", mops->b);
            return INT32_MAX;
        }
        return mops->a / mops->b;
    }
    if(mops->a == 0)
    {
        ESP_LOGE(TAG, "Division by zero: a=%i", mops->a);
        return INT32_MAX;
    }
    return mops->b / mops->a;
}