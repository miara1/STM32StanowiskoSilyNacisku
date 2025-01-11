#include "adc.h"

WartosciADC wartosciADC = {

		.wartosc1 = 0,
		.wartosc2 = 0,
		.wartosc3 = 0,
		.wartosc4 = 0,
		.wartosc5 = 0,
		.wartosc6 = 0

};

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

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if( (hadc->Instance == ADC1) && (warunkiPomiaru.flagaZmiany == 1) && (warunkiPomiaru.idPomiaruDynam[0] == 'T') )
	{
		sendDataRamkaDynamT(danePomiaroweDynam.tablicaPomiarowDynamicznych, warunkiPomiaru.czasPomiaruDynam);
		HAL_ADC_Stop_DMA(&hadc1);
	}

	if( (hadc->Instance == ADC1) && (warunkiPomiaru.flagaZmiany == 1) && (warunkiPomiaru.idPomiaruDynam[0] == 'X') )
	{
	}
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc) {
    printf("ADC DMA Error\n");
}

