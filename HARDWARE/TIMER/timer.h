#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
/************************************************
 IRcontroller-HAL�⺯����
 ����֧�֣�www.logic-elec.com
 �Ͼ��弪�˵��ӿƼ����޹�˾  
 ���ߣ�jone @LOGIC-ELEC
************************************************/ 							  
////////////////////////////////////////////////////////////////////////////////// 	
extern u8  TIM4CH4_CAPTURE_STA;		//���벶��״̬		    				
extern u32	TIM4CH4_CAPTURE_VAL;	//���벶��ֵ 

extern TIM_HandleTypeDef TIM4_Handler;    	//��ʱ��4��� 
extern TIM_HandleTypeDef TIM3_Handler;      //��ʱ����� 
void TIM2_Init(u16 arr,u16 psc);
void TIM3_Init(u16 arr,u16 psc);
void TIM5_Init(u16 arr,u16 psc);
void TIM4_CH4_Cap_Init(u32 arr,u16 psc);
void FeetSwitch(void);

#endif

