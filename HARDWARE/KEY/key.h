#ifndef _KEY_H
#define _KEY_H
#include "sys.h"

/************************************************
 IRcontroller-HAL库函数版
 技术支持：www.logic-elec.com
 南京洛吉克电子科技有限公司  
 作者：jone @LOGIC-ELEC
************************************************/

//下面的方式是通过直接操作HAL库函数方式读取IO
#define KEY_XIYIN        HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_15)   //PA15 ->吸引
#define KEY_CHONGXI      HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_12)   //PC12->冲洗
#define KEY_JIAOTA       HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9)    //PB9->脚踏

extern u8 JiaoTaFlag;

void KEY_Init(void);

#endif

