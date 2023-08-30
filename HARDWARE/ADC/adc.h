#ifndef __ADC_H
#define __ADC_H
#include "sys.h"

void ADC_Init(void); 				//ADC channel init
u16 Get_Adc(u32 ch); 
u16 Get_Adc_Average(u32 ch,u8 times);
u16 Get_R_Value(u32 ch);
float NegativePrussure(void);
float ChuliSensorPrussure(void);
float FlowSensor(void);
#endif 
