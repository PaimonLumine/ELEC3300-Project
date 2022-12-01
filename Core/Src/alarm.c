/*
 * alarm.c
 *
 *  Created on: 2022年11月14日
 *      Author: homan
 */
#include "alarm.h"


/*
 * Call when Initialize or Times up
 */
void alarm_update_next(){
	extern uint32_t next;
	extern DHT11_datastruct DHT11_data;

	uint32_t realtime_raw = RTC_raw();
	double humid = DHT11_data.humid_int;
	double temp = DHT11_data.temp_int;

	double result1 = 5400 * (1 + (humid / 100));

	//next = result1; //humidity
	if (temp > 26) {
		result1 = result1 / (1 + (temp - 26) / 10);
		//next = result1; //temperature
	}
	next = realtime_raw + result1;
}


void alarm_update_last(){
	extern uint32_t lastdrink_raw;
	lastdrink_raw = RTC_raw();
}

void alarm_times_up(){
	extern uint8_t ALARM_TIMES_UP_RENDER_FLAG;
	extern const unsigned char * petStats;
	extern uint8_t darkmode_toggle;
	  ALARM_TIMES_UP_RENDER_FLAG = 1; // Prevent Keep Updating UI
	  if(!darkmode_toggle) petStats = water1;
	  else petStats = water1_night;
}

void alarm_release(){ //When User Click Drink Water
	alarm_update_next();
	alarm_update_last();
	extern uint8_t ALARM_TIMES_UP_RENDER_FLAG;
	ALARM_TIMES_UP_RENDER_FLAG = 0;
	Beep_stop();
}

void debug_alarm_set(){ //Alarm rings after 3 second (Pressing K2)
	extern uint32_t next;

	uint32_t realtime_raw = RTC_raw();

	next = realtime_raw + 3;
}



