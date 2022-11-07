/**
	******************************************************************************
	* @file			bsp_dht11.c
	* @author		Source From https://blog.csdn.net/dingyc_ee/article/details/103530982
	* @date
	* @version	v1.0
	* @note			DHT11 driver
	******************************************************************************
	*/

#include "dht11.h"
#include "printf.h"
void dht11_delay_us( __IO uint32_t us){
	//72MHz/8MHz
	uint32_t delay = 12*us;
	while (delay--);
}

void DHT11_IO_OUT(void) //Set Pin As Output
{
	GPIO_InitTypeDef GPIO_InitStruct ={0};

	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}

void DHT11_IO_IN(void) //Set Pin As Input
{
	GPIO_InitTypeDef GPIO_InitStruct ={0};

	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;

  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}

void DHT11_RST(){
	DHT11_IO_OUT();
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_6,GPIO_PIN_RESET);
	HAL_Delay(20);
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_6,GPIO_PIN_SET);
	dht11_delay_us(30);
}

uint8_t Dht11_Check(){
	uint8_t retry=0;
	DHT11_IO_IN();
	while(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_6)&&retry<100){
		++retry;
		dht11_delay_us(1);
	}
	if(retry>=100)return 1; else retry=0;
	while(!HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_6)&&retry<100){
		++retry;
		dht11_delay_us(1);
	}
	if(retry>=100)return 1;
	return 0;
}

uint8_t DHT11_Init(){
	DHT11_RST();
	return Dht11_Check();
}

uint8_t DHT11_ReadBit(){
	uint8_t retry, i= 0;
	while(!HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_6) &&retry<100){
		++retry;
		dht11_delay_us(1);
	}

	//If Turn to zero -> zero

	for(i=0; i<10;++i){
		dht11_delay_us(1);
		if(!HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_6)){
			return 0;
		}
	}
	retry = 0;
	//Else, Wait Until zero -> one
	while(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_6) && retry < 100){
		++retry;
		dht11_delay_us(1);
	}
	return 1;
}

uint8_t DHT11_ReadByte(){
	uint8_t i,dat;
	dat=0;
	for (i=0;i<8;i++){
		dat<<=1;
		dat|=DHT11_ReadBit();
	}
	return dat;
}


uint8_t DHT11_ReadData(uint8_t *h){
	uint8_t buf[5];
	uint8_t i;
	uint8_t retry = 0;
	//Reset and Check
	if(DHT11_Init()==0){
		while(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_6) &&retry<100){
				++retry;
				dht11_delay_us(1);
			}
		for(i=0;i<5;++i){
			buf[i]=DHT11_ReadByte();
		}
	//i=0,1  = humidity  00.11
	//i=2,3  = temperature 22.33
	//i=4    = Check Sum 0+1+2+3==4
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4]){
		for(i=0;i<4;++i){
			*(h++) = buf[i];
			}
		}
	}else return 1;
	return 0;
}

