#ifndef INC_ADC_H_
#define INC_ADC_H_

#include "main.h"
#include "przetworzDane.h"
#include "komunikacjaUART.h"

typedef struct
{
	volatile int wartosc1;
	volatile int wartosc2;
	volatile int wartosc3;
	volatile int wartosc4;
	volatile int wartosc5;
	volatile int wartosc6;

}WartosciADC;

extern WartosciADC wartosciADC;

void ADC_Select_CH0 (void);
void ADC_Select_CH1 (void);
void ADC_Select_CH4 (void);
void ADC_Select_CH8 (void);
void ADC_Select_CH10 (void);
void ADC_Select_CH11 (void);

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc);

#endif /* INC_ADC_H_ */
