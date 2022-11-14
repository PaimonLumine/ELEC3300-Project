/*
 * buzzer.h
 *
 *  Created on: Nov 14, 2022
 *      Author: homan
 */

#ifndef INC_BUZZER_H_
#define INC_BUZZER_H_

#include "stm32f1xx_hal.h"
void Beep_start();
void Beep_stop();
void Beep_set(uint16_t prescaler, uint16_t period, uint16_t pulse);
#endif /* INC_BUZZER_H_ */
