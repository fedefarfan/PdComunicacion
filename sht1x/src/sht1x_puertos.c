/*
 * sht1x.c
 *
 *  Created on: 15 Junio 2022
 *      Author: Roberto Federico Farfan
 */
#include "../../Drivers/sht1x/inc/sht1x_puertos.h"



/*************************************************************************************************
	 *  @brief Funcion para configurar el pin GPIO (DATOS) como salida
     *
     *  @details
     *   	Configura el pin GPIO como salida para indicar al sensor que funcion debe ejecutar
     *
	 *  @param		None.
	 *  @return     None.
***************************************************************************************************/
void Gpio_Data_Out_Config(void){

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);
	/*Configure GPIO pin : PF13 D7 out*/
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
}

/*************************************************************************************************
	 *  @brief Funcion para configurar el pin GPIO (DATOS) como entrada
     *
     *  @details
     *   	Configura el pin GPIO como entrada para recibir del sensor paramatros de control
     *   	o la medida de temperatura y humedad
     *
	 *  @param		None.
	 *  @return     None.
***************************************************************************************************/
void Gpio_DATA_In_Config(void){

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	/*Configure GPIO pin : PF13 D7 in*/
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
}

/*************************************************************************************************
	 *  @brief Funcion para configurar el pin GPIO (SCK) como salida
     *
     *  @details
     *   	Configura el pin GPIO como salida para sincronizar la comunicacion entre el sensor
     *   	y el microcontrolador
     *
	 *  @param		None.
	 *  @return     None.
***************************************************************************************************/
void Gpio_SCK_Out_Config(void){

	 GPIO_InitTypeDef GPIO_InitStruct = {0};
     HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, GPIO_PIN_RESET);
	  /*Configure GPIO pin : PF14 */
	  GPIO_InitStruct.Pin = GPIO_PIN_14;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
}

/*************************************************************************************************
	 *  @brief Funcion para enviar ceros o unos por el pin DATA
     *
     *  @details
     *   	Esta función envia ceros o uno para indicar al sensor que tarea realizar
     *
	 *  @param		level	El estado del pin DATA, alto o bajo
	 *  @return     None.
***************************************************************************************************/
void Gpio_DATA_Level(bool level){

	if(level)
		HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);
}

/*************************************************************************************************
	 *  @brief Funcion para enviar ceros o unos por el pin SCK
     *
     *  @details
     *   	Esta función envia ceros o uno para indicar al sensor cuando interpretar ceros y uno
     *   	en el puerto DATA
     *
	 *  @param		level	El estado del pin SCK, alto o bajo
	 *  @return     None.
***************************************************************************************************/
void Gpio_SCK_Level(bool level){

	if(level)
		HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, GPIO_PIN_RESET);
}

/*************************************************************************************************
	 *  @brief Funcion para leer el estado del pin DATA
     *
     *  @details
     *   	Esta función lee el estado del pin DATA
     *   	La informacin la envia el sensor para control o lectura de la temperatura y humedad
     *
	 *  @param		None.
	 *  @return     El estado del pin DATA, alto o bajo
***************************************************************************************************/
bool Gpio_DATA_Read(void){

	return HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_13);
}

/*************************************************************************************************
	 *  @brief Funcion que implementa un delay simple de 1 milisegundo
     *
     *  @details
     *   	Esta funcion es especifica para el hardware utilizado.
     *
	 *  @param		millisecs	Los milisegundos a efectuar el delay.
	 *  @return     None.
	 *  @see		delay (implementacion de sAPI).
***************************************************************************************************/
void Queue_Time(uint32_t t){

	HAL_Delay(t);
}



