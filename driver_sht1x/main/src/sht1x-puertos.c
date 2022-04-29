#include "include/sht1x-puertos.h"


#define BLINK_GPIO CONFIG_BLINK_GPIO

static uint8_t LOW = 0;
static uint8_t HIGH = 1;

//CONFIGURA EL PIN SELECCIONADO (CLOCK) COMO SALIDA, RELOJ DE LA SEÑAL 
//void configure_CLOCK(void)
void gpio_set_direction_clock(void)
{
    gpio_reset_pin(CLOCK);
    gpio_set_direction(CLOCK, GPIO_MODE_OUTPUT);
    gpio_set_level(CLOCK, LOW);
}

//CONFIGURA EL PIN SELECCIONADO (DATOS) COMO SALIDA, PARA ENVIAR INFO AL SENSOR
//void configure_DATOS_OUT(void)
void gpio_set_direction_data_out(void)
{
    gpio_reset_pin(DATOS);
    gpio_set_direction(DATOS, GPIO_MODE_OUTPUT);
    gpio_set_level(DATOS, LOW);
 }   

//CONFIGURA EL PIN DE SELECCIONADO (DATOS) COMO ENTRADA, PARA LEER LA INFO DEL SENSOR 
//void configure_DATOS_IN(void)
void gpio_set_direction_data_in(void)
{
    gpio_reset_pin(DATOS);
    gpio_set_direction(DATOS, GPIO_MODE_INPUT);
}








