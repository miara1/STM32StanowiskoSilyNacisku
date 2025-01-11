#ifndef CRC_H
#define CRC_H

#include "main.h"

uint16_t crc16_ccitt_false(const char* pData, int length);

uint16_t parse_crc_from_text(const char *crc_text);

#endif // CRC_H
