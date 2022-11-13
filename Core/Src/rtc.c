
#include "rtc.h"

uint16_t ryear;
uint8_t rmon,rday,rhour,rmin,rsec,rweek;
uint8_t const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5};
uint8_t const mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};

void RTC_Init(RTC_HandleTypeDef* hrtc)
{
	hrtc->Instance = RTC;
	hrtc->Init.AsynchPrediv = RTC_AUTO_1_SECOND;
	hrtc->Init.OutPut = RTC_OUTPUTSOURCE_NONE;
	if (HAL_RTC_Init(hrtc) != HAL_OK)
	{
		Error_Handler();
	}
	//Check If first time connect to power source
	if(HAL_RTCEx_BKUPRead(hrtc,RTC_BKP_DR1)!=0x5050){//Deafult Value should be 0xffff
		HAL_RTCEx_BKUPWrite(hrtc,RTC_BKP_DR1,0x5050);
		RTC_Set(2022,1,1,0,0,0);
	}
}

uint8_t RTC_Set(uint16_t syear, int8_t smon, uint8_t sday,uint8_t rhour,uint8_t rmin,uint8_t rsec){
	/*
	 * Set Time According To Parameters (Be Done Later Using USART)
	 */
	uint16_t t;
	uint32_t seccount=0;
	if(syear<2000||syear>2099)return 1;// Range: 1970-2099
	for(t=1970;t<syear;++t){
		if(Is_Leap_Year(t))seccount+=31622400;
		else seccount+=31536000;
	}
	smon-=1;
	for(t=0;t<smon;t++){
		seccount+=(uint32_t)mon_table[t]*86400;
		if(Is_Leap_Year(syear)&&t==1)seccount+=86400;
	}
	seccount+=(uint32_t)(sday-1)*86400;
	seccount += (uint32_t)rhour*3600;
	seccount += (uint32_t)rmin*60;
	seccount += rsec;

	RTC->CRL|=1<<4;
	RTC->CNTL=seccount&0xffff;
	RTC->CNTH=seccount>>16;
	RTC->CRL&=~(1<<4);
	while(!(RTC->CRL&(1<<5)));

	return 0;
}

uint8_t RTC_Get(void){
	/*
	 * Refresh Time variables in rtc.c
	 */
	static uint16_t daycnt=0;
	uint32_t timecount=0;
	uint32_t temp=0;
	uint32_t temp1=0;

	timecount=RTC->CNTH;
	timecount<<=16;
	timecount+=RTC->CNTL;

	temp=timecount/86400;//Day
	if(daycnt!=temp){//New Day
		daycnt=temp;
		temp1=1970;
		while(temp>=365){
			if(Is_Leap_Year(temp1)){
				if(temp>=366)temp-=366;
				else{temp1++; break;}
			}
			else temp-=365;
			temp1++;
		}
		ryear=temp1;//Year
		temp1=0;
		while(temp>=28){
			if(Is_Leap_Year(ryear)&&temp1==1){
				if(temp>=29)temp-=29;
				else break;
			}else{
			if(temp>=mon_table[temp1])temp-=mon_table[temp1];
			else break;
			}
			temp1++;
		}
		rmon=temp1+1;
		rday=temp+1;
	}
	temp=timecount%86400;
	rhour=temp/3600;
	rmin=(temp%3600)/60;
	rsec=(temp%3600)%60;
	rweek=RTC_Get_Week(ryear,rmon,rday);
	return 0;
}

uint8_t RTC_Get_Week(uint16_t year, uint16_t month, uint16_t day){
	uint16_t temp2;
	uint8_t yearH,yearL;
	yearH=year/100;
	yearL=year%100;
	if (yearH>19)yearL+=100;
	temp2=yearL+yearL/4;
	temp2=temp2%7;
	temp2=temp2+day+table_week[month-1];
	if (yearL%4==0&&month<3)temp2--;
	return(temp2%7);
}

uint8_t Is_Leap_Year(uint16_t year){
	if(year%4==0){
		if(year%100==0){
			if(year%400==0)return 1;
			else return 0;
		}else return 1;
	}else return 0;
}

uint32_t RTC_raw(){
	uint32_t time;
	time=RTC->CNTH;
	time<<=16;
	time+=RTC->CNTL;

	return time;
}


void get_TimeStamp(TimeStamp* t){
	/*
	 * Store Updated Values By a TimeStamp Pointer
	 */
	t->ryear = ryear;
	t->rmon = rmon;
	t->rday = rday;
	t->rhour = rhour;
	t->rmin = rmin;
	t->rsec = rsec;
	t->rweek = rweek;
}
