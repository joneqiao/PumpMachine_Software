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
 IRcontroller-HAL�⺯����
 ����֧�֣�www.logic-elec.com
 �Ͼ��弪�˵��ӿƼ����޹�˾  
 ���ߣ�jone @LOGIC-ELEC
************************************************/ 		
//u8 xiyinflag;
//u8 bujinflag;
//�ⲿ�жϳ�ʼ��
void EXTI_Init(void)
{
    // GPIO_InitTypeDef GPIO_Initure;
    
    // __HAL_RCC_GPIOA_CLK_ENABLE();               //����GPIOAʱ��
    // __HAL_RCC_GPIOC_CLK_ENABLE();               //����GPIOCʱ��
  
    
    // GPIO_Initure.Pin=GPIO_PIN_15;                //PA15->����
    // GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     //�½��ش���
    // GPIO_Initure.Pull=GPIO_PULLUP;
    // HAL_GPIO_Init(GPIOA,&GPIO_Initure);

    // GPIO_Initure.Pin=GPIO_PIN_12; 				//PC12->��ϴ 
    // HAL_GPIO_Init(GPIOC,&GPIO_Initure);
    
    // //�ж���15-PA15
    // HAL_NVIC_SetPriority(EXTI15_10_IRQn,2,0);       //��ռ���ȼ�Ϊ2�������ȼ�Ϊ2
    // HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);             //ʹ���ж���
}

//�жϷ�����

// void EXTI15_10_IRQHandler(void)
// {
//     HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);		//�����жϴ����ú���
//     HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);		//�����жϴ����ú���
// }


//�жϷ����������Ҫ��������
//��HAL�������е��ⲿ�жϷ�����������ô˺���
//GPIO_Pin:�ж����ź�
// void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
// {
//     delay_ms(100);      	//����
//     switch(GPIO_Pin)
//     {
//         case GPIO_PIN_15://PA15->����
//             if(KEY_XIYIN==0) 
//             {
//                 xiyinflag = SET;
//             }
//             break;
//         case GPIO_PIN_12://PC12->��ϴ  key1
//             if(KEY_CHONGXI==0)  
//             {
//                 bujinflag = SET;
//             }
//             break;
//     }
// }
