#include "esp8266.h"
#include "bsp_usart.h"

//CH-PD 和RST 引脚配置
void ESP8266_GPIO_Config()
{
    //GPIO结构体
    GPIO_InitTypeDef  GPIO_InitStruct;
    //开启外设时钟
    ESP8266_CH_PD_CLK_ENABLE();
    ESP8266_RST_CLK_ENABLE();

    GPIO_InitStruct.Pin = ESP8266_CH_PD_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(ESP8266_CH_PD_PORT,&GPIO_InitStruct);

    GPIO_InitStruct.Pin = ESP8266_RST_PIN;
    HAL_GPIO_Init(ESP8266_RST_PORT,&GPIO_InitStruct);
}

//esp8266初始化 main调用
void ESP8266_Init(void)
{
    ESP8266_GPIO_Config();
    WIFI_USART_Config();

    //CHPD RST 初始化为高电平
    ESP8266_CH_PD_ENABLE();
    ESP8266_RST_ENABLE();
}
