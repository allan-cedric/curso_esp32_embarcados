#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "calculadora.h"
#include "led.h"

void app_main(void)
{
    int resultado = soma(10, 20);
    printf("Resultado: %d\n", resultado);

    liga_led();

    while (1)
    {
        printf("Hello World!\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}