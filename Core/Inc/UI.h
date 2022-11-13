/*
 * UI.h
 *
 *  Created on: Nov 6, 2022
 *      Author: homan
 */

#ifndef INC_UI_H_
#define INC_UI_H_
#include "xpt2046.h"
#include "dht11.h"


uint8_t Check_touchkey ();
void UI_Drink_Water();
void UI_Home();
void Render(uint8_t*, uint8_t*,const unsigned char *);
void UI_Home_Display_Date(uint16_t year, uint8_t month, uint8_t day);
void UI_Home_Display_Time(uint8_t hour, uint8_t minute, uint8_t second);
void UI_Home_Display_Pet(uint16_t StartX, uint16_t StartY,unsigned char *pic);
void UI_Home_Display_DHT11();
void UI_Stats();
void UI_Stats_Update();
void UI_Config();
void UI_Time_set();
void UI_Set();
void UI_Set_Update();
#endif /* INC_UI_H_ */
