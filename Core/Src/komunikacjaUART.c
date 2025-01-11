#include "komunikacjaUART.h"

OdbiorUART odbiorUART = {
		.buffer = {0},
		.receivedData = 0
};

// Funkcja odbierająca ramkę danych
void receive_data_frame(uint16_t length) {
    // Sprawdzamy, czy ramka jest wystarczająco długa oraz czy zaczyna się od 'U'
    if (length < 9 || odbiorUART.buffer[0] != 'U') {  // Zakładamy minimalną długość: U + dane + 4 znaki CRC + \r\n
        printf("Błąd: Niepoprawna ramka lub brak nagłówka 'U'\n");
        return;
    }
    //Zatrzymanie przerwan wsyzstkich timerow
    HAL_TIM_Base_Stop_IT(&htim2);
    HAL_TIM_Base_Stop_IT(&htim3);
    HAL_TIM_Base_Stop_IT(&htim4);
    //Czyszczenie buforow
//    memset(tablicaPomiarowDynamicznych, 0, sizeof(tablicaPomiarowDynamicznych));
//    memset(tablicaHistoriiPrzedPomiarem, 0, sizeof(tablicaHistoriiPrzedPomiarem));

    // Oddzielamy dane od CRC (ostatnie 4 znaki przed '\r\n' to tekstowe CRC)
    char crc_text[5] = {0};
    memcpy(crc_text, &odbiorUART.buffer[length - 6], 4);  // Kopiujemy 4 znaki tekstowego CRC (np. "a9f9")

    // Parsujemy tekstowe CRC na wartość liczbową
    uint16_t crc_received = parse_crc_from_text(crc_text);
    //printf("Mam nowe crc_received: %d\r\n", crc_received);

    // Obliczamy CRC dla odebranych danych (bez 'U' i bez CRC)
    uint16_t crc_calculated = crc16_ccitt_false((const char*)odbiorUART.buffer, length - 6);
    //printf("A crc calculated t o : %d\r\n", crc_calculated);

    // Sprawdzamy poprawność CRC
    if (crc_calculated == crc_received) {
        // Przekazujemy dane bez CRC i nagłówka 'U' do przetworzenia
        process_received_data(odbiorUART.buffer + 2, length - 7);  // -7 to U + CRC (4 znaki) + \r\n
    } else {
        printf("Błąd CRC! Odebrano: %u, Obliczono: %u\n", crc_received, crc_calculated);
    }
}

void sendDataRamkaDynamT(uint32_t *tablicaPomiarowDynamicznych, int czasPomiaruDynam)
{
    const int maxBufSize = 3*MAX_POMIARY;  // Większy bufor, aby zmieścić całą ramkę
    char frame[maxBufSize];      // Bufor dla całej ramki
    int czasPomNa2 = czasPomiaruDynam/2;
    int index = 0;

    // Dodanie nagłówka "P D [C5 lub C6]"
    index += snprintf(frame + index, sizeof(frame) - index, "P D %s ", warunkiPomiaru.idCzujnika);

    // Dodanie informacji o rodzaju pomiaru: "T [czasPomiaruDynam]"
    index += snprintf(frame + index, sizeof(frame) - index, "T %d ", czasPomiaruDynam);


    int wyliczonyIdxHistorii = warunkiPomiaru.indeksHistorii;
    // Dodanie historii pomiaru
    for ( int licznik = 0; licznik<ROZM_HISTORIA; licznik++)
    {
    	index += snprintf(frame + index, sizeof(frame) - index, "%lu ", danePomiaroweDynam.tablicaHistoriiPrzedPomiarem[wyliczonyIdxHistorii]);
    	wyliczonyIdxHistorii = (wyliczonyIdxHistorii + 1) % ROZM_HISTORIA;
    }
    // Dodanie danych pomiarowych

    for (int i = 0; i < czasPomNa2; i++) {
    	if(danePomiaroweDynam.tablicaPomiarowDynamicznych[i] > 0)
    	{
        index += snprintf(frame + index, sizeof(frame) - index, "%lu ", tablicaPomiarowDynamicznych[i] );
    	}
    }

    // Usuwanie ostatniej spacji, jeśli istnieje
    if (index > 0 && frame[index - 1] == ' ') {
        index--; // Cofnięcie indeksu, aby pominąć końcową spację
    }

    // Obliczenie CRC dla danych w ramce (bez końcowej spacji)
    uint16_t crc = crc16_ccitt_false((char *)frame, index);

    // Dodanie CRC na końcu ramki
    index += snprintf(frame + index, sizeof(frame) - index, " %u\r\n", crc);

    // Wysyłanie całej ramki jednorazowo
    HAL_UART_Transmit(&huart2, (uint8_t *)frame, index, HAL_MAX_DELAY);

    // Wyczyszczenie tablicy po wysłaniu danych
    memset(tablicaPomiarowDynamicznych, 0, sizeof(int) * MAX_POMIARY);  // Zerowanie zawartości tablicy
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        static uint16_t length = 0;

        // Sprawdzamy, czy ramka zaczyna się od 'U'
        if (length == 0 && odbiorUART.receivedData != 'U') {
        	printf("Nie ma U na poczatku\r\n");
            // Ignorujemy, jeśli pierwszy znak nie jest 'U'
            HAL_UART_Receive_IT(&huart2, &odbiorUART.receivedData, 1);
            return;
        }

        odbiorUART.buffer[length++] = odbiorUART.receivedData;  // Buforowanie odebranego znaku

        // Sprawdzamy, czy ramka kończy się na '\r\n'
        if (length >= 2 && odbiorUART.buffer[length - 2] == '\r' && odbiorUART.buffer[length - 1] == '\n') {
            receive_data_frame(length);  // Przetwarzamy ramkę danych
            length = 0;  // Resetujemy długość bufora po zakończeniu ramki
        }

        // Włączamy ponownie przerwanie odbioru danych UART
        HAL_UART_Receive_IT(&huart2, &odbiorUART.receivedData, 1);
    }
}
