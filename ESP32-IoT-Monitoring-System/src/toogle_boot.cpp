#include "toogle_boot.h"

#define BOOT 0

void toogle_boot_task(void *pvParameters)
{
    unsigned long buttonPressStartTime = 0;
    while (true)
    {
        if (digitalRead(BOOT) == LOW)
        {
            if (buttonPressStartTime == 0)
            {
                buttonPressStartTime = millis();
            }
            else if (millis() - buttonPressStartTime > 2000)
            {
                deleteInfoFile();
                vTaskDelete(NULL);
            }
        }
        else
        {
            buttonPressStartTime = 0;
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}