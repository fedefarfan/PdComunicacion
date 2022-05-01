/*
 * Definicion de puertos:
 *
 *    +-----------------+     +----------+
 *    |     ESP32       |     | SHT1x    |
 *    |                 |     |          | 
 *    |   GPIO 23 (CLK)  ------> SCK     |
 *    |   GPIO 22(DATOS) <-----> SDA     |
 *    +-----------------+     +----------+
 */
 
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"

#define DATOS  22
#define CLOCK  23

static const char *TAG = "sht10";
//CONFIGURA EL PIN DE 23 COMO SALIDA, RELOJ DE LA SEÑAL 
void gpio_set_direction_clock(void);

//CONFIGURA EL PIN DE 22 COMO SALIDA, PARA ENVIAR INFO AL SENSOR
void gpio_set_direction_data_out(void);
 
//CONFIGURA EL PIN DE 22 COMO ENTRADA, PARA LEE LA INFO DEL SENSOR
void gpio_set_direction_data_in(void);

//ENVIA UNO O CERO AL PUERTO DATOS 
void _gpio_level_data(uint8_t level);

//ENVIA UNO O CERO AL PUERTO CLOCK 
void _gpio_level_clock(uint8_t level);

//LEE POR EL PUERTO DATOS LA INFORMACIÓN QUE ENVIA EL SENSOR 
uint8_t gpio_set_level_data();

//PAUSA DE TIEMPO
void espera(uint32_t t);

// ENVIA 1 o 0, DEPENDIENDO SI LA CONFIGURACION DE LA MEDIDA ES ADECUADA
void cartel_config(uint16_t val1);

//IMPRIME EL RESULTADO DE LA CONFIGURACION DE LA MEDIDA SELECCIONADA
void cartel_resultado_congfig(uint8_t value1,uint8_t error1);

//IMPRIME LOS VALORES DE TEMPERATURA Y HUMEDAD DETERMINADOS
void imprime_temp_hum(float rh_true1,float t_C1);




