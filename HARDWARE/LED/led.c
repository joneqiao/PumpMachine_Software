#include "led.h"
/************************************************
 IRcontroller-HAL�⺯����
 ����֧�֣�www.logic-elec.com
 �Ͼ��弪�˵��ӿƼ����޹�˾  
 ���ߣ�jone @LOGIC-ELEC
************************************************/ 
 
//LED IO��ʼ��
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOC_CLK_ENABLE();           	//����GPIOCʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_8|GPIO_PIN_10; 	//red-��pc8   led1-��pc10
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  	//�������
    GPIO_Initure.Pull=GPIO_PULLUP;          	//����
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;    //����
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);

    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET);	//pc8��1��Ĭ�ϳ�ʼ�������
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_SET);	//pc10��1��Ĭ�ϳ�ʼ�������
	LED_POWER = 0;
	
	//	    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOA_CLK_ENABLE();           	//����GPIOAʱ��
	__HAL_RCC_GPIOD_CLK_ENABLE();           	//����GPIODʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_8; 				//PB5
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  	//�������
    GPIO_Initure.Pull=GPIO_PULLUP;          	//����
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;    	 	//����
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);

	GPIO_Initure.Pin=GPIO_PIN_2; 				//PE5
	HAL_GPIO_Init(GPIOD,&GPIO_Initure);
	
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET);	//PA8��1��Ĭ�ϳ�ʼ�������
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);	//PD2��1��Ĭ�ϳ�ʼ�������
}

