/*
 * sht1x.c
 *
 *  Created on: 15 Junio 2022
 *      Author: Roberto Federico Farfan
 */
#include "../../Drivers/sht1x/inc/sht1x_puertos.h"
#include "../../Drivers/sht1x/inc/sht1x.h"

static bool LOW = 0;
static bool HIGH = 1;

static uint8_t TEMP = 0;
static uint8_t HUMI = 1;


    typedef union
    {
      uint16_t i;
      uint32_t f;
    } valor;
    valor humedad, temperatura;

uint8_t errorsht10,checksum;


/*************************************************************************************************
	 *  @brief Funcion para ejecutar las tareas medir temperatura y humedad
     *
     *  @details
     *   	Llama a las funciones para medir temperatura y humedad
     *   	Recibe los dato medidos y la señal error
     *   	La señal error puede indicar que no se pudo configurar el sensor o que el sensor no
     *   	pudo realizar la medida de temperatura o humedad
     *
	 *  @param		None.
	 *  @return     None.
***************************************************************************************************/
void lectura_sht10(void){

	errorsht10=0;

	errorsht10+=sht10_medicion(&humedad.i, &checksum, HUMI);//LLAMA A LA FUNCIÓN PARA MEDIR HUMEDAD

	errorsht10+=sht10_medicion( &temperatura.i, &checksum, TEMP);//LLAMA A LA FUNCIÓN PARA MEDIR TEPERATURA

	sht10_calculos(&humedad.i, &temperatura.i);//SE OBTIENE LOS VALORES DE TEMPERATURA Y HUMEDAD CORREGIDOS

    if(errorsht10!=0){
    	sht10_hard_reset();
    }
}

/*************************************************************************************************
	 *  @brief Funcion para calcular la humedad y temperatura medida por el sht1X
     *
     *  @details
     *   	La función determina los valores de temperatura y humedad en base a la la informacion
     *   	que envia el sensor y a las constates que el fabricante indica
     *
	 *  @param		p_humedad	valor que entrega el sensor al realizar la lectura de humedad
	 *  			p_temperatura	valor que entrega el sensor al realizar la lectura de temperatura
	 *  @return     None.
***************************************************************************************************/
void sht10_calculos(uint16_t *p_humedad,uint16_t *p_temperatura){
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
/*************************************************************************************************
	 *  @brief Funcion para inicializar el sensor sht1X
     *
     *  @details
     *   	En la función se pide configurar el puerto DATA y SCK como salida para enviar las tramas
     *   	de inicio del sht1X
     *   	          __     __
     *   CLOCK     __/  /___/  /___
     *            ____       ____
     *   DATOS       /______/
     *
	 *  @param		None.
	 *  @return     None.
***************************************************************************************************/
void Sht10_Init(void){

	Gpio_SCK_Out_Config();//gpio_set_direction_clock();//CONFIGURA EL PIN DE CLOCK

	Gpio_Data_Out_Config();// gpio_set_direction_data_out();//CONFIGURA EL PIN DE SALIDA PARA ENVIAR TRAMA

	Gpio_DATA_Level(HIGH);
	Gpio_SCK_Level(LOW);
	Queue_Time(1);
	Gpio_SCK_Level(HIGH);
	Queue_Time(1);
	Gpio_DATA_Level(LOW);
	Queue_Time(1);
	Gpio_SCK_Level(LOW);
	Queue_Time(1);
	Gpio_SCK_Level(HIGH);
	Queue_Time(1);
	Gpio_DATA_Level(HIGH);
	Queue_Time(1);
	Gpio_SCK_Level(LOW);
	Queue_Time(1);
	Gpio_DATA_Level(LOW);
	Queue_Time(1);
}

/*************************************************************************************************
	 *  @brief Funcion que ejecuta las tareas para configurar las lecturas de humedad y temperatura
	 *  realizadas por el sensor
     *
     *  @details
     *   	La función ejecuta la funcion uint8_t sht10_escribir_byte(uint8_t value1) para indicar que
     *   	variable fisica se desea medir y recibe el valor medido enviado por el sht1X
     *
	 *  @param		p_valor		Valor obtenido de la medicion
	 *  			p_checksum		Calculo del CRC
	 *  			modo		Valor que indica si va a realizar la medida de tempertura o humedad
	 *  @return     error		Devuelve el ACK
***************************************************************************************************/
uint8_t sht10_medicion(uint16_t *p_valor,uint8_t *p_checksum,uint8_t modo){
	uint8_t error=0;
	uint32_t i,suma=0;

	Sht10_Init();
	switch(modo){   //SELECCIONA SI MEDIMOS TEMPERATURA O HUMEDAD

		case 0:
			error+=sht10_escribir_byte(modo);
			break;
		case 1:
			error+=sht10_escribir_byte(modo);
			break;
		default:
			break;
    }
//ESPERA QUE FINALICE LA MEDIDA
	for (i=0;i<65535;i++) if(Gpio_DATA_Read()==0) break;
	if(Gpio_DATA_Read()) error+=1;
	Queue_Time(800);//espera(500);

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


/*************************************************************************************************
	 *  @brief Funcion que envia un tren de pulso al sht1X para indicar que magnitud debe medir
	 *
     *  @details
     *   	La funcion envia un tren de pulso que indica que magnitud debe medirse
     *
	 *  @param		value1		valor que indica si se va a medir temperatura o humedad
	 *   @return    error		indica si los valores enviados son correctos o no
***************************************************************************************************/
uint8_t sht10_escribir_byte(uint8_t value1){
	uint8_t i,error=0,value=0;

	if(value1==0){
		value=3;
	}

	if(value1==1){
		value=5;
	}

	for (i=128;i>0;i/=2){ //DEPENDIENDO DE value1, MMANDA 8 BITS, 00000101 PARA HUM O 00000011 PARA TEMP.

		if (i & value)  Gpio_DATA_Level(HIGH);//ENVIA 1
		else  Gpio_DATA_Level(LOW);//ENVIA 0

		Gpio_SCK_Level(HIGH);
		Queue_Time(1);
		Gpio_SCK_Level(LOW);
	}

	Gpio_DATA_Level(HIGH);
	Gpio_SCK_Level(HIGH);
	Gpio_DATA_In_Config();//CAMBIA DATOS COMO ENTRADA
	error=error+Gpio_DATA_Read();//MIDE ACK
	Gpio_SCK_Level(LOW);
return error;
}

/*************************************************************************************************
	 *  @brief Funcion que lee la trama que envia el sensor al realizar una medida de humedad
	 *  o temperatura
	 *
     *  @details
     *   	La función realiza una lectura de cada bit formando numeros de 1bytes
     *
     *
     *TRAMA QUE DEVUELVE EL SENSOR
     *       __   __   __   __   __   __   __   __   __   __   __   __   __   __   __   __   __   __
     *    __/ /__/ /__/ /__/ /__/ /__/ /__/ /__/ /__/ /__/ /__/ /__/ /__/ /__/ /__/ /__/ /__/ /__/ /__
     *       1    2    3    4    5   6    7    8    ACK   1    2    3    4    5    6    7    8   ACK
     *     <----------  8 BITS (MEDIDA) ------->        <----------  8 BITS (MEDIDA) ------->
     *      __   __   __   __   __   __   __   __   __
     *   __/ /__/ /__/ /__/ /__/ /__/ /__/ /__/ /__/ /__
     *      1    2    3    4    5   6    7    8    ACK
     *    <----------  8 BITS (CRC)  --------->
     *
	 *  @param		ack		para control de lectura
	 *  @return     val		lectura de 1 Byte
***************************************************************************************************/
uint16_t sht10_leer_byteH(uint8_t ack){
uint16_t i,val=0;

Gpio_Data_Out_Config();
Gpio_DATA_Level(HIGH);
Gpio_DATA_In_Config();


    for (i=128;i>0;i/=2)
      {
    	Gpio_SCK_Level(HIGH);

    if(Gpio_DATA_Read())
                        val=(val | i);
        Gpio_SCK_Level(LOW);

    }
    Gpio_Data_Out_Config();

      if (ack) Gpio_DATA_Level(LOW);
    else Gpio_DATA_Level(HIGH);

      Gpio_SCK_Level(HIGH);
      Queue_Time(1);
      Gpio_SCK_Level(LOW);
      Gpio_DATA_Level(HIGH);

    return val;
}

/*************************************************************************************************
	 *  @brief Funcion que lee la trama que envia el sensor al realizar una medida de humedad
	 *  o temperatura
	 *
     *  @details
     *   	La función realiza una lectura de cada bit formando numero de 1bytes
     *    __   __   __   __   __   __   __   __   __   __   __
     * __/ /__/ /__/ /__/ /__/ /__/ /__/ /__/ /__/ /__/ /__/ /________
     *    1    2    3    4    5    6    7    8    9
     *    ____________________________________________     ___________
     * __/                                           /____/
     * <----------     DATOS       ---------><-INICIO DE TRAMA->
     *
	 *  @param		None.
	 *  @return     None.
***************************************************************************************************/
void sht10_hard_reset(void){
	uint8_t i;

	Gpio_SCK_Level(LOW);
	Gpio_DATA_Level(HIGH);
	Queue_Time(1);

	for(i=0;i<9;i++){
		Gpio_SCK_Level(HIGH);
		Queue_Time(1);
		Gpio_SCK_Level(LOW);
		Queue_Time(1);
    }
}


