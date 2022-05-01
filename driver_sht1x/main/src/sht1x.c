#include "include/sht1x.h"
#include "include/sht1x-puertos.h"



//static const char *TAG = "sht10";

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
     
    imprime_temp_hum(rh_true,t_C);

}
//ENVIA LA TRAMA DE INICIO DEL SENSOR 
//TRAMA DE INICIO ___    ___  
//   CLOCK     __/  /___/  /___
//            ____       ____    
//  DATOS        /______/   
//CONFIGURA PUERTOS COMO SALIDA
void sht10_init(void)
{
    gpio_set_direction_clock();//CONFIGURA EL PIN DE CLOCK
    
    gpio_set_direction_data_out();//CONFIGURA EL PIN DE SALIDA PARA ENVIAR TRAMA    

    _gpio_level_data(HIGH);
    _gpio_level_clock(LOW);
    espera(1);
    _gpio_level_clock(HIGH);  
    espera(1);
    _gpio_level_data(LOW);
    espera(1);
    _gpio_level_clock(LOW);
    espera(1);    
    _gpio_level_clock(HIGH);
    espera(1); 
    _gpio_level_data(HIGH);
    espera(1);
    _gpio_level_clock(LOW);
    espera(1);
    _gpio_level_data(LOW);
    espera(1);
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
   for (i=0;i<65535;i++) if(gpio_set_level_data()==0) break;
    if(gpio_set_level_data()) error+=1;
	espera(500);

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
    if (i & value)  _gpio_level_data(HIGH);//ENVIA 1  
    else  _gpio_level_data(LOW);//ENVIA 0
    
    _gpio_level_clock(HIGH);
    espera(1);
    _gpio_level_clock(LOW);
}

      _gpio_level_data(HIGH);
      _gpio_level_clock(HIGH);
      gpio_set_direction_data_in();//CAMBIA DATOS COMO ENTRADA  
      error=error+gpio_set_level_data();//MIDE ACK
      _gpio_level_clock(LOW);
    

    cartel_resultado_congfig(value,error);//IMPRIME EL RESULTADO DE LA CONFIGURACION DE LA MEDIDA SELECCIONADA
    


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
    _gpio_level_data(HIGH);  
    gpio_set_direction_data_in();
    
    
    for (i=128;i>0;i/=2)          
      {
    _gpio_level_clock(HIGH);
             
    if(gpio_set_level_data())
                        val=(val | i);  
    _gpio_level_clock(LOW);
   
    }    
    gpio_set_direction_data_out();

      if (ack) _gpio_level_data(LOW);      
    else _gpio_level_data(HIGH);
 
     _gpio_level_clock(HIGH);
     espera(1);
     _gpio_level_clock(LOW);
     _gpio_level_data(HIGH);
     
     cartel_config(val);//IMPRIME EL VALOR OBTENIDO

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
     
    _gpio_level_clock(LOW);
    _gpio_level_data(HIGH);   
    espera(1);
  
    for(i=0;i<9;i++)            
    {
    _gpio_level_clock(HIGH);
    espera(1);
    _gpio_level_clock(LOW);
    espera(1);
    }
               
}
    
    







