/*!
 * @file przetworzDane.c
 * @brief Implementacja funkcji do przetwarzania danych.
 *
 * Plik zawiera implementację funkcji do analizy i przetwarzania danych odbieranych
 * w systemie oraz konfiguracji parametrów na podstawie odebranych danych.
 */

#include "przetworzDane.h"

/*!
 * @brief Inicjalizacja struktury ,,warunkiPomiaru''.
 *
 * Zmienna ,,warunkiPomiaru'' przechowuje wszystkie parametry
 * związane z warunkami pomiarów w systemie.
 *
 * Struktura jest inicjalizowana wartościami domyślnymi, które wskazują
 * brak aktywnego pomiaru, zerowe wartości progów, czasu i parametrów.
 */
WarunkiPomiaru warunkiPomiaru = {
    .idCzujnika = "",
    .idPomiaruDynam = "",
    .idPomiaru = '\0',
    .czasPomiaruDynam = 0,
    .progPomiaruDynam = 0,
    .minimalnaZmiana = 0,
    .iloscPunktowDoSredniej = 0,
	.indeksHistorii = 0,
	.flagaZmiany = 0
};

/*!
 * @brief Inicjalizacja struktury ,,danePomiaroweDynam''.
 *
 * Zmienna ,,danePomiaroweDynam'' przechowuje dynamiczne dane pomiarowe
 * oraz historię pomiarów.
 *
 * Struktura jest inicjalizowana zerowymi wartościami.
 */
DanePomiaroweDynam danePomiaroweDynam = {
		.tablicaPomiarowDynamicznych = {0},
		.tablicaHistoriiPrzedPomiarem = {0}
};

/*!
 * Funkcja analizuje dane odebrane z interfejsu komunikacyjnego, identyfikuje
 * typ żądania (pomiar statyczny, dynamiczny, matrycowy) i odpowiednio
 * konfiguruje system na podstawie otrzymanych danych.
 *
 * @param[in] data - wskaźnik na dane do przetworzenia.
 * @param[in] length - długość danych.
 */
void process_received_data(uint8_t *data, uint16_t length) {
    char command = data[0];  // 'command' jest teraz w pierwszym znaku danych
    warunkiPomiaru.idPomiaru = command;


    if ( !(command == ID_POMIAR_MATRYCY) )
    {

    strncpy(warunkiPomiaru.idCzujnika, (char*)&data[2], 2);  // Kopiowanie dwóch znaków od indeksu 2
    warunkiPomiaru.idCzujnika[2] = '\0';

    } else {

    	warunkiPomiaru.idCzujnika[0] = ID_CZUJNIK_BRAK;
    	warunkiPomiaru.idCzujnika[1] = ID_CZUJNIK_BRAK;
    	warunkiPomiaru.idCzujnika[2] = '\0';
    }
    // Wyszukiwanie wartości po "P "
        char *ptrP = strstr((char *)data, "P ");
        if (ptrP != NULL) {
            ptrP += 2;  // Przesunięcie wskaźnika za "P "
            char *endPtr = strchr(ptrP, ' ');  // Znalezienie końca wartości (pierwszej spacji)

            if (endPtr == NULL) {  // Jeśli brak spacji, zakładamy koniec danych
                endPtr = (char *)data + length;
            }

            // Obliczanie długości wartości
            size_t len = endPtr - ptrP;
            if (len < 6) {  // Sprawdzamy, czy długość jest poprawna
                char progStr[6] = {0};  // Bufor na wartość progu
                strncpy(progStr, ptrP, len);
                progStr[len] = '\0';  // Zakończenie ciągu znaków
                warunkiPomiaru.progPomiaruDynam = (uint16_t)strtoul(progStr, NULL, 10);  // Konwersja na uint16_t
//                printf("Prog pomiaru dynam: %u\r\n", warunkiPomiaru.progPomiaruDynam);
            } else {
                printf("Błąd: Długość wartości progu przekracza limit!\r\n");
            }
        } else {
            printf("Błąd: Nie znaleziono ciągu 'P '.\r\n");
        }

//    printf("IdCzujnika: %s\r\n", warunkiPomiaru.idCzujnika);
//    printf("Command: %c\r\n", command);

    if ( command == ID_POMIAR_STAT )
    {
    	HAL_ADC_Stop_DMA(&hadc1);
        HAL_TIM_Base_Stop_IT(&htim2);
        HAL_TIM_Base_Stop_IT(&htim3);
        HAL_TIM_Base_Start_IT(&htim4);

        warunkiPomiaru.flagaZmiany = 0;

    } else if ( command == ID_POMIAR_MATRYCY ) {

    	HAL_ADC_Stop_DMA(&hadc1);
        HAL_TIM_Base_Start_IT(&htim4);
        HAL_TIM_Base_Stop_IT(&htim3);
        HAL_TIM_Base_Start_IT(&htim2);

        warunkiPomiaru.flagaZmiany = 0;

    } else if (command == ID_POMIAR_DYNAM) {

    	HAL_ADC_Stop(&hadc1);

    	if (strncmp((char*)data + 5, ID_DYNAM_CZASOWY_STR, 1) == 0) {  // Komenda "T" zaczyna się od indeksu 5
            strncpy(warunkiPomiaru.idPomiaruDynam, ID_DYNAM_CZASOWY_STR, 2);
            warunkiPomiaru.idPomiaruDynam[2] = '\0';  // Dodanie końca stringa

            // Parsowanie parametru jako uint16_t
            char param_str[6] = {0};
            strncpy(param_str, (char*)data + 7, 5);  // Parametr zaczyna się od indeksu 7
            warunkiPomiaru.czasPomiaruDynam = (uint16_t)strtoul(param_str, NULL, 10);  // Konwersja na uint16_t
//            printf("Mam komende D T");

    	} else if (strncmp((char*)data + 5, ID_DYNAM_BEZ_LIMITU_STR, 1) == 0) {  // Komenda "X" zaczyna się od indeksu 5
    	    strncpy(warunkiPomiaru.idPomiaruDynam, ID_DYNAM_BEZ_LIMITU_STR, 2);
    	    warunkiPomiaru.idPomiaruDynam[2] = '\0';  // Dodanie końca stringa

    	    // Wyodrębnianie wartości P
    	    char *ptrP = strstr((char *)data, "P ");
    	    if (ptrP) {
    	        ptrP += 2;  // Przesunięcie wskaźnika za "P "
    	        warunkiPomiaru.progPomiaruDynam = (uint16_t)strtoul(ptrP, NULL, 10);  // Konwersja do uint16_t
//    	        printf("Prog pomiaru (P): %u\r\n", warunkiPomiaru.progPomiaruDynam);
    	    } else {
    	        printf("Błąd: Nie znaleziono parametru 'P'.\r\n");
    	    }

    	    // Wyodrębnianie wartości N, ilosc punktow do sredniej dla pomiaru bez limitu
    	    char *ptrN = strstr((char *)data, "N ");
    	    if (ptrN) {
    	        ptrN += 2;  // Przesunięcie wskaźnika za "N "
    	        warunkiPomiaru.iloscPunktowDoSredniej = (uint16_t)strtoul(ptrN, NULL, 10);  // Konwersja do uint16_t
//    	        printf("Liczba pomiarów (N): %u\r\n", warunkiPomiaru.iloscPunktowDoSredniej);
    	    } else {
    	        printf("Błąd: Nie znaleziono parametru 'N'.\r\n");
    	    }

    	    // Wyodrębnianie wartości K, wartosci minimalnej zmiany
    	    char *ptrK = strstr((char *)data, "K ");
    	    if (ptrK) {
    	        ptrK += 2;  // Przesunięcie wskaźnika za "K "
    	        warunkiPomiaru.minimalnaZmiana = (uint16_t)strtoul(ptrK, NULL, 10);  // Konwersja do uint16_t
//    	        printf("Minimalna zmiana (K): %u\r\n", warunkiPomiaru.minimalnaZmiana);
    	    } else {
    	        printf("Błąd: Nie znaleziono parametru 'K'.\r\n");
    	    }
    	}
    	if ( warunkiPomiaru.idCzujnika[1] == ID_CZUJNIK_CP_150 )
    	{
    		warunkiPomiaru.flagaZmiany = 0;

    	} else if( warunkiPomiaru.idCzujnika[1] == ID_CZUJNIK_CP_154 ) {

    		warunkiPomiaru.flagaZmiany = 0;
    	}

        HAL_TIM_Base_Stop_IT(&htim2);
        HAL_TIM_Base_Stop_IT(&htim4);
        HAL_TIM_Base_Start_IT(&htim3);
    }
}

