/*!
 * @file crc.c
 * @brief Implementacja funkcji do obliczania i parsowania CRC
 *
 * Plik zawiera implementacje funkcji do obliczania wartości CRC16 zgodnie
 * ze standardem CCITT FALSE oraz parsowania CRC z tekstu.
 */

#include "crc.h"

/*!
 * Funkcja iteracyjnie oblicza wartość CRC dla podanych danych wejściowych.
 *
 * @param[in] pData - wskaźnik na dane wejściowe
 * @param[in] length - długość danych wejściowych
 * @return Wartość CRC16 obliczona zgodnie z CCITT FALSE
 */
uint16_t crc16_ccitt_false(const char* pData, int length)
{
    uint16_t wCrc = 0xffff;
    while (length--) {
        wCrc ^= *(unsigned const char *)pData++ << 8;
        for (int i=0; i < 8; i++)
            wCrc = wCrc & 0x8000 ? (wCrc << 1) ^ 0x1021 : wCrc << 1;
    }
    return wCrc;
}

/*!
 * Funkcja zamienia ciąg znaków zawierający wartość CRC w formacie
 * heksadecymalnym na odpowiadającą mu liczbę całkowitą 16-bitową.
 *
 * @param[in] crc_text - wskaźnik na ciąg znaków z wartością CRC w formacie heksadecymalnym
 * @return Wartość CRC jako 16-bitowa liczba całkowita
 */
uint16_t parse_crc_from_text(const char *crc_text) {
    uint16_t crc_value = 0;
    sscanf(crc_text, "%4hx", &crc_value);  // Parsowanie jako 16-bitowy hex
    return crc_value;
}
