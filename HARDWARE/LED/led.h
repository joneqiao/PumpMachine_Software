#ifndef _LED_H
#define _LED_H
#include "sys.h"
/************************************************
 IRcontroller-HAL�⺯����
 ����֧�֣�www.logic-elec.com
 �Ͼ��弪�˵��ӿƼ����޹�˾  
 ���ߣ�jone @LOGIC-ELEC
************************************************/ 

#define LED0 PAout(8)   	//LED0
#define LED1 PDout(2)   	//LED1

#define LED_RED       PCout(8)
#define LED_POWER     PCout(10)

void LED_Init(void);

#endif
