#Libreia para el sensor SHT1X. Compilar con ESP-IDF 

Esta libreria permite la comunicación entre el ESP32 y el sensor de temperatura y humedad SHT1X.

Los archivos .c en el proyecto se encuentran definidos como:

			set(srcs "main.c"
				"src/sht1x.c"
				"src/sht1x-puertos.c"
			)

			idf_component_register(SRCS ${srcs} INCLUDE_DIRS ".")

La libreria utiliza los archivos main.c, sht1X.c y sht1x-puertos.c, que se decriben.



archivo main.c : llama a la función lectura_sht10(). Esta invoca las funciones que configuran el sensor y recibe la medida de temperatura y humedead.



archivo sht1x-puertos.c: Este maneja el micro en bajo nivel, permite la configuración de los pines DATOS como entarda y salida, mientras que configura el pin CLOCK como salida.

#define DATOS  22
#define CLOCK  23
 
void gpio_set_direction_clock(void); //CONFIGURA EL PIN 23 COMO SALIDA, RELOJ DE LA SEÑAL

void gpio_set_direction_data_out(void);//CONFIGURA EL PIN 22 COMO SALIDA, PARA ENVIAR INFORMACION AL SENSOR
 
void gpio_set_direction_data_in(void);//CONFIGURA EL PIN DE 22 COMO ENTRADA, PARA LEE LA INFO DEL SENSOR

void _gpio_level_data(uint8_t level);//ENVIA UNO O CERO AL PUERTO DATOS 

void _gpio_level_clock(uint8_t level);//ENVIA UNO O CERO AL PUERTO CLOCK 

uint8_t gpio_set_level_data(); //LEE POR EL PUERTO DATOS LA INFORMACIÓN QUE ENVIA EL SENSOR 



archivo sht1x.c: Aqui se definen las funciones genericas del driver. Las funciones definidas en este archivo son las siguientes:

void sht10_init(void); //ENVIA LA TRAMA DE INICIO DEL SENSOR

uint8_t sht10_escribir_byte(uint8_t value1); //MANDA EL TREN DE PULSO PARA SELECCIONAR LA MEDIDA DE TEMPERATURA O HUMEDAD

uint16_t sht10_leer_byteH(uint8_t ack); //LEE LA INFORMACIÓN QUE ENVIA EL SENSOR. SE REALIZA DE A BYTES

void sht10_hard_reset(void); //RESET DEL SENSOR    

uint8_t sht10_medicion(uint16_t *p_valor,uint8_t *p_checksum,uint8_t modo); //LLAMA LAS FUNCIONES PARA CONFIGURARA LAS MEDIDA. DEPENDIENDO DE modo, SE INICIA LA MEDIDA DE TEMPERATURA O HUMEDAD. RETORNA EL PARAMETRO QUE INDICA SI HAY ERROR EN LA CONFIGURACIÓN

void lectura_sht10(void); //LLAMA LAS FUNCIONES PARA MEDIR TEMPERATURA Y HUMEDAD. RECIBE LOS DATOS MEDIDOS Y UNA SEÑAL DE ERROR SI LA COMUNICACIÓN FALLA.

void sht10_calculos(uint16_t *p_humedad,uint16_t *p_temperatura); //CÁLCULO EL VALOR DE HUMEDAD Y TEMPERATURA MEDIDA POR EL SENSOR 
