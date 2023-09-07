/*
 * @fileName: 
 * @Author: Jone
 * @LastEditors: Jone
 * @description: page description
 * @Date: 2023-09-05 15:59:09
 * @LastEditTime: 2023-09-06 13:43:23
 */
#include "exti.h"
#include "delay.h"
#include "key.h"
#include "led.h"
/************************************************
 IRcontroller-HAL库函数版
 技术支持：www.logic-elec.com
 南京洛吉克电子科技有限公司  
 作者：jone @LOGIC-ELEC
************************************************/ 		
//u8 xiyinflag;
//u8 bujinflag;
//外部中断初始化
void EXTI_Init(void)
{
    // GPIO_InitTypeDef GPIO_Initure;
    
    // __HAL_RCC_GPIOA_CLK_ENABLE();               //开启GPIOA时钟
    // __HAL_RCC_GPIOC_CLK_ENABLE();               //开启GPIOC时钟
  
    
    // GPIO_Initure.Pin=GPIO_PIN_15;                //PA15->吸引
    // GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     //下降沿触发
    // GPIO_Initure.Pull=GPIO_PULLUP;
    // HAL_GPIO_Init(GPIOA,&GPIO_Initure);

    // GPIO_Initure.Pin=GPIO_PIN_12; 				//PC12->冲洗 
    // HAL_GPIO_Init(GPIOC,&GPIO_Initure);
    
    // //中断线15-PA15
    // HAL_NVIC_SetPriority(EXTI15_10_IRQn,2,0);       //抢占优先级为2，子优先级为2
    // HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);             //使能中断线
}

//中断服务函数

// void EXTI15_10_IRQHandler(void)
// {
//     HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);		//调用中断处理公用函数
//     HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);		//调用中断处理公用函数
// }


//中断服务程序中需要做的事情
//在HAL库中所有的外部中断服务函数都会调用此函数
//GPIO_Pin:中断引脚号
// void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
// {
//     delay_ms(100);      	//消抖
//     switch(GPIO_Pin)
//     {
//         case GPIO_PIN_15://PA15->吸引
//             if(KEY_XIYIN==0) 
//             {
//                 xiyinflag = SET;
//             }
//             break;
//         case GPIO_PIN_12://PC12->冲洗  key1
//             if(KEY_CHONGXI==0)  
//             {
//                 bujinflag = SET;
//             }
//             break;
//     }
// }
