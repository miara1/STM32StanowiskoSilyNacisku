#ifndef INC_PRZETWORZDANE_H_
#define INC_PRZETWORZDANE_H_

#include "main.h"

typedef struct
{
	uint32_t tablicaPomiarowDynamicznych[MAX_POMIARY];  // Bufor danych pomiaru dynamicznego
	uint32_t tablicaHistoriiPrzedPomiarem[ROZM_HISTORIA];
}DanePomiaroweDynam;

typedef struct
{
	char idCzujnika[3];
	char idPomiaruDynam[2];
	char idPomiaru;
	uint16_t czasPomiaruDynam;
	uint16_t progPomiaruDynam;
	uint16_t minimalnaZmiana;
	uint16_t iloscPunktowDoSredniej;
	uint16_t indeksHistorii;
	int flagaZmiany;

}WarunkiPomiaru;

extern DanePomiaroweDynam danePomiaroweDynam;
extern WarunkiPomiaru warunkiPomiaru;

void process_received_data(uint8_t *data, uint16_t length);


#endif /* INC_PRZETWORZDANE_H_ */
