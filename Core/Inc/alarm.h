/*
 * alarm.h
 *
 *  Created on: 2022年11月14日
 *      Author: homan
 */

#ifndef INC_ALARM_H_
#define INC_ALARM_H_
//Handle All Alarm Related Function
#include "stm32f1xx_hal.h"
#include "buzzer.h"
#include "rtc.h"
#include "dht11.h"
#include "pet.h"
void alarm_update_next();
void alarm_update_last();
void alarm_times_up();
void alarm_release();

void debug_alarm_set();
#endif /* INC_ALARM_H_ */
