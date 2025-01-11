#ifndef INC_GLOBALNEZMIENNE_H_
#define INC_GLOBALNEZMIENNE_H_

#include "stm32f4xx_hal.h"

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

extern UART_HandleTypeDef huart2;


#endif /* INC_GLOBALNEZMIENNE_H_ */
