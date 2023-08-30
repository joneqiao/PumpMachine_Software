#ifndef __IWDG_H
#define __IWDG_H
#include "sys.h"

/************************************************
 IRcontroller-HAL库函数版
 技术支持：www.logic-elec.com
 南京洛吉克电子科技有限公司  
 作者：jone @LOGIC-ELEC
************************************************/ 
////////////////////////////////////////////////////////////////////////////////// 	

void IWDG_Init(u8 prer,u16 rlr);//初始化IWDG，并使能IWDG
void IWDG_Feed(void);//喂狗
#endif
