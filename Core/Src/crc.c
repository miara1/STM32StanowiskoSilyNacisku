#include "crc.h"

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

uint16_t parse_crc_from_text(const char *crc_text) {
    uint16_t crc_value = 0;
    sscanf(crc_text, "%4hx", &crc_value);  // Parsowanie jako 16-bitowy hex
    return crc_value;
}
