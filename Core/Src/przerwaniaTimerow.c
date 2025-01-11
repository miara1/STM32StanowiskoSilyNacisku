/*!
 * @file przerwaniaTimerow.c
 * @brief Implementacja funkcji obsługujących przerwania timerów
 *
 * Plik zawiera implementację funkcji obsługujących przerwania timerów,
 * odpowiedzialnych za realizację pomiarów w różnych trybach, takich jak
 * matryca, pomiar dynamiczny i statyczny.
 */

#include "przerwaniaTimerow.h"

/*!
 * Funkcja sprawdza, który timer wywołał przerwanie, zatrzymuje pozostałe timery
 * i wywołuje odpowiednią funkcję obsługi przerwania.
 *
 * @param[in] htim - wskaźnik na uchwyt timera
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

	if(htim == &htim2)
	{
        HAL_TIM_Base_Stop_IT(&htim3);
        HAL_TIM_Base_Stop_IT(&htim4);

        obsluzPrzerwanieMatrycy();
	}

	if (htim == &htim3)
	    {
        HAL_TIM_Base_Stop_IT(&htim2);
        HAL_TIM_Base_Stop_IT(&htim4);

        obsluzPrzerwanieDynam();
	    }

	if (htim == &htim4)
	{
        HAL_TIM_Base_Stop_IT(&htim3);
        HAL_TIM_Base_Stop_IT(&htim2);

        obsluzPrzerwanieStat();

	}
}

/*!
 * Funkcja wykonuje odczyty z czterech kanałów ADC w trybie matrycy.
 * Wyniki pomiarów są zapisywane w strukturze ,,wartosciADC'' i wysyłane przez UART.
 *
 * Obsługa pomiarów realizowana jest za pomocą instrukcji ,,switch'', gdzie w każdej
 * iteracji wykonywany jest odczyt z kolejnego kanału ADC. Wyniki pomiarów są
 * przechowywane w odpowiednich polach struktury ,,wartosciADC''. Po wykonaniu
 * odczytów wszystkich kanałów, dane są formatowane i wysyłane przez UART.
 */
void obsluzPrzerwanieMatrycy()
{
	static uint8_t licznikDoPomiaruMatrycy = 0;
	if( warunkiPomiaru.idPomiaru == ID_POMIAR_MATRYCY && warunkiPomiaru.idCzujnika[0] == ID_CZUJNIK_BRAK )
	{
		switch(licznikDoPomiaruMatrycy)
		{
		case 0:
			HAL_ADC_Stop(&hadc1);
			ADC_Select_CH0();
			HAL_ADC_Start(&hadc1);
			HAL_ADC_PollForConversion(&hadc1, 1000);
			wartosciADC.wartosc1=HAL_ADC_GetValue(&hadc1);
			break;

		case 1:
			HAL_ADC_Stop(&hadc1);
			ADC_Select_CH1();
			HAL_ADC_Start(&hadc1);
			HAL_ADC_PollForConversion(&hadc1, 1000);
			wartosciADC.wartosc2=HAL_ADC_GetValue(&hadc1);
			break;

		case 2:
			HAL_ADC_Stop(&hadc1);
			ADC_Select_CH4();
			HAL_ADC_Start(&hadc1);
			HAL_ADC_PollForConversion(&hadc1, 1000);
			wartosciADC.wartosc3=HAL_ADC_GetValue(&hadc1);
			break;

		case 3:
			HAL_ADC_Stop(&hadc1);
			ADC_Select_CH8();
			HAL_ADC_Start(&hadc1);
			HAL_ADC_PollForConversion(&hadc1, 1000);
			wartosciADC.wartosc4=HAL_ADC_GetValue(&hadc1);

			// Wyslij raz na 4 przerwania zegara
			char dane[100];

			sprintf(dane, "P M C1 %d C2 %d C3 %d C4 %d", wartosciADC.wartosc1, wartosciADC.wartosc2,
					wartosciADC.wartosc3, wartosciADC.wartosc4 );
			int crc16=crc16_ccitt_false(dane, strlen(dane));
			printf( "%s %0*d\r\n", dane, 5, crc16);
			break;

		default:
			break;
		}

		licznikDoPomiaruMatrycy = (licznikDoPomiaruMatrycy + 1) %4;
	}
}

/*!
 * Funkcja realizuje obsługę pomiarów dynamicznych z czujnika 5 i 6.
 * Na podstawie warunków pomiaru oraz wartości progowych określa, czy należy
 * rozpocząć rejestrację danych pomiarowych w trybie czasowym lub bez limitu.
 *
 * Wykonywane operacje:
 * - Odczyt wartości z wybranego kanału ADC na podstawie ustawień czujnika.
 * - Aktualizacja tablicy historii pomiarów przed detekcją zmiany.
 * - Sprawdzanie, czy wartość progowa została przekroczona i inicjacja odpowiedniego trybu pomiarowego.
 */
void obsluzPrzerwanieDynam()
{
	int tempValPom = 0;
	if ( warunkiPomiaru.idCzujnika[1] == ID_CZUJNIK_CP_150 )
	{
		HAL_ADC_Stop(&hadc1);
		ADC_Select_CH11();
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 1000);
		wartosciADC.wartosc5=HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);

		tempValPom = wartosciADC.wartosc5;

	} else if( warunkiPomiaru.idCzujnika[1] == ID_CZUJNIK_CP_154 ) {

		HAL_ADC_Stop(&hadc1);
		ADC_Select_CH10();
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 1000);
		wartosciADC.wartosc6=HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);

		tempValPom = wartosciADC.wartosc6;
	}

	if ( !warunkiPomiaru.flagaZmiany)
	{
		danePomiaroweDynam.tablicaHistoriiPrzedPomiarem[warunkiPomiaru.indeksHistorii] = tempValPom;
		warunkiPomiaru.indeksHistorii = (warunkiPomiaru.indeksHistorii + 1) % ROZM_HISTORIA;  // Przejście na kolejny indeks cyklicznie
	}

	if (( (wartosciADC.wartosc6 > warunkiPomiaru.progPomiaruDynam) && warunkiPomiaru.idCzujnika[1] == ID_CZUJNIK_CP_154) ||
			( (wartosciADC.wartosc5 > warunkiPomiaru.progPomiaruDynam) && warunkiPomiaru.idCzujnika[1] == ID_CZUJNIK_CP_150))
	{
		if( warunkiPomiaru.idPomiaruDynam[0] == ID_DYNAM_CZASOWY )
		{
			// Sprawdzenie, czy zmieniło się z <30 na >30
			if (!warunkiPomiaru.flagaZmiany)
			{
				if( warunkiPomiaru.idCzujnika[1] == ID_CZUJNIK_CP_150)
				{
					printf("T oraz 5");
					//printf("ZaczynamDynamT!!!\r\n");
					HAL_ADC_Stop(&hadc1);
					ADC_Select_CH11();
					HAL_ADC_Start_DMA(&hadc1, (uint32_t*)danePomiaroweDynam.tablicaPomiarowDynamicznych, MAX_POMIARY);
					warunkiPomiaru.flagaZmiany = 1;  // Ustawienie flagi zmiany

				} else if ( warunkiPomiaru.idCzujnika[1] == ID_CZUJNIK_CP_154 ) {

					HAL_ADC_Stop(&hadc1);
					ADC_Select_CH10();
					HAL_ADC_Start_DMA(&hadc1, (uint32_t*)danePomiaroweDynam.tablicaPomiarowDynamicznych, MAX_POMIARY);
					warunkiPomiaru.flagaZmiany = 1;  // Ustawienie flagi zmiany
				}

			}

		} else if( warunkiPomiaru.idPomiaruDynam[0] == ID_DYNAM_BEZ_LIMITU ) {

			if(!warunkiPomiaru.flagaZmiany)
			{
				if( warunkiPomiaru.idCzujnika[1] == ID_CZUJNIK_CP_150 )
				{
					//						HAL_ADC_Start_DMA(hadc, dma_buff, iloscPunktowDoSredniej);
					//						flagaZmiany = 1;

				} else if( warunkiPomiaru.idCzujnika[1] == ID_CZUJNIK_CP_154 )

					printf("D C6 oraz X\r\n");
			}
		}
	}
}

/*!
 * Funkcja realizuje pomiary statyczne dla czujników CP150 oraz CP154.
 * Na podstawie konfiguracji w strukturze ,,warunkiPomiaru'' wybierany jest odpowiedni kanał ADC.
 * Wyniki pomiarów są przesyłane przez UART w postaci ramki z obliczonym CRC.
 *
 * Wykonywane operacje:
 * - Sprawdzenie, czy tryb pracy to pomiar statyczny (,,ID_POMIAR_STAT'').
 * - Odczyt wartości z ADC dla czujnika 5 (,,ID_CZUJNIK_CP_150'') lub 6 (,,ID_CZUJNIK_CP_154'').
 * - Generacja ramki danych w formacie „P S Cx <wartość>” oraz obliczenie sumy kontrolnej CRC.
 * - Wysłanie ramki przez UART.
 */
void obsluzPrzerwanieStat()
{
	if( warunkiPomiaru.idPomiaru == ID_POMIAR_STAT )
	{

		if( warunkiPomiaru.idCzujnika[1] == ID_CZUJNIK_CP_150 )
		{
			  HAL_ADC_Stop(&hadc1);
			  ADC_Select_CH11();
			  HAL_ADC_Start(&hadc1);
			  HAL_ADC_PollForConversion(&hadc1, 1000);
			  wartosciADC.wartosc5=HAL_ADC_GetValue(&hadc1);

			  char dane[100];
			  sprintf(dane, "P S C5 %d", wartosciADC.wartosc5);
			  int crc16=crc16_ccitt_false(dane, strlen(dane));
			  printf( "%s %0*d\r\n", dane, 5, crc16);
		}

		if( warunkiPomiaru.idCzujnika[1] == ID_CZUJNIK_CP_154 )
		{
			  HAL_ADC_Stop(&hadc1);
			  ADC_Select_CH10();
			  HAL_ADC_Start(&hadc1);
			  HAL_ADC_PollForConversion(&hadc1, 1000);
			  wartosciADC.wartosc6=HAL_ADC_GetValue(&hadc1);

			  char dane[100];
			  sprintf(dane, "P S C6 %d", wartosciADC.wartosc6);
			  int crc16=crc16_ccitt_false(dane, strlen(dane));
			  printf( "%s %0*d\r\n", dane, 5, crc16);
		}

	}
}


