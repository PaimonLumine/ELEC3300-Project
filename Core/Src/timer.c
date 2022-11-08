#include "timer.h"

TIM_HandleTypeDef* htim2_loc;

void TIM2_INIT(TIM_HandleTypeDef* htim2){
	htim2_loc = htim2;
}


void delay_us(uint16_t nus)
{
	__HAL_TIM_SET_COUNTER(htim2_loc, 0);
	__HAL_TIM_ENABLE(htim2_loc);
	while (__HAL_TIM_GET_COUNTER(htim2_loc) < nus)
	{
	}
	__HAL_TIM_DISABLE(htim2_loc);
}
