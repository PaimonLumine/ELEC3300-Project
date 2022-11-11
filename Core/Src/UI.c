#include "UI.h"
#include "lcdtp.h"
#include "rtc.h"

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
		*mode_new = 3;
	}
	*render_status = 1;
}

void UI_Drink_Water() {
	LCD_Clear(0, 0, 240, 320);

	LCD_DrawString(10, 150, "Drinked a glass of water");
	LCD_DrawString(10, 250, "Return to home in 2s");
	HAL_Delay(1000);
	LCD_Clear(10, 250, 240, 320);
	LCD_DrawString(10, 250, "Return to home in 1s");
	HAL_Delay(1000);
	LCD_Clear(10, 250, 240, 320);
	LCD_DrawString(10, 250, "Return to home in 0s");
	HAL_Delay(1000);
	extern uint32_t lastdrink_raw;
	lastdrink_raw = RTC_raw();
}

void UI_Home() {

	extern const unsigned char * petStats;
	UI_Home_Display_Button();
	UI_Home_Display_Pet(60,70,petStats);
	UI_Home_Display_DHT11();
}

void UI_Home_Display_Button(){
	LCD_Clear(0, 0, 240, 320);
	LCD_DrawString(2, 10, "Config");
	LCD_DrawString(200, 10, "Stats");
	LCD_DrawString(40, 220, "Set");
	LCD_DrawString(40, 240, "Exercise");
	LCD_DrawString(40, 260, "Timer");
	LCD_DrawString(140, 220, "Drink");
	LCD_DrawString(140, 240, "water");
	LCD_DrawString(200, 260, "Dark");
	LCD_DrawString(200, 280, "mode");
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

	//Draw Year
	sprintf(str, "%02i", hour);
	LCD_DrawString(95, 30, str);

	//Draw Month
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
	LCD_DrawString(70, 200, "Back to home");
}
void UI_Stats_Update() {
	extern uint32_t lastupdate_raw, lastdrink_raw;
	uint32_t realtime_raw = RTC_raw();

	if(realtime_raw == lastupdate_raw) return;
	uint32_t time_diff = realtime_raw - lastdrink_raw;

	//Update New Last Drink
	lastupdate_raw = realtime_raw;

	char timestr[15];
	sprintf(timestr, "%02d : %02d : %02d", time_diff / 3600,
			(time_diff % 3600) / 60, time_diff % 60);
	LCD_DrawString(75, 85, timestr);
}
