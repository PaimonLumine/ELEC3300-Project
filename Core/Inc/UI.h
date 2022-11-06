/*
 * UI.h
 *
 *  Created on: Nov 6, 2022
 *      Author: homan
 */

#ifndef INC_UI_H_
#define INC_UI_H_
#include "xpt2046.h"
#include "lcdtp.h"

uint8_t Check_touchkey ();
void UI_Drink_Water();
void UI_Home();
void Render(uint8_t*, uint8_t*);
void UI_Home_Display_Date(uint16_t year, uint8_t month, uint8_t day);
void UI_Home_Display_Time(uint8_t hour, uint8_t minute, uint8_t second);

#endif /* INC_UI_H_ */
