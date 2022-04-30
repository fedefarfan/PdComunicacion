#include <stdio.h>
#include "include/sht1x.h"
#include "include/sht1x-puertos.h"
 
void app_main(void)
{

    while (1) {
       
        lectura_sht10();
        espera(1000);
        sht10_hard_reset();

    }
}

