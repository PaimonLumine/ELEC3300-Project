

#ifndef INC_RTC_H_
#define INC_RTC_H_
#include "stm32f1xx_hal.h"

void RTC_Init(RTC_HandleTypeDef* hrtc);
uint8_t Is_Leap_Year(uint16_t year);
uint8_t RTC_Get(void);
uint8_t RTC_Set(uint16_t syear, int8_t smon, uint8_t sday,uint8_t rhour,uint8_t rmin,uint8_t rsec);
uint8_t RTC_Get_Week(uint16_t year, uint16_t month, uint16_t day);
uint32_t RTC_raw();

typedef	struct
{
	uint16_t ryear;
	uint8_t rmon,rday,rhour,rmin,rsec,rweek;

} TimeStamp;

void get_TimeStamp(TimeStamp* t);

#endif /* INC_RTC_H_ */
