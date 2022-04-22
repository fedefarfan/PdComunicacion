/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"

static const char *TAG = "example";



#define SHT10_ACK   1
#define SHT10_NOACK 0 
 

#define DATOS  22
#define CLOCK  23
#define entrada 21


/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
uint8_t pin=2;

#define BLINK_GPIO CONFIG_BLINK_GPIO



static uint8_t LOW = 0;
static uint8_t HIGH = 1;


static uint8_t TEMP = 0;
static uint8_t HUMI = 1;


    typedef union
    {
      uint16_t i;
      uint32_t f;
    } valor;
    valor humedad, temperatura;
    

uint8_t errorsht10,checksum;


static void configure_CLOCK(void)
{
  //  ESP_LOGI(TAG, "Configuramos los pines de entrada y salida");
  
    gpio_reset_pin(pin);
    gpio_reset_pin(CLOCK);
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_direction(CLOCK, GPIO_MODE_OUTPUT);
    gpio_set_level(CLOCK, LOW);
    gpio_set_level(pin, LOW);
}


static void configure_DATOS_OUT(void)
{
    gpio_reset_pin(DATOS);
    gpio_set_direction(DATOS, GPIO_MODE_OUTPUT);
    gpio_set_level(DATOS, LOW);
 }   
 
 static void configure_DATOS_IN(void)
 {
    gpio_reset_pin(DATOS);
    gpio_set_direction(DATOS, GPIO_MODE_INPUT);
       
        
}

    
    void sht10_calculos(uint16_t *p_humedad,uint16_t *p_temperatura)
    {
   
    const float C1=-4.3468;
    const float C2=0.0367;
    const float C3=-0.0000015955;
    
    const uint32_t T1=+0.01;
    const uint32_t T2=+0.00008;
     
    float rh;
    float t;
    float rh_lin;
    float rh_true;
    float t_C;
    rh =(float) *p_humedad;
    
    t = (float) *p_temperatura;
     
    t_C = t*0.01 - 40;
    rh_lin=C3*rh*rh + C2*rh + C1;

    rh_true=(t_C-25)*(T1+T2*rh)+rh_lin;
    if(rh_true>100)rh_true=100;
    if(rh_true<0.1)rh_true=0.1;
     

   ESP_LOGI(TAG, "HUMEDAD %f ", rh_true);
   ESP_LOGI(TAG, "TEMPERATURA %f ", t_C);
    }



 void sht10_init(void)
    {
    
    configure_CLOCK();
    configure_DATOS_OUT();    
    
    gpio_set_level(DATOS, HIGH);
    gpio_set_level(CLOCK, LOW);
    vTaskDelay(1 / portTICK_PERIOD_MS);
    gpio_set_level(CLOCK, HIGH);  
    vTaskDelay(1 / portTICK_PERIOD_MS);
    gpio_set_level(DATOS, LOW);
    vTaskDelay(1 / portTICK_PERIOD_MS);
    gpio_set_level(CLOCK, LOW);
    vTaskDelay(1 / portTICK_PERIOD_MS);    
    gpio_set_level(CLOCK, HIGH);
    vTaskDelay(1 / portTICK_PERIOD_MS); 
    gpio_set_level(DATOS, HIGH);
    vTaskDelay(1 / portTICK_PERIOD_MS);
    gpio_set_level(CLOCK, LOW);
    vTaskDelay(1 / portTICK_PERIOD_MS);
    gpio_set_level(DATOS, LOW);
    vTaskDelay(1 / portTICK_PERIOD_MS);
    }
    
      void sht10_hard_reset(void)
    {
    uint8_t i;
     
    gpio_set_level(CLOCK, LOW);
    gpio_set_level(DATOS, HIGH);   
    vTaskDelay(1 / portTICK_PERIOD_MS);
  
    for(i=0;i<9;i++)            
      {
    gpio_set_level(CLOCK, HIGH);
    vTaskDelay(1 / portTICK_PERIOD_MS);
    gpio_set_level(CLOCK, LOW);
    vTaskDelay(1 / portTICK_PERIOD_MS);


    }
                
    }
    
    
    uint8_t sht10_escribir_byte(uint8_t value1)
    {
      
    
     uint8_t i,error=0,value=0;
     
     
     if(value1==0)
{
     value=3;
 }    
     
     if(value1==1)
     {
     value=5;
     }
     
    for (i=128;i>0;i/=2)                
      {
    if (i & value)  gpio_set_level(DATOS, HIGH);  
    else  gpio_set_level(DATOS, LOW);
    
    
    gpio_set_level(CLOCK, HIGH);
    vTaskDelay(1 / portTICK_PERIOD_MS);
    gpio_set_level(CLOCK, LOW);
    

      }
      gpio_set_level(DATOS, HIGH);
      gpio_set_level(CLOCK, HIGH);
      configure_DATOS_IN();    
      error=error+gpio_get_level(DATOS);
      gpio_set_level(CLOCK, LOW);
    
    
    if(value==5)
    {
    if(error==0)
    {
    ESP_LOGI(TAG, "ACK1-Configuracion exitosa (HUMEDAD) %u", error);
    }else
    {
    ESP_LOGI(TAG, "ACK1-Fallo configuracion , (HUMEDAD) %u", error);
    }
    }else{
    
    if(error==0)
    {
    ESP_LOGI(TAG, "ACK1-Configuracion exitosa (TEMPERATURA) %u", error);
    }else
    {
    ESP_LOGI(TAG, "ACK1-Fallo configuracion , (TEMPERATURA) %u", error);
    }
    
    }
      

    return error;     
         
    }


uint16_t sht10_leer_byteH(uint8_t ack)
    {
    
    
        uint16_t i,val=0;
    
    
    configure_DATOS_OUT(); 
    gpio_set_level(DATOS, HIGH);  
    configure_DATOS_IN(); 
    
    
    for (i=128;i>0;i/=2)          
      {
    gpio_set_level(CLOCK, HIGH);
             
    if(gpio_get_level(DATOS))
                        val=(val | i);  
      gpio_set_level(CLOCK, LOW);
   
    }    
        configure_DATOS_OUT();   
      if (ack) gpio_set_level(DATOS, LOW);      
    else gpio_set_level(DATOS, HIGH);
 
     gpio_set_level(CLOCK, HIGH);
     vTaskDelay(1 / portTICK_PERIOD_MS);
     gpio_set_level(CLOCK, LOW);
     gpio_set_level(DATOS, HIGH);
     ESP_LOGI(TAG, "Valor de VAL 1= %u ", val);

    return val;
 

    }


     uint8_t sht10_medicion(uint16_t *p_valor,uint8_t *p_checksum,uint8_t modo)
    {
    uint8_t error=0;
    uint32_t i,v,suma=0;
     
    sht10_init();
    switch(modo)                            
      {
    case 0: 
    error+=sht10_escribir_byte(modo); 
    break;
    case 1: 
    error+=sht10_escribir_byte(modo); 
    break;
    default     : break;
    }
   for (i=0;i<65535;i++) if(gpio_get_level(DATOS)==0) break;  
    if(gpio_get_level(DATOS)) error+=1;



	vTaskDelay(500 / portTICK_PERIOD_MS);

    
   *(p_valor)= sht10_leer_byteH(SHT10_ACK);             
     suma=(*(p_valor))*256; 
    
   *(p_valor)= sht10_leer_byteH(SHT10_ACK);             
     suma=suma+*(p_valor); 
     
   *(p_checksum) =sht10_leer_byteH(SHT10_NOACK);  
     
  
    *(p_valor)=suma;
  
    
    return error;
    }



////////////////////////////////
//LECTURA DEL SENSOR
////////////////////////////////

   void lectura_sht10(void)
    {
       
    errorsht10=0;
     
    
    errorsht10+=sht10_medicion(&humedad.i, &checksum, HUMI);     //medida de humedad
    
    errorsht10+=sht10_medicion( &temperatura.i, &checksum, TEMP); //medida de temperatura
    
   
     sht10_calculos(&humedad.i, &temperatura.f);              //calcula humedad y temperatura 
    
    
    
    if(errorsht10!=0)                                        //si hay error
      {
        sht10_hard_reset();
      }
    
    }

void app_main(void)
{
    gpio_reset_pin(entrada);
    
    gpio_set_direction(entrada, GPIO_MODE_OUTPUT);
    
    

    while (1) {
       
        lectura_sht10();
        vTaskDelay(1000/portTICK_PERIOD_MS);
        sht10_hard_reset();

    }
}


