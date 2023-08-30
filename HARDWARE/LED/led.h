#ifndef _LED_H
#define _LED_H
#include "sys.h"
/************************************************
 IRcontroller-HAL库函数版
 技术支持：www.logic-elec.com
 南京洛吉克电子科技有限公司  
 作者：jone @LOGIC-ELEC
************************************************/ 

#define LED0 PAout(8)   	//LED0
#define LED1 PDout(2)   	//LED1

#define LED_RED       PCout(8)
#define LED_POWER     PCout(10)

void LED_Init(void);

#endif
