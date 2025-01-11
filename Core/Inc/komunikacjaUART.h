/*!
 * @file komunikacjaUART.h
 * @brief Deklaracje funkcji i struktur do obsługi komunikacji UART
 *
 * Plik zawiera definicje i deklaracje funkcji, zmiennych
 * i struktur potrzebnych do obsługi komunikacji UART.
 */

#ifndef INC_KOMUNIKACJAUART_H_
#define INC_KOMUNIKACJAUART_H_

#include "main.h"
#include "crc.h"
#include "przetworzDane.h"

/*!
 * @brief Struktura przechowująca dane dla odbioru UART
 *
 * Struktura ,,OdbiorUART'' zawiera bufor odbiorczy oraz zmienną na
 * przechowywanie ostatniego odebranego bajtu.
 */
typedef struct
{
	/*! Ostatni odebrany bajt danych UART. */
	uint8_t receivedData;
	/*! Bufor odbiorczy do przechowywania ramki danych. */
	uint8_t buffer[MAX_FRAME_SIZE];

}OdbiorUART;

/*!
 * @brief Zmienna globalna przechowująca dane odbioru UART
 */
extern OdbiorUART odbiorUART;

/*!
 * @brief Funkcja przetwarzająca odebraną ramkę danych
 */
void receive_data_frame(uint16_t length);

/*!
 * @brief Funkcja wysyłająca ramkę danych dynamicznych dla pomiaru z ustalonym czasem
 */
void sendDataRamkaDynamT(uint32_t *tablicaPomiarowDynamicznych, int czasPomiaruDynam);

/*!
 * @brief Callback wywoływany po zakończeniu odbioru danych UART
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#endif /* INC_KOMUNIKACJAUART_H_ */
