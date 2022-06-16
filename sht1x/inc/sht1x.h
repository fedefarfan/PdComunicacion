/*
 * sht1x.c
 *
 *  Created on: 15 Junio 2022
 *      Author: Roberto Federico Farfan
 */
#define SHT10_ACK   1
#define SHT10_NOACK 0

void lectura_sht10(void);
void sht10_calculos(uint16_t *p_humedad,uint16_t *p_temperatura);
void Sht10_Init(void);
uint8_t sht10_medicion(uint16_t *p_valor,uint8_t *p_checksum,uint8_t modo);
uint8_t sht10_escribir_byte(uint8_t value1);
uint16_t sht10_leer_byteH(uint8_t ack);
void sht10_hard_reset(void);

