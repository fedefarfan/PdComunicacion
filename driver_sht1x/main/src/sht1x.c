#include "include/sht1x.h"
#include "include/sht1x-puertos.h"



static const char *TAG = "sht10";

static uint8_t LOW = 0;
static uint8_t HIGH = 1;

static uint8_t TEMP = 0;
static uint8_t HUMI = 1;

//LLAMA LAS FUNCIONES PARA MEDIR TEMPERATURA Y HUMEDAD. RECIBE LOS DATOS MEDIDOS Y 
//UNA SEÑAL DE ERROR SI LA COMUNICACIÓN FALLA.
void lectura_sht10(void)
{
errorsht10=0;
    
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
//ENVIA LA TRAMA DE INICIO DEL SENSOR 
//TRAMA DE INICIO ___    ___  
//   CLOCK     __/  /___/  /___
//            ____       ____    
//  DATOS        /______/   
//CONFIGURA PUERTOS COMO SALIDA
void sht10_init(void)
{
    gpio_set_direction_clock();
    //configure_CLOCK();//CONFIGURA EL PIN DE CLOCK
    gpio_set_direction_data_out();
    //configure_DATOS_OUT();//CONFIGURA EL PIN DE SALIDA PARA ENVIAR TRAMA    
    
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
      gpio_set_direction_data_in();
      //configure_DATOS_IN(); //CAMBIA DATOS COMO ENTRADA   
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
    
    gpio_set_direction_data_out();
    //configure_DATOS_OUT(); 
    gpio_set_level(DATOS, HIGH);  
    gpio_set_direction_data_in();
    //configure_DATOS_IN(); 
    
    for (i=128;i>0;i/=2)          
      {
    gpio_set_level(CLOCK, HIGH);
             
    if(gpio_get_level(DATOS))
                        val=(val | i);  
      gpio_set_level(CLOCK, LOW);
   
    }    
    gpio_set_direction_data_out();
//        configure_DATOS_OUT();   
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
    
    






