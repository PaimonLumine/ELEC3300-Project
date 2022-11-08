#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#include "stm32f1xx_hal.h"

void TIM2_INIT(TIM_HandleTypeDef* htim2);
void delay_us(uint16_t nus);

#endif /* INC_TIMER_H_ */
