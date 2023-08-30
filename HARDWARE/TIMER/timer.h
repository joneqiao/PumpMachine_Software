#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
/************************************************
 IRcontroller-HAL库函数版
 技术支持：www.logic-elec.com
 南京洛吉克电子科技有限公司  
 作者：jone @LOGIC-ELEC
************************************************/ 							  
////////////////////////////////////////////////////////////////////////////////// 	
extern u8  TIM4CH4_CAPTURE_STA;		//输入捕获状态		    				
extern u32	TIM4CH4_CAPTURE_VAL;	//输入捕获值 

extern TIM_HandleTypeDef TIM4_Handler;    	//定时器4句柄 
extern TIM_HandleTypeDef TIM3_Handler;      //定时器句柄 
void TIM2_Init(u16 arr,u16 psc);
void TIM3_Init(u16 arr,u16 psc);
void TIM5_Init(u16 arr,u16 psc);
void TIM4_CH4_Cap_Init(u32 arr,u16 psc);
void FeetSwitch(void);

#endif

