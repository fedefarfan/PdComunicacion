/*
 * sht1x.c
 *
 *  Created on: 15 Junio 2022
 *      Author: Roberto Federico Farfan
 */

#ifndef DRIVER_SHT1X_B_H_
#define DRIVER_SHT1X_B_H_

#include "main.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>


void Gpio_Data_Out_Config(void);
void Gpio_DATA_In_Config(void);
void Gpio_SCK_Out_Config(void);
void Gpio_DATA_Level(bool level);
bool Gpio_DATA_Read(void);
void Gpio_SCK_Level(bool level);
void Queue_Time(uint32_t t);
void imprime_temp_hum(float rh_true1,float t_C1);

#endif
