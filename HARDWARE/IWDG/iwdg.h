#ifndef __IWDG_H
#define __IWDG_H
#include "sys.h"

/************************************************
 IRcontroller-HAL�⺯����
 ����֧�֣�www.logic-elec.com
 �Ͼ��弪�˵��ӿƼ����޹�˾  
 ���ߣ�jone @LOGIC-ELEC
************************************************/ 
////////////////////////////////////////////////////////////////////////////////// 	

void IWDG_Init(u8 prer,u16 rlr);//��ʼ��IWDG����ʹ��IWDG
void IWDG_Feed(void);//ι��
#endif
