#include "esp8266.h"
#include "bsp_usart.h"
#include "printf.h"
#include "lcdtp.h"
#include "ring_buffer.h"
uint8_t esp8266_get_time_flag = 9;

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
void ESP8266_RESET(void)
{
    ESP8266_GPIO_Config();
    WIFI_USART_Config();

    ESP8266_CH_PD_ENABLE();
    ESP8266_RST_ENABLE();
}


void esp8266_cmd_reset(){
	if(!Wait_for("ready")) return;
	HAL_Delay(100);
	printf("AT+RST\r\n");
	esp8266_get_time_flag = 2;
}

void esp8266_cmd_set_station(){
	if(!Wait_for("ready")) return;
	HAL_Delay(100);
	printf("AT+CWMODE=1\r\n");
	esp8266_get_time_flag = 3;
}

void esp8266_cmd_connectWifi(){
	if(!Wait_for("OK")) return;
	HAL_Delay(100);
	extern const char* ssid;
    extern const char* wifi_password;
	printf("AT+CWJAP=\"%s\",\"%s\"\r\n",ssid,wifi_password);

	LCD_Clear(0, 100, 250,150);
	LCD_DrawString(20, 100, "Connecting To Wifi...");

	esp8266_get_time_flag = 4;
}

void esp8266_cmd_tcp(){
	if(!Wait_for("OK")) return;
	HAL_Delay(100);
	printf("AT+CIPSTART=\"TCP\",\"marsohk.pythonanywhere.com\",80\r\n");
	esp8266_get_time_flag = 5;
}

void esp8266_cmd_trans_mode(){
	if(!Wait_for("OK")) return;
	HAL_Delay(100);
	printf("AT+CIPMODE=1\r\n");
	esp8266_get_time_flag = 6;
}


void esp8266_cmd_send_request(){
	if(!Wait_for("OK")) return;
	HAL_Delay(100);
	printf("AT+CIPSEND\r\n");
	LCD_Clear(0, 100, 250,150);
	LCD_DrawString(20, 100, "Sending Request To Server..");

	esp8266_get_time_flag = 7;
}

void esp8266_cmd_fill_request(){
	if(!Wait_for("OK")) return;
	HAL_Delay(100);
	printf("GET / HTTP/1.1\r\nHost: marsohk.pythonanywhere.com\r\n\r\n");
	esp8266_get_time_flag = 8;
}

void esp8266_get_time_buffer(){
	if(!Wait_for("OK")) return;
	HAL_Delay(100);
	extern char USART_DATE_BUFFER[15];

	//LCD_Clear(0, 100, 250,150);
	//LCD_DrawString(20, 100, "Done...");
	Wait_for("DATA:");
	Get_after("DATA:",15, USART_DATE_BUFFER);
	esp8266_get_time_flag = 9;//Done
    ESP8266_CH_PD_DISABLE();
    ESP8266_CH_PD_DISABLE();
}

void esp8266_get_time(){
	switch (esp8266_get_time_flag){
		case 1://Reset
			esp8266_cmd_reset();
			break;
		case 2://Set Station
			esp8266_cmd_set_station();
			break;
		case 3://Connect Wifi
			esp8266_cmd_connectWifi();
			break;
		case 4://Set TCP
			esp8266_cmd_tcp();
			break;
		case 5://SET Trans mode
			esp8266_cmd_trans_mode();
			break;
		case 6://Send Request
			esp8266_cmd_send_request();
			break;
		case 7://Fill Request
			esp8266_cmd_fill_request();
			break;
		case 8://Fill Data to buffer
			esp8266_get_time_buffer();
			break;
		case 0:
			esp8266_get_time_flag = 1;
			ESP8266_RESET();
			break;
	}
}
