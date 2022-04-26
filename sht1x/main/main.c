#include <stdio.h>
#include "include/sht1x.h"
 
void app_main(void)
{

    while (1) {
       
        lectura_sht10();
        vTaskDelay(1000/portTICK_PERIOD_MS);
        sht10_hard_reset();

    }
}

