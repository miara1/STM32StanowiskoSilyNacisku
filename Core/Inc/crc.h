/*!
 * @file crc.h
 * @brief Deklaracje funkcji do obliczania i parsowania CRC
 *
 * Plik zawiera deklaracje funkcji używanych do obliczania wartości CRC16
 * zgodnie ze standardem CCITT FALSE oraz do parsowania CRC z tekstu.
 */

#ifndef CRC_H
#define CRC_H

#include "main.h"

/*!
 * @brief Oblicza wartość CRC16 zgodnie ze standardem CCITT FALSE.
 */
uint16_t crc16_ccitt_false(const char* pData, int length);

/*!
 * @brief Parsuje wartość CRC z tekstu.
 */
uint16_t parse_crc_from_text(const char *crc_text);

#endif // CRC_H
