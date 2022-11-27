#include "esp8266.h"
#include "bsp_usart.h"
#include "printf.h"
#include "lcdtp.h"
#include "ring_buffer.h"
#include "rtc.h"
uint8_t esp8266_step_flag = 9;

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

    ESP8266_CH_PD_DISABLE();
    ESP8266_CH_PD_DISABLE();

    HAL_Delay(100);

    ESP8266_CH_PD_ENABLE();
    ESP8266_RST_ENABLE();
}


void esp8266_cmd_reset(){
	if(!IsDataAvailable()) return;
	if(!Wait_for("ready")) return;
	//HAL_Delay(100);
	printf("AT+RST\r\n");
	++esp8266_step_flag;
}

void esp8266_cmd_set_station(){
	if(!IsDataAvailable()) return;
	if(!Wait_for("ready")) return;
	//HAL_Delay(100);
	printf("AT+CWMODE=1\r\n");
	++esp8266_step_flag;
}

void esp8266_cmd_connectWifi(){
	if(!IsDataAvailable()) return;
	if(!Wait_for("OK")) return;
	//HAL_Delay(100);
	extern const char* ssid;
    extern const char* wifi_password;
	printf("AT+CWJAP=\"%s\",\"%s\"\r\n",ssid,wifi_password);
	++esp8266_step_flag;
}

void esp8266_cmd_tcp(){
	if(!IsDataAvailable()) return;
	if(!Wait_for("OK")) return;
	//HAL_Delay(100);
	printf("AT+CIPSTART=\"TCP\",\"marsohk.pythonanywhere.com\",80\r\n");
	++esp8266_step_flag;
}

void esp8266_cmd_trans_mode(){
	if(!IsDataAvailable()) return;
	if(!Wait_for("OK")) return;
	//HAL_Delay(100);
	printf("AT+CIPMODE=1\r\n");
	++esp8266_step_flag;
}


void esp8266_cmd_send_request(){
	if(!IsDataAvailable()) return;
	if(!Wait_for("OK")) return;
	//HAL_Delay(100);
	printf("AT+CIPSEND\r\n");
	++esp8266_step_flag;
}

void esp8266_cmd_fill_time_request(){
	if(!IsDataAvailable()) return;
	if(!Wait_for("OK")) return;
	//HAL_Delay(100);
	printf("GET /date HTTP/1.1\r\nHost: marsohk.pythonanywhere.com\r\n\r\n");
	++esp8266_step_flag;
}

void esp8266_cmd_fill_water_request(){
	if(!IsDataAvailable()) return;
	if(!Wait_for("OK")) return;
	printf("GET /water?set=1 HTTP/1.1\r\nHost: marsohk.pythonanywhere.com\r\n\r\n");
	++esp8266_step_flag;
}

void esp8266_cmd_fill_exercise_request(){
	if(!IsDataAvailable()) return;
	if(!Wait_for("OK")) return;
	extern uint32_t exertime_fixed;
	printf("GET /exercise?time=%d HTTP/1.1\r\nHost: marsohk.pythonanywhere.com\r\n\r\n", exertime_fixed);
	++esp8266_step_flag;
}

void esp8266_get_time_buffer(){
	if(!IsDataAvailable()) return;
	if(!Wait_for("OK")) return;
	HAL_Delay(100);
	extern char USART_DATE_BUFFER[15];

	//LCD_Clear(0, 100, 250,150);
	//LCD_DrawString(20, 100, "Done...");
	Wait_for("DATA:");
	Get_after("DATA:",15, USART_DATE_BUFFER);

    ++esp8266_step_flag;//Done
}

void esp8266_water_done(){
	if(!IsDataAvailable()) return;
	if(!Wait_for("Drink")) return;
	extern uint8_t USART_WATER_FLAG;
	USART_WATER_FLAG = 0;
}

void esp8266_exercise_done(){
	if(!IsDataAvailable()) return;
	if(!Wait_for("Exercise")) return;
	extern uint8_t USART_EXERCISE_FLAG;
	USART_EXERCISE_FLAG = 0;
}

void esp8266_get_time(){
	switch (esp8266_step_flag){
		case 1://Reset
			esp8266_cmd_reset();
			break;
		case 2://Set Station
			esp8266_cmd_set_station();
			break;
		case 3://Connect Wifi
			esp8266_cmd_connectWifi();
			LCD_Clear(0, 100, 250,150);
			LCD_DrawString(20, 100, "Connecting To Wifi...");
			break;
		case 4://Set TCP
			esp8266_cmd_tcp();
			break;
		case 5://SET Trans mode
			esp8266_cmd_trans_mode();
			break;
		case 6://Send Request
			esp8266_cmd_send_request();
			LCD_Clear(0, 100, 250,150);
			LCD_DrawString(20, 100, "Sending Request To Server..");
			break;
		case 7://Fill Request
			esp8266_cmd_fill_time_request();
			break;
		case 8://Fill Data to buffer
			esp8266_get_time_buffer();
			break;
		case 0:
			esp8266_step_flag = 1;
			ESP8266_RESET();
			break;
	}
}

void esp8266_update_water(){
	switch (esp8266_step_flag){
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
			esp8266_cmd_fill_water_request();
			break;
		case 8://Done, Reset Flag
			esp8266_water_done();
			break;
		case 0:
			esp8266_step_flag = 1;
			ESP8266_RESET();
			break;
	}
}


void esp8266_update_exercise(){
	switch (esp8266_step_flag){
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
			esp8266_cmd_fill_exercise_request();
			break;
		case 8://Done, Reset Flag
			esp8266_exercise_done();
			break;
		case 0:
			esp8266_step_flag = 1;
			ESP8266_RESET();
			break;
	}
}

