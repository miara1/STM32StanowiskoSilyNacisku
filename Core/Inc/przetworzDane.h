/*!
 * @file przetworzDane.h
 * @brief Deklaracje struktur i funkcji do przetwarzania danych.
 *
 * Plik zawiera deklaracje struktur danych oraz funkcji
 * do przetwarzania danych odbieranych i wysyłanych w systemie.
 */

#ifndef INC_PRZETWORZDANE_H_
#define INC_PRZETWORZDANE_H_

#include "main.h"

/*!
 * @brief Struktura przechowująca dane pomiarowe dla pomiarów dynamicznych.
 */
typedef struct
{
	/*! Bufor danych pomiaru dynamicznego. */
	uint32_t tablicaPomiarowDynamicznych[MAX_POMIARY];
	/*! Historia pomiarów przed rozpoczęciem pomiaru dynamicznego. */
	uint32_t tablicaHistoriiPrzedPomiarem[ROZM_HISTORIA];
}DanePomiaroweDynam;

/*!
 * @brief Struktura przechowująca warunki pomiaru.
 */
typedef struct
{
	 /*! Identyfikator czujnika. */
	char idCzujnika[3];
	/*! Identyfikator trybu pomiaru dynamicznego. */
	char idPomiaruDynam[2];
	/*! Identyfikator aktualnego trybu pomiaru. */
	char idPomiaru;
	/*! Czas trwania pomiaru dynamicznego z ustalonym czasem. */
	uint16_t czasPomiaruDynam;
	/*! Próg pomiaru dynamicznego. */
	uint16_t progPomiaruDynam;
	/*! Minimalna zmiana wartości pomiaru dla pomiaru bez limitu czasowego. */
	uint16_t minimalnaZmiana;
	/*! Liczba punktów do średniej w celu zadania warunku zakończenia dla pomiaru dynamicznego bez limitu. */
	uint16_t iloscPunktowDoSredniej;
	/*! Indeks w historii pomiarów. */
	uint16_t indeksHistorii;
	/*! Flaga zmiany stanu pomiaru dynamicznego. */
	int flagaZmiany;

}WarunkiPomiaru;

/*!
 * @brief Globalna zmienna przechowująca dane pomiarowe dynamiczne.
 */
extern DanePomiaroweDynam danePomiaroweDynam;

/*!
 * @brief Globalna zmienna przechowująca warunki pomiaru.
 */
extern WarunkiPomiaru warunkiPomiaru;

/*!
 * @brief Przetwarza odebrane dane.
 */
void process_received_data(uint8_t *data, uint16_t length);


#endif /* INC_PRZETWORZDANE_H_ */
