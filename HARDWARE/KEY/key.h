/*
 * @fileName: 
 * @Author: Jone
 * @LastEditors: Jone
 * @description: page description
 * @Date: 2023-09-05 16:01:56
 * @LastEditTime: 2023-09-06 13:59:37
 */
#ifndef _KEY_H
#define _KEY_H
#include "sys.h"

/************************************************
 IRcontroller-HAL�⺯����
 ����֧�֣�www.logic-elec.com
 �Ͼ��弪�˵��ӿƼ����޹�˾  
 ���ߣ�jone @LOGIC-ELEC
************************************************/

#define KEY_XIYIN        HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_15)   //PA15 ->����
#define KEY_CHONGXI      HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_12)   //PC12->��ϴ
#define KEY_JIAOTA       HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9)    //PB9->��̤

extern u8 JiaoTaFlag;

void KEY_Init(void);
void ExternalButtonSwitch(void);
#endif

