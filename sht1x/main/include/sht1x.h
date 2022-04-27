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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"

#define SHT10_ACK   1
#define SHT10_NOACK 0 

#define DATOS  22
#define CLOCK  23

#define BLINK_GPIO CONFIG_BLINK_GPIO

    typedef union
    {
      uint16_t i;
      uint32_t f;
    } valor;
    valor humedad, temperatura;

uint8_t errorsht10,checksum;

//CONFIGURA EL PIN DE 23 COMO SALIDA, RELOJ DE LA SEÑAL 
static void configure_CLOCK(void);

//CONFIGURA EL PIN DE 22 COMO SALIDA, PARA ENVIAR INFO AL SENSOR
static void configure_DATOS_OUT(void);
 
//CONFIGURA EL PIN DE 22 COMO ENTRADA, PARA LEE LA INFO DEL SENSOR
static void configure_DATOS_IN(void);

//LLAMA LAS FUNCIONES PARA MEDIR TEMPERATURA Y HUMEDAD. RECIBE LOS DATOS MEDIDOS Y 
//UNA SEÑAL DE ERROR SI LA COMUNICACIÓN FALLA.
void lectura_sht10(void);

//ENVIA LA TRAMA DE INICIO DEL SENSOR
void sht10_init(void);

//LLAMA LAS FUNCIONES PARA CONFIGURARA LAS MEDIDA.
//DEPENDIENDO DE modo, SE INICIA LA MEDIDA DE TEMPERATURA O HUMEDAD
// RETORNA EL PARAMETRO QUE INDICA SI HAY ERROR EN LA CONFIGURACIÓN
uint8_t sht10_medicion(uint16_t *p_valor,uint8_t *p_checksum,uint8_t modo);

//MANDA EL TREN DE PULSO PARA SELECCIONAR LA MEDIDA DE TEMPERATURA O HUMEDAD
uint8_t sht10_escribir_byte(uint8_t value1);
    
//LEE LA INFORMACIÓN QUE ENVIA EL SENSOR. SE REALIZA DE A BYTES
uint16_t sht10_leer_byteH(uint8_t ack);

//CÁLCULO DE LA HUMEDAD Y TEMPERATURA MEDIDA POR EL SENSOR
void sht10_calculos(uint16_t *p_humedad,uint16_t *p_temperatura);
    
//RESET DEL SENSOR    
void sht10_hard_reset(void);

    


