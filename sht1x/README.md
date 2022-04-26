#Libreia para el sensor SHT1X. Compilar con ESP-IDF 

Esta libreria permite la comunicación entre el ESP32 y un sensor de temperatura y humedad SHT1X

La libreria contiene los siguientes archivos:


archivo main.c : solo llama a la función lectura_sht10(). Esta configura el sensor y recibe la medida de temperatura y humedead.


archivo sht1x.c: tiene las funciones donde el puerto de comunicación (pin DATOS), cambia constantemente, ya sea para leer información que envia el sensor o configurara el mismo (escritura). Las funciones definidas en este archivo son las siguientes:

	static void configure_CLOCK(void); CONFIGURA EL PIN SELECCIONADO (CLOCK) COMO SALIDA, RELOJ DE LA SEÑAL.	

	static void configure_DATOS_OUT(void); CONFIGURA EL PIN SELECCIONADO (DATOS) COMO SALIDA, PARA ENVIAR INFO AL SENSOR
	
 	static void configure_DATOS_IN(void); CONFIGURA EL PIN DE SELECCIONADO (DATOS) COMO ENTRADA, PARA LEER LA INFO DEL SENSOR 
	
	void sht10_init(void); ENVIA LA TRAMA DE INICIO DEL SENSOR

	uint8_t sht10_escribir_byte(uint8_t value1); MANDA EL TREN DE PULSO PARA SELECCIONAR LA MEDIDA DE TEMPERATURA O HUMEDAD

	uint16_t sht10_leer_byteH(uint8_t ack); LEE LA INFORMACIÓN QUE ENVIA EL SENSOR. SE REALIZA DE A BYTES

	void sht10_hard_reset(void); RESET DEL SENSOR    

	uint8_t sht10_medicion(uint16_t *p_valor,uint8_t *p_checksum,uint8_t modo); LLAMA LAS FUNCIONES PARA CONFIGURARA LAS MEDIDA. DEPENDIENDO DE modo, SE INICIA LA MEDIDA DE TEMPERATURA O HUMEDAD. RETORNA EL PARAMETRO QUE INDICA SI HAY ERROR EN LA CONFIGURACIÓN

archivo sht1x-calculos.c: contiene las funciones donde no se cambia la configuración de los puertos. Aqui las funciones invocan a otras para realizar la comunicación y calculos de la temperatura y humedad medida. Las funciones definidas en el archivo son las siguientes:

	void lectura_sht10(void);LLAMA LAS FUNCIONES PARA MEDIR TEMPERATURA Y HUMEDAD. RECIBE LOS DATOS MEDIDOS Y 
	UNA SEÑAL DE ERROR SI LA COMUNICACIÓN FALLA.

	void sht10_calculos(uint16_t *p_humedad,uint16_t *p_temperatura); CÁLCULO DE LA HUMEDAD Y TEMPERATURA MEDIDA POR EL SENSOR

archivo sht1x.h: se definen variables y los pines de comunicación con el sensor; DATOS (pin 22) y CLOCK (pin 23). 

    


     


