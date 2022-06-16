#Libreia para el sensor SHT1X.

Esta libreria permite la comunicación entre la STM32f429ZI y el sensor de temperatura y humedad SHT1X.


La libreria utiliza los archivos main.c, sht1X.c y sht1x_puertos.c.


archivo main.c : llama a la función lectura_sht10(). Esta invoca las funciones que configuran el sensor y recibe la medida de temperatura y humedead.


archivo sht1x_puertos.c: Este maneja el micro en bajo nivel, permite la configuración de los pines DATOS como entarda y salida, mientras que configura el pin CLOCK como salida.

Se tiliza el pin GPIO_PIN_13 como DATA.

Se utiliza el pin GPIO_PIN_14 como SCK
 

