#ifndef ESP8266_H
#define ESP8266_H

#include "stm32f1xx.h"
#include <stdio.h>
/*
ESP8266 IO  <------>    STM32 IO
URXD                    PB10（tx）
UTXD                    PB11 (rx)
CH-PD                   PB8
RST                     PB9
*/
/******* ESP8266 pins *************/
//8266 CH_PD引脚
#define ESP8266_CH_PD_PORT                         GPIOB
#define ESP8266_CH_PD_PIN                          GPIO_PIN_8
#define ESP8266_CH_PD_CLK_ENABLE()                 __HAL_RCC_GPIOB_CLK_ENABLE()

//8266 RST引脚
#define ESP8266_RST_PORT                           GPIOB
#define ESP8266_RST_PIN                            GPIO_PIN_9
#define ESP8266_RST_CLK_ENABLE()                    __HAL_RCC_GPIOB_CLK_ENABLE()

#define ESP8266_CH_PD_ENABLE()                     HAL_GPIO_WritePin(ESP8266_CH_PD_PORT,ESP8266_CH_PD_PIN,GPIO_PIN_SET)
#define ESP8266_RST_ENABLE()                       HAL_GPIO_WritePin(ESP8266_RST_PORT,ESP8266_RST_PIN,GPIO_PIN_SET)

#define ESP8266_CH_PD_DISABLE()                     HAL_GPIO_WritePin(ESP8266_CH_PD_PORT,ESP8266_CH_PD_PIN,GPIO_PIN_RESET)
#define ESP8266_RST_DISABLE()                       HAL_GPIO_WritePin(ESP8266_RST_PORT,ESP8266_RST_PIN,GPIO_PIN_RESET)


//functions
extern UART_HandleTypeDef huart3;

void ESP8266_RESET(void);
void esp8266_get_time();
void esp8266_update_water();
void esp8266_update_exercise();
#endif
