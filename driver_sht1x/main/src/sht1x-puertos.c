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

//ENVIA UNO O CERO AL PUERTO DATOS 
void _gpio_level_data(uint8_t level)
{
    gpio_set_level(DATOS, level);
}

//ENVIA UNO O CERO AL PUERTO CLOCK 
void _gpio_level_clock(uint8_t level)
{
    gpio_set_level(CLOCK, level);
}

//LEE POR EL PUERTO DATOS LA INFORMACIÓN QUE ENVIA EL SENSOR 
uint8_t gpio_set_level_data()
{
uint8_t dat;
dat=gpio_get_level(DATOS);
return dat;
}

//PAUSA DE TIEMPO
void espera(uint32_t t)
{
vTaskDelay(t/portTICK_PERIOD_MS);
}

// ENVIA 1 o 0, DEPENDIENDO SI LA CONFIGURACION DE LA MEDIDA ES ADECUADA
void cartel_config(uint16_t val1)
{
 ESP_LOGI(TAG, "Valor de VAL 1= %u ", val1);//IMPRIME EL VALOR OBTENIDO
}

//IMPRIME EL RESULTADO DE LA CONFIGURACION DE LA MEDIDA SELECCIONADA
void cartel_resultado_congfig(uint8_t value1,uint8_t error1)
{
    if(value1==5)//CARTEL QUE INDICA SI LA CONFIGURACIÓN PARA LA MEDIDA DE HUM FUE EXITOSA
    {
    if(error1==0)
    {
    ESP_LOGI(TAG, "ACK1-Configuracion exitosa (HUMEDAD) %u", error1);
    }else
    {
    ESP_LOGI(TAG, "ACK1-Fallo configuracion , (HUMEDAD) %u", error1);
    }
    }else{//CARTEL QUE INDICA SI LA CONFIGURACIÓN PARA LA MEDIDA DE TEMP FUE EXITOSA
    
    if(error1==0)
    {
    ESP_LOGI(TAG, "ACK1-Configuracion exitosa (TEMPERATURA) %u", error1);
    }else
    {
    ESP_LOGI(TAG, "ACK1-Fallo configuracion , (TEMPERATURA) %u", error1);
    }
    
    }

}

//IMPRIME LOS VALORES DE TEMPERATURA Y HUMEDAD DETERMINADOS
void imprime_temp_hum(float rh_true1,float t_C1)
{
   ESP_LOGI(TAG, "HUMEDAD %f ", rh_true1);//IMPRIME VALOR DE HUM
   ESP_LOGI(TAG, "TEMPERATURA %f ", t_C1);//IMPRIME VALOR DE TEMP
}

