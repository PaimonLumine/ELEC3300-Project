#ifndef INC_DHT11_H_
#define INC_DHT11_H_
#include "stm32f1xx_hal.h"
#include "printf.h"

void DHT11_IO_OUT();
void DHT11_IO_IN();
void DHT11_RST();
uint8_t Dht11_Check();
uint8_t Dht11_ReadBit();
uint8_t Dht11_ReadByte();
uint8_t DHT11_Init();
uint8_t DHT11_ReadData(uint8_t *h);
void dht11_delay_us( __IO uint32_t us);

#endif /* INC_DHT11_H_ */
