#include <stdio.h>
#include "include/sht1x.h"


static const char *TAG = "sht10";

static uint8_t LOW = 0;
static uint8_t HIGH = 1;


//CONFIGURA EL PIN SELECCIONADO (CLOCK) COMO SALIDA, RELOJ DE LA SEÑAL 
static void configure_CLOCK(void)
{
    gpio_reset_pin(CLOCK);
    gpio_set_direction(CLOCK, GPIO_MODE_OUTPUT);
    gpio_set_level(CLOCK, LOW);
}

//CONFIGURA EL PIN SELECCIONADO (DATOS) COMO SALIDA, PARA ENVIAR INFO AL SENSOR
static void configure_DATOS_OUT(void)
{
    gpio_reset_pin(DATOS);
    gpio_set_direction(DATOS, GPIO_MODE_OUTPUT);
    gpio_set_level(DATOS, LOW);
 }   

//CONFIGURA EL PIN DE SELECCIONADO (DATOS) COMO ENTRADA, PARA LEER LA INFO DEL SENSOR 
static void configure_DATOS_IN(void)
{
    gpio_reset_pin(DATOS);
    gpio_set_direction(DATOS, GPIO_MODE_INPUT);
}


//ENVIA LA TRAMA DE INICIO DEL SENSOR
//TRAMA DE INICIO ___    ___  
//   CLOCK     __/  /___/  /___
//            ____       ____    
//  DATOS        /______/   
//CONFIGURA PUERTOS COMO SALIDA
void sht10_init(void)
{
    configure_CLOCK();//CONFIGURA EL PIN DE CLOCK
    configure_DATOS_OUT();//CONFIGURA EL PIN DE SALIDA PARA ENVIAR TRAMA    
    
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

//LLAMA LAS FUNCIONES PARA CONFIGURARA LAS MEDIDA.
//DEPENDIENDO DE modo, SE INICIA LA MEDIDA DE TEMPERATURA O HUMEDAD
// RETORNA EL PARAMETRO QUE INDICA SI HAY ERROR EN LA CONFIGURACIÓN
uint8_t sht10_medicion(uint16_t *p_valor,uint8_t *p_checksum,uint8_t modo)
{
    uint8_t error=0;
    uint32_t i,suma=0;
     
    sht10_init();
    switch(modo)   //SELECCIONA SI MEDIMOS TEMPERATURA O HUMEDAD                         
      {
    case 0: 
    error+=sht10_escribir_byte(modo); 
    break;
    case 1: 
    error+=sht10_escribir_byte(modo); 
    break;
    default     : break;
    }
//ESPERA QUE FINALICE LA MEDIDA
   for (i=0;i<65535;i++) if(gpio_get_level(DATOS)==0) break;  
    if(gpio_get_level(DATOS)) error+=1;
	vTaskDelay(500 / portTICK_PERIOD_MS);

//RECIBE LOS PRIMEROS 8 BIT DE LA MEDIDA    
   *(p_valor)= sht10_leer_byteH(SHT10_ACK);             
     suma=(*(p_valor))*256; 
//RECIBE LOS SEGUNDOS 8 BIT DE LA MEDIDA    
   *(p_valor)= sht10_leer_byteH(SHT10_ACK);             
     suma=suma+*(p_valor); 
//RECIBE LOS ULTIMOS 8 BIT DE LA MEDIDA (CRC)     
   *(p_checksum) =sht10_leer_byteH(SHT10_NOACK);  
     
   *(p_valor)=suma;//MEDIDA
     
return error;//DEVUELVE ERROR
}

//MANDA EL TREN DE PULSO PARA SELECCIONAR LA MEDIDA DE TEMPERATURA O HUMEDAD
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
     
for (i=128;i>0;i/=2) //DEPENDIENDO DE value1, MMANDA 8 BITS, 00000101 PARA HUM O 00000011 PARA TEMP.
{
    if (i & value)  gpio_set_level(DATOS, HIGH);//ENVIA 1  
    else  gpio_set_level(DATOS, LOW);//ENVIA 0
    
    gpio_set_level(CLOCK, HIGH);
    vTaskDelay(1 / portTICK_PERIOD_MS);
    gpio_set_level(CLOCK, LOW);
}

      gpio_set_level(DATOS, HIGH);
      gpio_set_level(CLOCK, HIGH);
      configure_DATOS_IN(); //CAMBIA DATOS COMO ENTRADA   
      error=error+gpio_get_level(DATOS);//MIDE ACK
      gpio_set_level(CLOCK, LOW);
    
    
    if(value==5)//CARTEL QUE INDICA SI LA CONFIGURACIÓN PARA LA MEDIDA DE HUM FUE EXITOSA
    {
    if(error==0)
    {
    ESP_LOGI(TAG, "ACK1-Configuracion exitosa (HUMEDAD) %u", error);
    }else
    {
    ESP_LOGI(TAG, "ACK1-Fallo configuracion , (HUMEDAD) %u", error);
    }
    }else{//CARTEL QUE INDICA SI LA CONFIGURACIÓN PARA LA MEDIDA DE TEMP FUE EXITOSA
    
    if(error==0)
    {
    ESP_LOGI(TAG, "ACK1-Configuracion exitosa (TEMPERATURA) %u", error);
    }else
    {
    ESP_LOGI(TAG, "ACK1-Fallo configuracion , (TEMPERATURA) %u", error);
    }
    
    }

return error;//DEVUELVE ERROR     
}


//LEE LA INFORMACIÓN QUE ENVIA EL SENSOR. SE REALIZA DE A 8 BITS
//TRAMA QUE DEVUELVE EL SENSOR
//    __   __   __   __   __   __   __   __   __   __   __   __   __   __   __   __   __   __  
// __/ /__/ /__/ /__/ /__/ /__/ /__/ /__/ /__/ /__/ /__/ /__/ /__/ /__/ /__/ /__/ /__/ /__/ /__
//    1    2    3    4    5   6    7    8    ACK   1    2    3    4    5    6    7    8   ACK
//  <----------  8 BITS (MEDIDA) ------->        <----------  8 BITS (MEDIDA) ------->   
//    __   __   __   __   __   __   __   __   __    
// __/ /__/ /__/ /__/ /__/ /__/ /__/ /__/ /__/ /__
//    1    2    3    4    5   6    7    8    ACK  
//  <----------  8 BITS (CRC)  --------->         


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
     ESP_LOGI(TAG, "Valor de VAL 1= %u ", val);//IMPRIME EL VALOR OBTENIDO

    return val;
}

//RESET DEL SENSOR  
//    __   __   __   __   __   __   __   __   __   __   __     
// __/ /__/ /__/ /__/ /__/ /__/ /__/ /__/ /__/ /__/ /__/ /________
//    1    2    3    4    5    6    7    8    9   
//    ____________________________________________     ___________    
// __/                                           /____/    
//                 
//  <----------     DATOS       ---------><-INICIO DE TRAMA-> 
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
    
    







