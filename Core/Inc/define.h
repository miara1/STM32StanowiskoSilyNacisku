/*!
 * @file define.h
 * @brief Definicje stałych używanych w projekcie.
 *
 * Plik zawiera makra definiujące stałe, takie jak maksymalne rozmiary buforów i
 * identyfikatory rodzajów pomiarów.
 */

#ifndef INC_DEFINE_H_
#define INC_DEFINE_H_

/*!
 * @def MAX_POMIARY
 * @brief Maksymalna liczba pomiarów do składowania w tablicy.
 */
#define MAX_POMIARY 5000

/*!
 * @def FRAME_END
 * @brief Sekwencja znaków oznaczająca zakończenie ramki danych.
 */
#define FRAME_END "\r\n"  // Zakończenie ramki

/*!
 * @def MAX_FRAME_SIZE
 * @brief Maksymalny rozmiar ramki danych dla bufora wejścia UART.
 */
#define MAX_FRAME_SIZE 50  // Maksymalny rozmiar ramki danych

/*!
 * @def ROZM_HISTORIA
 * @brief Liczba zapisów w buforze historii pomiarów dla pomiarów dynamicznych.
 */
#define ROZM_HISTORIA 50

/*!
 * @def ID_POMIAR_MATRYCY
 * @brief Identyfikator pomiaru matrycy.
 */
#define ID_POMIAR_MATRYCY 'M'

/*!
 * @def ID_POMIAR_DYNAM
 * @brief Identyfikator pomiaru dynamicznego.
 */
#define ID_POMIAR_DYNAM 'D'

/*!
 * @def ID_POMIAR_STAT
 * @brief Identyfikator pomiaru statycznego.
 */
#define ID_POMIAR_STAT 'S'

/*!
 * @def ID_DYNAM_CZASOWY
 * @brief Identyfikator trybu pomiaru dynamicznego ograniczonego czasowo.
 */
#define ID_DYNAM_CZASOWY 'T'

/*!
 * @def ID_DYNAM_CZASOWY_STR
 * @brief Tekstowy identyfikator trybu pomiaru dynamicznego ograniczonego czasowo.
 */
#define ID_DYNAM_CZASOWY_STR "T"

/*!
 * @def ID_DYNAM_BEZ_LIMITU
 * @brief Identyfikator trybu pomiaru dynamicznego bez ograniczeń.
 */
#define ID_DYNAM_BEZ_LIMITU 'X'

/*!
 * @def ID_DYNAM_BEZ_LIMITU_STR
 * @brief Tekstowy identyfikator trybu pomiaru dynamicznego bez ograniczeń.
 */
#define ID_DYNAM_BEZ_LIMITU_STR "X"

/*!
 * @def NAGLOWEK_RAMKI_APLIKACJI
 * @brief Nagłówek ramki aplikacji (rozpoznawanie ramki).
 */
#define NAGLOWEK_RAMKI_APLIKACJI 'U'

/*!
 * @def ID_CZUJNIK_CP_150
 * @brief Identyfikator czujnika CP-150.
 */
#define ID_CZUJNIK_CP_150 '5'

/*!
 * @def ID_CZUJNIK_CP_154
 * @brief Identyfikator czujnika CP-154.
 */
#define ID_CZUJNIK_CP_154 '6'

/*!
 * @def ID_CZUJNIK_BRAK
 * @brief Identyfikator braku sprecyzowanego czujnika w ramce z aplikacji.
 */
#define ID_CZUJNIK_BRAK '0'

#endif /* INC_DEFINE_H_ */
