#include "cmos.h"
/************************************************
 IRcontroller-HAL�⺯����
 ����֧�֣�www.logic-elec.com
 �Ͼ��弪�˵��ӿƼ����޹�˾  
 ���ߣ�jone @LOGIC-ELEC
************************************************/ 
 
//MOS IO��ʼ��
void MOS_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOC_CLK_ENABLE();           	//����GPIOCʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_6; 	//ֱ�������->pc6
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  	//�������
    GPIO_Initure.Pull=GPIO_PULLUP;          	//����
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;    //����
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);

    ATTRACT_CMOS=RESET;//�رձ�
}

