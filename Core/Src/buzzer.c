/*
 * buzzer.c
 *
 *  Created on: Nov 14, 2022
 *      Author: homan
 */
#include "buzzer.h"

void Buzzer_INIT(){
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,0);
}

void Beep_start(){
	extern TIM_HandleTypeDef htim1;

	//Start Beeping
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
}

void Beep_stop(){
	extern TIM_HandleTypeDef htim1;
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
}

void Beep_set(uint16_t prescaler, uint16_t period, uint16_t pulse){
	extern TIM_HandleTypeDef htim1;
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);

	TIM_OC_InitTypeDef sConfigOC = {0};
	sConfigOC.OCMode = TIM_OCMODE_PWM1;

	//Change Sound (PWM)
	htim1.Init.Prescaler = 63999;// E.g. If Prescaler = 63999,  72M Hz / (63999 + 1) = 1M Hz
	htim1.Init.Period = 1124;// (One Period + 1) Per Second
	sConfigOC.Pulse = 562;// PWM = Pulse/Period
	HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1);
}
