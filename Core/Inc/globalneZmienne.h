/*!
 * @file globalneZmienne.h
 * @brief Deklaracje uchwytów dla komponentów systemowych STM32.
 *
 * Plik zawiera deklaracje globalnych uchwytów do konfiguracji i obsługi
 * różnych peryferiów mikrokontrolera STM32, takich jak ADC, DMA, UART i timery.
 */

#ifndef INC_GLOBALNEZMIENNE_H_
#define INC_GLOBALNEZMIENNE_H_

#include "stm32f4xx_hal.h"

/*!
 * @brief Uchwyt do przetwornika analogowo-cyfrowego (ADC1).
 */
extern ADC_HandleTypeDef hadc1;

/*!
 * @brief Uchwyt do kontrolera DMA używanego z ADC1.
 */
extern DMA_HandleTypeDef hdma_adc1;

/*!
 * @brief Uchwyt do timera 2.
 */
extern TIM_HandleTypeDef htim2;

/*!
 * @brief Uchwyt do timera 3.
 */
extern TIM_HandleTypeDef htim3;

/*!
 * @brief Uchwyt do timera 4.
 */
extern TIM_HandleTypeDef htim4;

/*!
 * @brief Uchwyt do interfejsu UART2.
 */
extern UART_HandleTypeDef huart2;

#endif /* INC_GLOBALNEZMIENNE_H_ */
