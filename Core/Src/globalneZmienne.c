/*!
 * @file globalneZmienne.c
 * @brief Definicje uchwytów dla komponentów systemowych STM32.
 *
 * Plik zawiera definicje globalnych uchwytów dla peryferiów mikrokontrolera STM32,
 * takich jak ADC, DMA, UART i timery. Są one wykorzystywane do konfiguracji i
 * obsługi sprzętowych modułów systemu.
 */

#include "globalneZmienne.h"

/*!
 * @brief Definicja uchwytu dla przetwornika ADC1.
 */
ADC_HandleTypeDef hadc1;

/*!
 * @brief Definicja uchwytu dla kontrolera DMA używanego z ADC1.
 */
DMA_HandleTypeDef hdma_adc1;

/*!
 * @brief Definicja uchwytu dla timera 2.
 */
TIM_HandleTypeDef htim2;

/*!
 * @brief Definicja uchwytu dla timera 3.
 */
TIM_HandleTypeDef htim3;

/*!
 * @brief Definicja uchwytu dla timera 4.
 */
TIM_HandleTypeDef htim4;

/*!
 * @brief Definicja uchwytu dla interfejsu UART2.
 */
UART_HandleTypeDef huart2;
