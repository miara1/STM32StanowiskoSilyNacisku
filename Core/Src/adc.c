/*!
 * @file adc.c
 * @brief Implementacja funkcji i callbacków związanych z obsługą ADC
 *
 * Plik zawiera implementację funkcji odpowiedzialnych za konfigurację
 * i obsługę przetwornika ADC, w tym wybór kanałów, obsługę przerwań
 * oraz obsługę błędów.
 */

#include "adc.h"

/*!
 * \brief Inicjalizacja struktury przechowującej wartości ADC.
 *
 * Struktura ,,wartosciADC'' przechowuje wartości odczytane z kanałów ADC.
 * Każda wartość jest ustawiona na 0 podczas inicjalizacji.
 */
WartosciADC wartosciADC = {

		.wartosc1 = 0,
		.wartosc2 = 0,
		.wartosc3 = 0,
		.wartosc4 = 0,
		.wartosc5 = 0,
		.wartosc6 = 0

};

/*!
 * @brief Funkcja wybierająca kanał ADC do pomiarów z czujnika 1.
 *
 * Funkcja konfiguruje przetwornik ADC, aby korzystał z kanału 0.
 * W przypadku niepowodzenia konfiguracji wywołuje funkcję ,,Error_Handler''.
 */
void ADC_Select_CH0 (void)
{
	ADC_ChannelConfTypeDef sConfig ={0};

	  sConfig.Channel = ADC_CHANNEL_0;
	  sConfig.Rank = 1;
	  sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
	  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

/*!
 * @brief Funkcja wybierająca kanał ADC do pomiarów z czujnika 2.
 *
 * Funkcja konfiguruje przetwornik ADC, aby korzystał z kanału 1.
 * W przypadku niepowodzenia konfiguracji wywołuje funkcję ,,Error_Handler''.
 */
void ADC_Select_CH1 (void)
{
	ADC_ChannelConfTypeDef sConfig ={0};
	  sConfig.Channel = ADC_CHANNEL_1;
	  sConfig.Rank = 1;
	  sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
	  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

/*!
 * @brief Funkcja wybierająca kanał ADC do pomiarów z czujnika 3.
 *
 * Funkcja konfiguruje przetwornik ADC, aby korzystał z kanału 4.
 * W przypadku niepowodzenia konfiguracji wywołuje funkcję ,,Error_Handler''.
 */
void ADC_Select_CH4 (void)
{
	ADC_ChannelConfTypeDef sConfig ={0};
	  sConfig.Channel = ADC_CHANNEL_4;
	  sConfig.Rank = 1;
	  sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
	  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

/*!
 * @brief Funkcja wybierająca kanał ADC do pomiarów z czujnika 4.
 *
 * Funkcja konfiguruje przetwornik ADC, aby korzystał z kanału 8.
 * W przypadku niepowodzenia konfiguracji wywołuje funkcję ,,Error_Handler''.
 */
void ADC_Select_CH8 (void)
{
	ADC_ChannelConfTypeDef sConfig ={0};
	  sConfig.Channel = ADC_CHANNEL_8;
	  sConfig.Rank = 1;
	  sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
	  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

/*!
 * @brief Funkcja wybierająca kanał ADC do pomiarów z czujnika 6.
 *
 * Funkcja konfiguruje przetwornik ADC, aby korzystał z kanału 10.
 * W przypadku niepowodzenia konfiguracji wywołuje funkcję ,,Error_Handler''.
 */
void ADC_Select_CH10 (void)
{
	ADC_ChannelConfTypeDef sConfig ={0};
	  sConfig.Channel = ADC_CHANNEL_10;
	  sConfig.Rank = 1;
	  sConfig.SamplingTime = ADC_SAMPLETIME_56CYCLES;
	  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

/*!
 * @brief Funkcja wybierająca kanał ADC do pomiarów z czujnika 5.
 *
 * Funkcja konfiguruje przetwornik ADC, aby korzystał z kanału 11.
 * W przypadku niepowodzenia konfiguracji wywołuje funkcję ,,Error_Handler''.
 */
void ADC_Select_CH11 (void)
{
	ADC_ChannelConfTypeDef sConfig ={0};
	  sConfig.Channel = ADC_CHANNEL_11;
	  sConfig.Rank = 1;
	  sConfig.SamplingTime = ADC_SAMPLETIME_56CYCLES;
	  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

/*!
 * Funkcja obsługuje zakończenie konwersji ADC. Jeśli spełnione są określone
 * warunki, dane są przesyłane lub zatrzymywana jest praca DMA.
 *
 * @param[in] hadc - wskaźnik na uchwyt ADC
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if( (hadc->Instance == ADC1) && (warunkiPomiaru.flagaZmiany == 1) &&
			(warunkiPomiaru.idPomiaruDynam[0] == ID_DYNAM_CZASOWY) )
	{
		sendDataRamkaDynamT(danePomiaroweDynam.tablicaPomiarowDynamicznych, warunkiPomiaru.czasPomiaruDynam);
		HAL_ADC_Stop_DMA(&hadc1);
	}

	if( (hadc->Instance == ADC1) && (warunkiPomiaru.flagaZmiany == 1) &&
			(warunkiPomiaru.idPomiaruDynam[0] == ID_DYNAM_BEZ_LIMITU) )
	{
	}
}

/*!
 * Funkcja wywoływana w przypadku wystąpienia błędu w pracy przetwornika ADC.
 * Komunikuje błąd za pomocą komunikatu debugowego.
 *
 * @param[in] hadc - wskaźnik na uchwyt ADC
 */
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc) {
    printf("ADC DMA Error\n");
}

