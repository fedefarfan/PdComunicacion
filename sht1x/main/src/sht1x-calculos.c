#include <stdio.h>
#include "include/sht1x.h"


static const char *TAG = "sht10";

static uint8_t TEMP = 0;
static uint8_t HUMI = 1;

//LLAMA LAS FUNCIONES PARA MEDIR TEMPERATURA Y HUMEDAD. RECIBE LOS DATOS MEDIDOS Y 
//UNA SEÑAL DE ERROR SI LA COMUNICACIÓN FALLA.
void lectura_sht10(void)
{
uint8_t errorsht10=0;
    
errorsht10+=sht10_medicion(&humedad.i, &checksum, HUMI);//LLAMA A LA FUNCIÓN PARA MEDIR HUMEDAD
    
errorsht10+=sht10_medicion( &temperatura.i, &checksum, TEMP);//LLAMA A LA FUNCIÓN PARA MEDIR TEPERATURA
    
sht10_calculos(&humedad.i, &temperatura.i);//SE OBTIENE LOS VALORES DE TEMPERATURA Y HUMEDAD CORREGIDOS
   
    if(errorsht10!=0) //SI HAY ERROR, RESET DEL SENSOR
      {
        sht10_hard_reset();
      }
}

//CÁLCULO DE LA HUMEDAD Y TEMPERATURA MEDIDA POR EL SENSOR
void sht10_calculos(uint16_t *p_humedad,uint16_t *p_temperatura)
{
//CONSTANTES QUE INDICA EL FABRICANTE   
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
     
    t_C = t*0.01 - 40;//VALOR DE TEMP
    rh_lin=C3*rh*rh + C2*rh + C1;//VALOR DE HUM LINEALIZADA

    rh_true=(t_C-25)*(T1+T2*rh)+rh_lin;//VALOR DE HUM CORREGIDA POR TEMPERATURA
    if(rh_true>100)rh_true=100;
    if(rh_true<0.1)rh_true=0.1;
     

   ESP_LOGI(TAG, "HUMEDAD %f ", rh_true);//IMPRIME VALOR DE HUM
   ESP_LOGI(TAG, "TEMPERATURA %f ", t_C);//IMPRIME VALOR DE TEMP
}







