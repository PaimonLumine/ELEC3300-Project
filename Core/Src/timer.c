#include "timer.h"

uint8_t timer_left = 0;//30 sec for each count
uint8_t start_counting = 0;

void TIMER_INIT(){
	__HAL_TIM_CLEAR_FLAG(&htim5, TIM_FLAG_UPDATE);
	HAL_TIM_Base_Start(&htim5);
	HAL_TIM_Base_Start_IT(&htim5);
}

void delay_us(uint16_t nus)
{
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	__HAL_TIM_ENABLE(&htim2);
	while (__HAL_TIM_GET_COUNTER(&htim2) < nus)
	{
	}
	__HAL_TIM_DISABLE(&htim2);
}

void timer_min(uint8_t min)
{
	__HAL_TIM_CLEAR_FLAG(&htim3, TIM_FLAG_UPDATE);
	timer_left = min*2;
	start_counting = 1;
	HAL_TIM_Base_Start(&htim3);
	HAL_TIM_Base_Start_IT(&htim3);
}
