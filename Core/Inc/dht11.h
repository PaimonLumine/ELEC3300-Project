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


typedef struct
{
	uint8_t humid_int,
		  humid_dec,
          temp_int,
          temp_dec,
		  check_sum;

} DHT11_datastruct;

uint8_t DHT11_ReadData(DHT11_datastruct *ds);

#endif /* INC_DHT11_H_ */
