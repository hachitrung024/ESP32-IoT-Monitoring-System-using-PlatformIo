#include "toogle_boot.h"

#define BOOT_PIN GPIO_NUM_0

void toogle_boot_task(void *pvParameters)
{
    pinMode(BOOT_PIN, INPUT_PULLUP);

    unsigned long buttonPressStartTime = 0;
    while (true)
    {
        // BOOT Button to delte info.dat file
        if (digitalRead(BOOT_PIN) == LOW)
        {
            vTaskDelay(100 / portTICK_PERIOD_MS); // Debounce delay
            if (digitalRead(BOOT_PIN) == LOW)
            {
                Serial.println("Info: press detected, calling deleteInfoFile()");
                deleteInfoFile();

                vTaskDelay(500 / portTICK_PERIOD_MS);
            }
        }
        else
        {
            buttonPressStartTime = 0;
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}