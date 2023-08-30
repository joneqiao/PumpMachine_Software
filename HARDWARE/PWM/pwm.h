#ifndef _TIMERPWM_H
#define _TIMERPWM_H
#include "sys.h"

#define CTRL_PEND     PBout(3)
#define CTRL_ALM      HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4)    //PB4->电机报警
#define CTRL_ENA      PBout(5)
#define CTRL_DIR      PBout(6)

extern TIM_HandleTypeDef TIM4_Handler;      //定时器句柄 

void MotorInit(void);
void TIM4_PWM_Init(u16 arr,u16 psc);
void EnableMotorPWM(void);
void DisableMotorPWM(void);
void TIM_SetTIM4Compare2(u32 compare);
void MotorSpeedUp(u16 motorarr);//  motorarr >= 2500  DM422 max300 khz
void Motor0_1lmin(void);//0.1l/min
void MotorSpeedDownlmin(void);//0.1l/min
u16 Motor_PscCount(u16 motogear);//relationship between flow set value and motor speed 
void MotorAlarm(void);
#endif

