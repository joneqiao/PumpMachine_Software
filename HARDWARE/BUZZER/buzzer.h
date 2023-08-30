#ifndef _BUZZER_H
#define _BUZZER_H
#include "sys.h"

#define BUZZER PCout(5)   	//BUZZER
#define BUZZER_MODE_LOW      1
#define BUZZER_MODE_MIDDLE   2

void BUZZER_Init(void);
void Buzzer_Mode(u8 mode);
void EnableBuzzer(void); 
void DisableBuzzer(void); 

#endif


