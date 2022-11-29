#include "UI.h"
#include "lcdtp.h"
#include "rtc.h"
#include "pet.h"
#include "alarm.h"
#include "esp8266.h"
uint8_t Check_touchkey(const int *constraints,
		strType_XPT2046_Coordinate *pDisplayCoordinate) {
	uint8_t match = (constraints[0] <= pDisplayCoordinate->x
			&& constraints[1] >= pDisplayCoordinate->x
			&& constraints[2] <= pDisplayCoordinate->y
			&& constraints[3] >= pDisplayCoordinate->y);
	return match;
}

void Render(uint8_t *mode_new, uint8_t *render_status,
		const unsigned char *petStats) {
	//Not Render if done
	if (*render_status == 1)
		return;

	//Mode 0 = Home, Mode 1 = Drink Water, Mode 2 = Toggle Dark Mode, Mode 3 = Stats,
	switch (*mode_new) {
	case (0):
		UI_Home();
		break;
	case (1):
		UI_Drink_Water();
		*mode_new = 0;
		break;
	case (2):
		LCD_Darkmode_Toggle();
		*mode_new = 0;
		break;
	case (3):
		UI_Stats();
		break;
	case (4):
		UI_Config();
		break;
	case (5):
		UI_Time_set();
		break;
	case (6):
		UI_Set();
	}
	*render_status = 1;
}

void UI_Drink_Water() {
	LCD_Clear(0, 0, 240, 320);
	extern uint8_t darkmode_toggle;

	LCD_DrawString(10, 220, "Drinked a glass of water");
	LCD_DrawString(10, 250, "Return to home in 2s");
	if(!darkmode_toggle) UI_Home_Display_Pet(60,70,water1);
	else UI_Home_Display_Pet(60,70,water1_night);

	UI_WATER_WIFI_HANDLE(1000);

	LCD_Clear(10, 250, 240, 320);
	LCD_DrawString(10, 250, "Return to home in 1s");
	if(!darkmode_toggle) UI_Home_Display_Pet(60,70,water2);
	else UI_Home_Display_Pet(60,70,water2_night);
	UI_WATER_WIFI_HANDLE(1000);

	LCD_Clear(10, 250, 240, 320);
	LCD_DrawString(10, 250, "Return to home in 0s");
	if(!darkmode_toggle) UI_Home_Display_Pet(60,70,water3);
	else UI_Home_Display_Pet(60,70,water3_night);
	UI_WATER_WIFI_HANDLE(1000);

}

void UI_WATER_WIFI_HANDLE(uint8_t time_delay){
	int i;
	extern uint8_t USART_WATER_FLAG;
	extern uint8_t USART_GET_TIME_FLAG;
	extern uint8_t esp8266_step_flag;
	extern uint8_t USART_EXERCISE_FLAG;
	for(i=0; i< time_delay; ++i){//Don't Waste Time
		  //Upload Exercise Data
		  if(USART_EXERCISE_FLAG && !USART_GET_TIME_FLAG){
			  if(USART_EXERCISE_FLAG==1) {esp8266_step_flag = 0; USART_EXERCISE_FLAG=2;} //Reset Step Flag
			  esp8266_update_exercise();
		  }

		  //Upload drink water data
		  if(USART_WATER_FLAG && !USART_GET_TIME_FLAG && !USART_EXERCISE_FLAG){
			  if(USART_WATER_FLAG==1) {esp8266_step_flag = 0;USART_WATER_FLAG=2;} //Reset Step Flag
			  esp8266_update_water();
		  }
	  HAL_Delay(1);
	}
}

void UI_Home() {

	extern const unsigned char *petStats;
	UI_Home_Display_Button();
	UI_Home_Display_Pet(60, 70, petStats);
	UI_Home_Display_DHT11();
}

void UI_Home_Display_Button() {
	LCD_Clear(0, 0, 240, 320);
	LCD_DrawString(2, 10, "Config");
	LCD_DrawString(200, 10, "Stats");
	LCD_DrawString(40, 220, "Set");
	LCD_DrawString(40, 240, "Exercise");
	LCD_DrawString(40, 260, "Timer");
	LCD_DrawString(140, 220, "Drink");
	LCD_DrawString(140, 240, "water");
	extern uint8_t ADC_DARKMODE_TOGGLE;
	if(!ADC_DARKMODE_TOGGLE){
		LCD_DrawString(200, 260, "Dark");
		LCD_DrawString(200, 280, "mode");
	}
}

void UI_Home_Display_Date(uint16_t year, uint8_t month, uint8_t day) {
	char str[10];

	//Draw Year
	sprintf(str, "%04i", year);
	LCD_DrawString(86, 10, str);

	//Draw Month
	sprintf(str, "%02i", month);
	LCD_DrawString(125, 10, str);

	//Draw Day
	sprintf(str, "%02i", day);
	LCD_DrawString(145, 10, str);
}

void UI_Home_Display_Time(uint8_t hour, uint8_t minute, uint8_t second) {
	char str[10];

	//Draw Hour
	sprintf(str, "%02i", hour);
	LCD_DrawString(95, 30, str);

	//Draw Minute
	sprintf(str, "%02i", minute);
	LCD_DrawString(115, 30, str);

	//Draw Day
	sprintf(str, "%02i", second);
	LCD_DrawString(135, 30, str);
}

void UI_Home_Display_Pet(uint16_t StartX, uint16_t StartY, unsigned char *pic) {

	LCD_DrawPicture(StartX, StartY, pic);
}

void UI_Home_Display_DHT11() {
	extern DHT11_datastruct DHT11_data;
	char Stemp[10];
	char Shum[10];
	sprintf(Stemp, "%02i", DHT11_data.temp_int);
	LCD_DrawString(10, 50, "T");
	LCD_DrawString(0, 70, Stemp);
	if (LCD_GetPointPixel(240, 320) == 0x000000) {
		LCD_DrawCircle(17, 72, 2, WHITE);
	} else {
		LCD_DrawCircle(17, 72, 2, BLACK);
	}
	LCD_DrawString(20, 70, "C");
	sprintf(Shum, "%03i", DHT11_data.humid_int);
	LCD_DrawString(220, 50, "H");
	LCD_DrawString(205, 70, Shum);
	LCD_DrawString(230, 70, "%");

}

void UI_Stats() {
	LCD_Clear(0, 0, 240, 320);
	LCD_DrawString(40, 50, "Time since last drink");

	UI_Stats_Update();
	LCD_DrawString(80, 125, "Next drink");
	//TODO: Time Of Next Water Drinking Event
	LCD_DrawString(70, 190, "Exercise Timer");
	LCD_DrawString(70, 280, "Back to home");
}
void UI_Stats_Update() {
	extern uint32_t lastupdate_raw, lastdrink_raw;
	uint32_t realtime_raw = RTC_raw();

	if (realtime_raw == lastupdate_raw)
		return;
	uint32_t time_diff = realtime_raw - lastdrink_raw;

	//Update New Last Drink
	lastupdate_raw = realtime_raw;

	char timestr[15];
	sprintf(timestr, "%02d : %02d : %02d", time_diff / 3600,
			(time_diff % 3600) / 60, time_diff % 60);
	LCD_DrawString(75, 85, timestr);

	//Update next drink
	extern uint32_t next; //default value
	extern DHT11_datastruct DHT11_data;
	extern int tilnext; // time till next drink
	extern uint32_t exertimer;
	extern uint8_t EXER_TIMER_SET_FLAG;

	tilnext = next - realtime_raw;
	if (tilnext < 0) {
		tilnext = 0;
	}

	sprintf(timestr, "%02d : %02d : %02d", tilnext / 3600,
			(tilnext % 3600) / 60, tilnext % 60);
	LCD_DrawString(75, 155, timestr);

	//tilexer: when will the clock ring next time
	int tilexer = exertimer - realtime_raw;
	if (tilexer < 0) {
			tilexer = 0;
		}
	if(EXER_TIMER_SET_FLAG){
		sprintf(timestr, "%02d : %02d : %02d", tilexer / 3600,
					(tilexer % 3600) / 60, tilexer % 60);
		LCD_DrawString(75, 205, timestr);
	}else{//Timer not set
		LCD_DrawString(75, 205,  "-- : -- : --");
	}

}

void UI_Set() {
	LCD_Clear(0, 0, 240, 320);
	extern exertime;
	LCD_DrawString(80, 50, "Hour");
	LCD_DrawString(140, 50, "Minute");
	LCD_DrawString(80, 90, " +       + ");
	char time[16];
	sprintf(time, "%02d : %02d", exertime / 3600, (exertime % 3600) / 60);
	LCD_DrawString(90, 130, time);
	LCD_DrawString(80, 170, " -       -");
	LCD_DrawString(100, 240, "Set");
	LCD_DrawString(70, 280, "Back to home");
}
void UI_Set_Update() {
	extern exertime;
	char time[16];
	sprintf(time, "%02d : %02d", exertime / 3600, (exertime % 3600) / 60);
	LCD_DrawString(90, 130, time);
}

void UI_Config(){
	LCD_Clear(0, 0, 240, 320);
	LCD_DrawString(40, 50, "- Sync Time (Wifi)");
	extern uint8_t ADC_DARKMODE_TOGGLE;
	if (ADC_DARKMODE_TOGGLE) LCD_DrawString(40, 100, "- (On) Auto Dark Mode");
	else LCD_DrawString(40, 100, "- (Off) Auto Dark Mode");
	LCD_DrawString(10, 280, "Back");
}

void UI_Time_set(){
	LCD_Clear(0, 0, 240, 320);
	LCD_DrawString(90, 50, "Sync Time (Wifi)");
	LCD_DrawString(10, 280, "Back");
	//Handle In esp8266.c
	LCD_Clear(0, 100, 250,150);
	LCD_DrawString(20, 100, "Resetting...");
}
