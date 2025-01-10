/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MAX_POMIARY 5000
#define FRAME_END "\r\n"  // Zakończenie ramki
#define MAX_FRAME_SIZE 50  // Maksymalny rozmiar ramki danych
#define ROZM_HISTORIA 100
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
volatile int adc_flag;
volatile int adc_value1;
volatile int adc_value2;
volatile int adc_value3;
volatile int adc_value4;
volatile int adc_value5;
volatile int adc_value6;

char dane[100];
uint8_t receivedData = 0;

uint8_t buffer[MAX_FRAME_SIZE]; // Bufor do odbioru ramki danych
uint8_t data[MAX_FRAME_SIZE - 5]; // Bufor na dane bez CRC (zakładamy 4-znakowe CRC)

//char tablicaPomiarowDynamicznych[500] = "";
volatile int tempAdcValue = 0;
volatile int czyZadzialalo = 0;

char idCzujnika[3];
char idPomiaruDynam[2];
char idPomiaru;
uint16_t czasPomiaruDynam = 0;
uint16_t progPomiaruDynam = 0;

int tmpVal = 0;


uint8_t licznikDoPomiaruMatrycy = 0;


uint32_t tablicaPomiarowDynamicznych[MAX_POMIARY];  // Bufor
uint32_t tablicaHistoriiPrzedPomiarem[ROZM_HISTORIA] = {0};
uint16_t indeksHistorii = 0;

int licznikPoczatku = 0;  // Wskaźnik na najstarszy pomiar
int liczbaPomiarow = 0;   // Licznik faktycznej liczby pomiarów w buforze
int licznikDanychPowyzej30 = 0;  // Licznik pomiarów powyżej 30
int flagaZmiany = 0;  // Flaga wykrywająca zmianę z <30 na >30
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int __io_putchar(int ch)
{

	HAL_UART_Transmit(&huart2, (uint8_t*)&ch, 1, HAL_MAX_DELAY);
	return ch;
}





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


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        static uint16_t length = 0;

        // Sprawdzamy, czy ramka zaczyna się od 'U'
        if (length == 0 && receivedData != 'U') {
        	printf("Nie ma U na poczatku\r\n");
            // Ignorujemy, jeśli pierwszy znak nie jest 'U'
            HAL_UART_Receive_IT(&huart2, &receivedData, 1);
            return;
        }

        buffer[length++] = receivedData;  // Buforowanie odebranego znaku

        // Sprawdzamy, czy ramka kończy się na '\r\n'
        if (length >= 2 && buffer[length - 2] == '\r' && buffer[length - 1] == '\n') {
            receive_data_frame(length);  // Przetwarzamy ramkę danych
            length = 0;  // Resetujemy długość bufora po zakończeniu ramki
        }

        // Włączamy ponownie przerwanie odbioru danych UART
        HAL_UART_Receive_IT(&huart2, &receivedData, 1);
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

	if(htim == &htim2)
	{
		if( idPomiaru == 'S' )
		{
			if(strcmp(idCzujnika, "C5") == 0)
			{
				  HAL_ADC_Stop(&hadc1);
				  ADC_Select_CH11();
				  HAL_ADC_Start(&hadc1);
				  HAL_ADC_PollForConversion(&hadc1, 1000);
				  adc_value5=HAL_ADC_GetValue(&hadc1);

				sprintf(dane, "P S C5 %d", adc_value5);
				int crc16=crc16_ccitt_false(dane, strlen(dane));
				printf( "%s %0*d\r\n", dane, 5, crc16);
			}

			if(strcmp(idCzujnika, "C6") == 0)
			{
				  HAL_ADC_Stop(&hadc1);
				  ADC_Select_CH10();
				  HAL_ADC_Start(&hadc1);
				  HAL_ADC_PollForConversion(&hadc1, 1000);
				  adc_value6=HAL_ADC_GetValue(&hadc1);


				sprintf(dane, "P S C6 %d", adc_value6);
				int crc16=crc16_ccitt_false(dane, strlen(dane));
				printf( "%s %0*d\r\n", dane, 5, crc16);
			}
		}

		if( idPomiaru == 'M')
		{
			switch(licznikDoPomiaruMatrycy)
			{
			case 0:
			  HAL_ADC_Stop(&hadc1);
			  ADC_Select_CH0();
			  HAL_ADC_Start(&hadc1);
			  HAL_ADC_PollForConversion(&hadc1, 1000);
			  adc_value1=HAL_ADC_GetValue(&hadc1);
			  break;

			case 1:
			  HAL_ADC_Stop(&hadc1);
			  ADC_Select_CH1();
			  HAL_ADC_Start(&hadc1);
			  HAL_ADC_PollForConversion(&hadc1, 1000);
			  adc_value2=HAL_ADC_GetValue(&hadc1);
			  break;

			case 2:
			  HAL_ADC_Stop(&hadc1);
			  ADC_Select_CH4();
			  HAL_ADC_Start(&hadc1);
			  HAL_ADC_PollForConversion(&hadc1, 1000);
			  adc_value3=HAL_ADC_GetValue(&hadc1);
			  break;

			case 3:
			  HAL_ADC_Stop(&hadc1);
			  ADC_Select_CH8();
			  HAL_ADC_Start(&hadc1);
			  HAL_ADC_PollForConversion(&hadc1, 1000);
			  adc_value4=HAL_ADC_GetValue(&hadc1);

			// Wyslij raz na 4 przerwania zegara
			sprintf(dane, "P M C1 %d C2 %d C3 %d C4 %d", adc_value1, adc_value2, adc_value3, adc_value4 );
			int crc16=crc16_ccitt_false(dane, strlen(dane));
			printf( "%s %0*d\r\n", dane, 5, crc16);
			  break;

			default:
			  break;
			}

			licznikDoPomiaruMatrycy = (licznikDoPomiaruMatrycy + 1) %4;
		}
		/*
		  sprintf(dane, "S1 %d S2 %d S3 %d S4 %d S5 %d S6 %d", adc_value1, adc_value2, adc_value3, adc_value4, adc_value5, adc_value6);
		  int crc16=crc16_ccitt_false(dane, strlen(dane));
		  printf( "%s %0*d\r\n", dane, 5, crc16);
		  */
	}

	if (htim == &htim3)
	    {
		//printf( "htim3\r\n" );

		int tempValPom = 0;
			if( idPomiaruDynam[0] == 'T' )
			{
				if ( idCzujnika[1] == '5' )
				{

					tmpVal = (++tmpVal)%10000;
					  HAL_ADC_Stop(&hadc1);
					  ADC_Select_CH11();
					  HAL_ADC_Start(&hadc1);
					  HAL_ADC_PollForConversion(&hadc1, 1000);
					  adc_value5=HAL_ADC_GetValue(&hadc1);
					  HAL_ADC_Stop(&hadc1);
					  if ( !tmpVal )
					  {
						  printf("htim3 cz5: %d\r\n", adc_value5);
						  //printf("Prog pomiarow: %d\r\n", progPomiaruDynam);
					  }
					  tempValPom = adc_value5;

				} else if( idCzujnika[1] == '6' ) {

					  HAL_ADC_Stop(&hadc1);
					  ADC_Select_CH10();
					  HAL_ADC_Start(&hadc1);
					  HAL_ADC_PollForConversion(&hadc1, 1000);
					  adc_value6=HAL_ADC_GetValue(&hadc1);
					  HAL_ADC_Stop(&hadc1);

					  tempValPom = adc_value6;
				}

				if ( !flagaZmiany)
				{
			    tablicaHistoriiPrzedPomiarem[indeksHistorii] = tempValPom;
			    indeksHistorii = (indeksHistorii + 1) % ROZM_HISTORIA;  // Przejście na kolejny indeks cyklicznie
				}
//				if ( (adc_value5 <= progPomiaruDynam) && flagaZmiany ) flagaZmiany = 0;

		        if (( (adc_value6 > progPomiaruDynam) && idCzujnika[1] == '6') ||
		        	( (adc_value5 > progPomiaruDynam) && idCzujnika[1] == '5'))
		        {
		        	//printf("aasddasdasd");
		            // Sprawdzenie, czy zmieniło się z <30 na >30
		            if (!flagaZmiany)
		            {
		            	//printf("ZaczynamDynamT!!!\r\n");
		            	HAL_ADC_Stop(&hadc1);
		            	ADC_Select_CH11();
		            	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)tablicaPomiarowDynamicznych, MAX_POMIARY);
		                flagaZmiany = 1;  // Ustawienie flagi zmiany
		            }

//		            // Dodajemy nowy pomiar do bufora w zaleznosci czy C5 czy C6
//		            if(strcmp(idCzujnika, "C5") == 0)
//		            {
//		            	tablicaPomiarowDynamicznych[licznikPoczatku] = adc_value5;
//
//		            } else if (strcmp(idCzujnika, "C6") == 0) {
//
//		            	tablicaPomiarowDynamicznych[licznikPoczatku] = adc_value6;
//		            }
//
//		            // Przesuwamy wskaźnik na następną pozycję, z uwzględnieniem cykliczności
//		            licznikPoczatku = (licznikPoczatku + 1) % maxPomiarow;
//
//		            // Aktualizujemy licznik liczby pomiarów, ale nie przekraczamy limitu
//		            if (liczbaPomiarow < maxPomiarow)
//		            {
//		                liczbaPomiarow++;
//		            }
//
//		            // Zwiększamy licznik danych powyżej 30
//		            licznikDanychPowyzej30++;
//
//		            // Jeśli osiągnęliśmy 90 pomiarów powyżej 30, wyświetlamy zawartość bufora
//		            if (licznikDanychPowyzej30 >= maxPomiarow-10 && flagaZmiany == 1)
//		            {
//		                // Zmieniamy flagę na 2, aby zapobiec ponownemu wyświetlaniu
//		                flagaZmiany = 2;
//
//		                // Wyświetlanie wszystkich danych z bufora
//		                sendDataRamkaDynamT(tablicaPomiarowDynamicznych, czasPomiaruDynam);

		            }
		        }
		        else
		        {
		            // Gdy `adc_value6` spadnie poniżej 30, zerujemy flagę zmiany i licznik danych
		            flagaZmiany = 0;
		            licznikDanychPowyzej30 = 0;  // Reset licznika tylko po spadku wartości poniżej 30
		        }

			} else if(strcmp(idPomiaruDynam, "X") == 0) {
				if(strcmp(idCzujnika, "C5") == 0)
				{
					printf("D C5 oraz X\r\n");

				} else if(strcmp(idCzujnika, "C6") == 0)

					printf("D C6 oraz X\r\n");
				}
			}

		/*
	        if (adc_value6 > 30)
	        {
	            // Sprawdzenie, czy zmieniło się z <30 na >30
	            if (!flagaZmiany)
	            {			if(strcmp(idCzujnika, "C5") == 0)
			{

				  HAL_ADC_PollForConversion(&hadc1, 1000);
				  adc_value5=HAL_ADC_GetValue(&hadc1);

				sprintf(dane, "P S C5 %d", adc_value5);
				int crc16=crc16_ccitt_false(dane, strlen(dane));
				printf( "%s %0*d\r\n", dane, 5, crc16);
			}

			if(strcmp(idCzujnika, "C6") == 0)
			{
				sprintf(dane, "P S C6 %d", adc_value6);
				int crc16=crc16_ccitt_false(dane, strlen(dane));
				printf( "%s %0*d\r\n", dane, 5, crc16);
			}
	                flagaZmiany = 1;  // Ustawienie flagi zmiany
	            }

	            // Dodajemy nowy pomiar do bufora
	            tablicaPomiarowDynamicznych[licznikPoczatku] = adc_value6;

	            // Przesuwamy wskaźnik na następną pozycję, z uwzględnieniem cykliczności
	            licznikPoczatku = (licznikPoczatku + 1) % MAX_POMIARY;

	            // Aktualizujemy licznik liczby pomiarów, ale nie przekraczamy limitu
	            if (liczbaPomiarow < MAX_POMIARY)
	            {
	                liczbaPomiarow++;
	            }

	            // Zwiększamy licznik danych powyżej 30
	            licznikDanychPowyzej30++;

	            // Jeśli osiągnęliśmy 90 pomiarów powyżej 30, wyświetlamy zawartość bufora
	            if (licznikDanychPowyzej30 >= MAX_POMIARY-10 && flagaZmiany == 1)
	            {
	                // Zmieniamy flagę na 2, aby zapobiec ponownemu wyświetlaniu
	                flagaZmiany = 2;

	                // Wyświetlanie wszystkich danych z bufora
	                sendDataRamkaDynam(tablicaPomiarowDynamicznych, MAX_POMIARY);

	            }
	        }
	        else
	        {
	            // Gdy `adc_value6` spadnie poniżej 30, zerujemy flagę zmiany i licznik danych
	            flagaZmiany = 0;
	            licznikDanychPowyzej30 = 0;  // Reset licznika tylko po spadku wartości poniżej 30
	        }
	        	    */
//	    }

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
	  //HAL_Delay_us(1);
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
	  //HAL_Delay_us(1);
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
	  //HAL_Delay_us(1);
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
	  //HAL_Delay_us(1);
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
	  //HAL_Delay_us(1);
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
	  //HAL_Delay_us(1);
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
//    if (hadc->Instance == ADC1) {
//        printf("DMA Transfer Complete\r\n");
//        for (int i = 0; i < 10; i++) { // Wyświetl kilka pierwszych wartości
//            printf("tablica[%d] = %lu\r\n", i, tablicaPomiarowDynamicznych[i]);
//        }
//    }

	if( (hadc->Instance == ADC1) && (flagaZmiany == 1) && (idPomiaruDynam[0] == 'T') )
	{
		sendDataRamkaDynamT(tablicaPomiarowDynamicznych, czasPomiaruDynam);
		HAL_ADC_Stop_DMA(&hadc1);
	}

//	static uint8_t counter = 0;
//
//	switch (counter)
//	{
//	case 0:
//		ADC_Select_CH0();
//		adc_value4 = HAL_ADC_GetValue(&hadc1);
//		counter = 1;
//		break;
//
//	case 1:
//		ADC_Select_CH1();
//		adc_value2 = HAL_ADC_GetValue(&hadc1);
//		counter = 2;
//		break;
//
//	case 2:
//		ADC_Select_CH4();
//		adc_value5 = HAL_ADC_GetValue(&hadc1);
//		counter = 3;
//		break;
//
//	case 3:
//		ADC_Select_CH8();
//		adc_value6 = HAL_ADC_GetValue(&hadc1);
//		counter = 4;
//		break;
//
//	case 4:
//		ADC_Select_CH10();
//		adc_value3 = HAL_ADC_GetValue(&hadc1);
//		counter = 5;
//		break;
//
//	case 5:
//		ADC_Select_CH11();
//		adc_value1 = HAL_ADC_GetValue(&hadc1);
//		counter = 0;
//		break;
//
//	default:
//		counter = 0;
//		break;
//	}
//
//	HAL_ADC_Start_IT(&hadc1);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  HAL_ADC_Start_IT(&hadc1);
  //HAL_ADC_Start_DMA(&hadc1, tablicaPomiarowDynamicznych, MAX_POMIARY);

  //HAL_TIM_Base_Start_IT(&htim2);
  //HAL_TIM_Base_Start_IT(&htim3);

  HAL_UART_Receive_IT(&huart2, &receivedData, 1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  printf("Printujeee\r\n");

	  //HAL_UART_Receive_IT(&huart2, &buffer[length], 1, HAL_MAX_DELAY);
	  /*
	  ADC_Select_CH0();HAL_ADCEx_Calibration_Start(&hadc1)
	  HAL_ADC_Start(&hadc1);
	  HAL_ADC_PollForConversion(&hadc1, 1000);
	  adc_value1=HAL_ADC_GetValue(&hadc1);
	  HAL_ADC_Stop(&hadc1);

	  ADC_Select_CH1();
	  HAL_ADC_Start(&hadc1);
	  HAL_ADC_PollForConversion(&hadc1, 1000);
	  adc_value2=HAL_ADC_GetValue(&hadc1);
	  HAL_ADC_Stop(&hadc1);

	  ADC_Select_CH4();
	  HAL_ADC_Start(&hadc1);
	  HAL_ADC_PollForConversion(&hadc1, 1000);
	  adc_value3=HAL_ADC_GetValue(&hadc1);
	  HAL_ADC_Stop(&hadc1);

	  ADC_Select_CH8();
	  HAL_ADC_Start(&hadc1);
	  HAL_ADC_PollForConversion(&hadc1, 1000);
	  adc_value4=HAL_ADC_GetValue(&hadc1);
	  HAL_ADC_Stop(&hadc1);
	  */
	  /*
	  ADC_Select_CH10();
	  HAL_ADC_Start(&hadc1);
	  HAL_ADC_PollForConversion(&hadc1, 1000);
	  adc_value6=HAL_ADC_GetValue(&hadc1);
	  HAL_ADC_Stop(&hadc1);

	  ADC_Select_CH11();
	  HAL_ADC_Start(&hadc1);
	  HAL_ADC_PollForConversion(&hadc1, 1000);
	  adc_value5=HAL_ADC_GetValue(&hadc1);
	  HAL_ADC_Stop(&hadc1);
	  */

	  //printf("Testowanie\n\r");


	  //HAL_Delay(100);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 68;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_11;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_56CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = TIM1_PERIOD;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = TIM1_PRESCALER;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = TIM2_PRESCALER;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = TIM2_PERIOD;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = TIM3_PRESCALER;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = TIM3_PERIOD;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void sendDataRamkaDynamT(uint32_t *tablicaPomiarowDynamicznych, int czasPomiaruDynam)
{
    const int maxBufSize = 3*MAX_POMIARY;  // Większy bufor, aby zmieścić całą ramkę
    char frame[maxBufSize];      // Bufor dla całej ramki
    int czasPomNa2 = czasPomiaruDynam/2;
    int index = 0;

    // Dodanie nagłówka "P D [C5 lub C6]"
    index += snprintf(frame + index, sizeof(frame) - index, "P D %s ", idCzujnika);

    // Dodanie informacji o rodzaju pomiaru: "T [czasPomiaruDynam]"
    index += snprintf(frame + index, sizeof(frame) - index, "T %d ", czasPomiaruDynam);


    int wyliczonyIdxHistorii = indeksHistorii;
    // Dodanie historii pomiaru
    for ( int licznik = 0; licznik<ROZM_HISTORIA; licznik++)
    {
    	index += snprintf(frame + index, sizeof(frame) - index, "%lu ", tablicaHistoriiPrzedPomiarem[wyliczonyIdxHistorii]);
    	wyliczonyIdxHistorii = (wyliczonyIdxHistorii + 1) % ROZM_HISTORIA;
    }
    // Dodanie danych pomiarowych

    for (int i = 0; i < czasPomNa2; i++) {
        //int pozycja = (licznikPoczatku + i) % liczbaPomiarow;
    	if(tablicaPomiarowDynamicznych[i] > 0)
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
//    licznikPoczatku = 0;  // Reset wskaźnika na początek
//    liczbaPomiarow = 0;   // Reset liczby pomiarów
    memset(tablicaPomiarowDynamicznych, 0, sizeof(int) * MAX_POMIARY);  // Zerowanie zawartości tablicy
}



uint16_t parse_crc_from_text(const char *crc_text) {
    uint16_t crc_value = 0;
    sscanf(crc_text, "%4hx", &crc_value);  // Parsowanie jako 16-bitowy hex
    return crc_value;
}

// Funkcja odbierająca ramkę danych
void receive_data_frame(uint16_t length) {
    // Sprawdzamy, czy ramka jest wystarczająco długa oraz czy zaczyna się od 'U'
    if (length < 9 || buffer[0] != 'U') {  // Zakładamy minimalną długość: U + dane + 4 znaki CRC + \r\n
        printf("Błąd: Niepoprawna ramka lub brak nagłówka 'U'\n");
        return;
    }

    // Oddzielamy dane od CRC (ostatnie 4 znaki przed '\r\n' to tekstowe CRC)
    char crc_text[5] = {0};
    memcpy(crc_text, &buffer[length - 6], 4);  // Kopiujemy 4 znaki tekstowego CRC (np. "a9f9")

    // Parsujemy tekstowe CRC na wartość liczbową
    uint16_t crc_received = parse_crc_from_text(crc_text);
    //printf("Mam nowe crc_received: %d\r\n", crc_received);

    // Obliczamy CRC dla odebranych danych (bez 'U' i bez CRC)
    uint16_t crc_calculated = crc16_ccitt_false((const char*)buffer, length - 6);
    //printf("A crc calculated t o : %d\r\n", crc_calculated);

    // Sprawdzamy poprawność CRC
    if (crc_calculated == crc_received) {
        // Przekazujemy dane bez CRC i nagłówka 'U' do przetworzenia
        process_received_data(buffer + 2, length - 7);  // -7 to U + CRC (4 znaki) + \r\n
    } else {
        printf("Błąd CRC! Odebrano: %u, Obliczono: %u\n", crc_received, crc_calculated);
    }
}


void process_received_data(uint8_t *data, uint16_t length) {
    char command = data[0];  // 'command' jest teraz w pierwszym znaku danych
    idPomiaru = command;


    if ( !(command == 'M') )
    {

    strncpy(idCzujnika, (char*)&data[2], 2);  // Kopiowanie dwóch znaków od indeksu 2
    idCzujnika[2] = '\0';

    } else {

    	idCzujnika[0] = '0';
    	idCzujnika[1] = '0';
    	idCzujnika[2] = '\0';
    }
    // Wyszukiwanie wartości po "P "
        char *ptrP = strstr((char *)data, "P ");
        if (ptrP != NULL) {
            ptrP += 2;  // Przesunięcie wskaźnika za "P "
            char *endPtr = strchr(ptrP, ' ');  // Znalezienie końca wartości (pierwszej spacji)

            if (endPtr == NULL) {  // Jeśli brak spacji, zakładamy koniec danych
                endPtr = (char *)data + length;
            }

            // Obliczanie długości wartości
            size_t len = endPtr - ptrP;
            if (len < 6) {  // Sprawdzamy, czy długość jest poprawna
                char progStr[6] = {0};  // Bufor na wartość progu
                strncpy(progStr, ptrP, len);
                progStr[len] = '\0';  // Zakończenie ciągu znaków
                progPomiaruDynam = (uint16_t)strtoul(progStr, NULL, 10);  // Konwersja na uint16_t
                printf("Prog pomiaru dynam: %u\r\n", progPomiaruDynam);
            } else {
                printf("Błąd: Długość wartości progu przekracza limit!\r\n");
            }
        } else {
            printf("Błąd: Nie znaleziono ciągu 'P '.\r\n");
        }

    printf("IdCzujnika: %s\r\n", idCzujnika);
    printf("Command: %c\r\n", command);

    if (command == 'S' || command == 'M') {

    	HAL_ADC_Stop_DMA(&hadc1);
        HAL_TIM_Base_Start_IT(&htim2);
        HAL_TIM_Base_Stop_IT(&htim3);

    } else if (command == 'D') {

//    	HAL_ADC_STOP(&hadc1);
    	HAL_ADC_Stop(&hadc1);

    	if (strncmp((char*)data + 5, "T", 1) == 0) {  // Komenda "T" zaczyna się od indeksu 5
            strncpy(idPomiaruDynam, "T", 2);
            idPomiaruDynam[2] = '\0';  // Dodanie końca stringa

            // Parsowanie parametru jako uint16_t
            char param_str[6] = {0};
            strncpy(param_str, (char*)data + 7, 5);  // Parametr zaczyna się od indeksu 7
            czasPomiaruDynam = (uint16_t)strtoul(param_str, NULL, 10);  // Konwersja na uint16_t
            //printf("Czas pomiaru (uint16_t): %u\r\n", czasPomiaruDynam);

        } else if (strncmp((char*)data + 5, "X", 1) == 0) {  // Komenda "X" zaczyna się od indeksu 5
            strncpy(idPomiaruDynam, "X", 2);
            idPomiaruDynam[2] = '\0';  // Dodanie końca stringa
        }
    	if ( idCzujnika[1] == '5' )
    	{
    		ADC_Select_CH11();

    	} else if( idCzujnika[1] == '6' ) {

    		ADC_Select_CH10();
    	}
    	flagaZmiany = 0;
        HAL_TIM_Base_Stop_IT(&htim2);
        HAL_TIM_Base_Start_IT(&htim3);
    }
}

void HAL_Delay_us(uint16_t us) {
    uint32_t start = __HAL_TIM_GET_COUNTER(&htim1); // Załóżmy, że htim1 jest skonfigurowany na 1 MHz (1 µs na tick)
    while ((__HAL_TIM_GET_COUNTER(&htim1) - start) < us);
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc) {
    printf("ADC DMA Error\n");
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
