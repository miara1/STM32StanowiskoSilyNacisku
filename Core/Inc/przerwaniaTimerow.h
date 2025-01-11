/*!
 * @file przerwaniaTimerow.h
 * @brief Deklaracje funkcji obsługujących przerwania timerów
 *
 * Plik zawiera deklaracje funkcji obsługujących przerwania timerów,
 * odpowiedzialnych za realizację pomiarów w różnych trybach.
 */

#ifndef INC_PRZERWANIATIMEROW_H_
#define INC_PRZERWANIATIMEROW_H_

#include "adc.h"
#include "main.h"
#include "crc.h"

/*!
 * @brief Callback wywoływany w przypadku wystąpienia przerwania timera
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

/*!
 * @brief Obsługuje przerwanie związane z pomiarami matrycy.
 */
void obsluzPrzerwanieMatrycy();

/*!
 * @brief Obsługuje przerwanie związane z pomiarami dynamicznymi.
 */
void obsluzPrzerwanieDynam();

/*!
 * @brief Obsługuje przerwanie związane z pomiarami statycznymi.
 */
void obsluzPrzerwanieStat();

#endif /* INC_PRZERWANIATIMEROW_H_ */
