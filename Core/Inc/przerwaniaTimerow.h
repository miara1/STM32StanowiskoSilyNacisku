#ifndef INC_PRZERWANIATIMEROW_H_
#define INC_PRZERWANIATIMEROW_H_

#include "adc.h"
#include "main.h"
#include "crc.h"

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

void obsluzPrzerwanieMatrycy();
void obsluzPrzerwanieDynam();
void obsluzPrzerwanieStat();

#endif /* INC_PRZERWANIATIMEROW_H_ */
