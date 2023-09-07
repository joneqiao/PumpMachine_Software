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
 IRcontroller-HAL库函数版
 技术支持：www.logic-elec.com
 南京洛吉克电子科技有限公司  
 作者：jone @LOGIC-ELEC
************************************************/

#define KEY_XIYIN        HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_15)   //PA15 ->吸引
#define KEY_CHONGXI      HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_12)   //PC12->冲洗
#define KEY_JIAOTA       HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9)    //PB9->脚踏

extern u8 JiaoTaFlag;

void KEY_Init(void);
void ExternalButtonSwitch(void);
#endif

