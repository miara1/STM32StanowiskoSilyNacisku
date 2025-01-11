/**
 * @file adc.h
 * @brief Deklaracje funkcji i struktur związanych z obsługą ADC
 *
 * Plik zawiera deklaracje funkcji i struktur wykorzystywanych
 * do konfiguracji i obsługi ADC.
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

#include "main.h"
#include "przetworzDane.h"
#include "komunikacjaUART.h"

/**
 * @brief Struktura przechowująca wartości ADC
 */
typedef struct
{
	/*! Wartość odczytana dla czujnika 1. */
	volatile int wartosc1;
	/*! Wartość odczytana dla czujnika 2. */
	volatile int wartosc2;
	/*! Wartość odczytana dla czujnika 3. */
	volatile int wartosc3;
	/*! Wartość odczytana dla czujnika 4. */
	volatile int wartosc4;
	/*! Wartość odczytana dla czujnika 5. */
	volatile int wartosc5;
	/*! Wartość odczytana dla czujnika 6. */
	volatile int wartosc6;

}WartosciADC;

/**
 * @brief Struktura przechowująca wartości odczytane z kanałów ADC.
 *
 * Zmienna globalna używana do przechowywania najnowszych wyników konwersji
 * dla każdego zdefiniowanego kanału ADC.
 */
extern WartosciADC wartosciADC;

/**
 * @brief Funkcja wybierająca kanał ADC do pomiarów z czujnika 1
 */
void ADC_Select_CH0 (void);

/**
 * @brief Funkcja wybierająca kanał ADC do pomiarów z czujnika 2
 */
void ADC_Select_CH1 (void);

/**
 * @brief Funkcja wybierająca kanał ADC do pomiarów z czujnika 3
 */
void ADC_Select_CH4 (void);

/**
 * @brief Funkcja wybierająca kanał ADC do pomiarów z czujnika 4
 */
void ADC_Select_CH8 (void);

/**
 * @brief Funkcja wybierająca kanał ADC do pomiarów z czujnika 6
 */
void ADC_Select_CH10 (void);

/**
 * @brief Funkcja wybierająca kanał ADC do pomiarów z czujnika 5
 */
void ADC_Select_CH11 (void);

/**
 * @brief Callback wywoływany po zakończeniu konwersji ADC
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);

/**
 * @brief Callback obsługujący błąd ADC
 */
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc);

#endif /* INC_ADC_H_ */
