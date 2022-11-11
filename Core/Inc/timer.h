#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#include "stm32f1xx_hal.h"

extern TIM_HandleTypeDef htim2, htim3, htim5;

void TIMER_INIT();
void delay_us(uint16_t nus);
void timer_min(uint8_t min);


#endif /* INC_TIMER_H_ */
