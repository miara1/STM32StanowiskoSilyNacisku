#ifndef INC_KOMUNIKACJAUART_H_
#define INC_KOMUNIKACJAUART_H_

#include "main.h"
#include "crc.h"
#include "przetworzDane.h"

typedef struct
{
	uint8_t receivedData; // odebrane dane
	uint8_t buffer[MAX_FRAME_SIZE]; // Bufor do odbioru ramki danych

}OdbiorUART;

extern OdbiorUART odbiorUART;

void receive_data_frame(uint16_t length);
void sendDataRamkaDynamT(uint32_t *tablicaPomiarowDynamicznych, int czasPomiaruDynam);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#endif /* INC_KOMUNIKACJAUART_H_ */
