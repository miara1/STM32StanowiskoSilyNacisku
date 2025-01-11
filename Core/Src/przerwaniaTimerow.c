#include "przerwaniaTimerow.h"

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

void obsluzPrzerwanieMatrycy()
{
	static uint8_t licznikDoPomiaruMatrycy = 0;
	if( warunkiPomiaru.idPomiaru == 'M' && warunkiPomiaru.idCzujnika[0] == '0' )
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

void obsluzPrzerwanieDynam()
{
	int tempValPom = 0;
	if ( warunkiPomiaru.idCzujnika[1] == '5' )
	{
		HAL_ADC_Stop(&hadc1);
		ADC_Select_CH11();
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 1000);
		wartosciADC.wartosc5=HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);

		tempValPom = wartosciADC.wartosc5;

	} else if( warunkiPomiaru.idCzujnika[1] == '6' ) {

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

	if (( (wartosciADC.wartosc6 > warunkiPomiaru.progPomiaruDynam) && warunkiPomiaru.idCzujnika[1] == '6') ||
			( (wartosciADC.wartosc5 > warunkiPomiaru.progPomiaruDynam) && warunkiPomiaru.idCzujnika[1] == '5'))
	{
		if( warunkiPomiaru.idPomiaruDynam[0] == 'T' )
		{
			// Sprawdzenie, czy zmieniło się z <30 na >30
			if (!warunkiPomiaru.flagaZmiany)
			{
				if( warunkiPomiaru.idCzujnika[1] == '5')
				{
					printf("T oraz 5");
					//printf("ZaczynamDynamT!!!\r\n");
					HAL_ADC_Stop(&hadc1);
					ADC_Select_CH11();
					HAL_ADC_Start_DMA(&hadc1, (uint32_t*)danePomiaroweDynam.tablicaPomiarowDynamicznych, MAX_POMIARY);
					warunkiPomiaru.flagaZmiany = 1;  // Ustawienie flagi zmiany

				} else if (warunkiPomiaru.idCzujnika[1] == '6') {

					HAL_ADC_Stop(&hadc1);
					ADC_Select_CH10();
					HAL_ADC_Start_DMA(&hadc1, (uint32_t*)danePomiaroweDynam.tablicaPomiarowDynamicznych, MAX_POMIARY);
					warunkiPomiaru.flagaZmiany = 1;  // Ustawienie flagi zmiany
				}

			}

		} else if(strcmp(warunkiPomiaru.idPomiaruDynam, "X") == 0) {

			if(!warunkiPomiaru.flagaZmiany)
			{
				if(strcmp(warunkiPomiaru.idCzujnika, "C5") == 0)
				{
					//						HAL_ADC_Start_DMA(hadc, dma_buff, iloscPunktowDoSredniej);
					//						flagaZmiany = 1;

				} else if(strcmp(warunkiPomiaru.idCzujnika, "C6") == 0)

					printf("D C6 oraz X\r\n");
			}
		}
	}
}

void obsluzPrzerwanieStat()
{
	if( warunkiPomiaru.idPomiaru == 'S' )
	{

		if(strcmp(warunkiPomiaru.idCzujnika, "C5") == 0)
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

		if(strcmp(warunkiPomiaru.idCzujnika, "C6") == 0)
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


