#include "UI.h"
#include "lcdtp.h"

uint8_t Check_touchkey(const int* constraints ,strType_XPT2046_Coordinate *pDisplayCoordinate) {
	uint8_t match = (constraints[0]<= pDisplayCoordinate->x && constraints[1]>= pDisplayCoordinate->x
			 && constraints[2]<= pDisplayCoordinate->y && constraints[3]>= pDisplayCoordinate->y);
	 return match;
}

void Render(uint8_t* mode_new, uint8_t* render_status){
	//Not Render if done
	if(*render_status==1) return;

	//Mode 0 = Home, Mode 1 = Drink Water, Mode 2 = Toggle Dark Mode, Mode 3 = Pet
	switch(*mode_new){
		case(0):
			UI_Home();
			break;
		case(1):
			UI_Drink_Water();
			*mode_new = 0;
			break;
		case(2):
			LCD_Darkmode_Toggle();
			*mode_new = 0;
			break;

	}
	*render_status = 1;
}

void UI_Drink_Water(){
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

}

void UI_Home(){
	LCD_Clear ( 0, 0, 240, 320);
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

void UI_Home_Display_Date(uint16_t year, uint8_t month, uint8_t day){
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

void UI_Home_Display_Time(uint8_t hour, uint8_t minute, uint8_t second){
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

void UI_Home_Display_Pet(uint16_t StartX, uint16_t StartY,unsigned char *pic){

	LCD_DrawPicture(StartX,StartY,pic);
}


