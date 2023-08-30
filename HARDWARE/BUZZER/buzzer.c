#include "buzzer.h"
#include "timer.h"
u8 buzzermode; 
//BUZZER IO��ʼ��
void BUZZER_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOC_CLK_ENABLE();           	//����GPIOCʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_5; 				//PC5
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  	//�������
    GPIO_Initure.Pull=GPIO_PULLUP;          	//����
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;    //����
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);

    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_5,GPIO_PIN_RESET);	//Pc5��1��Ĭ�ϳ�ʼ������������У��ߵ�ƽbuzzer��
    
    TIM5_Init(50-1,7200-1);//��ʱ��5��ʼ������ʱ��ʱ��Ϊ72M����Ƶϵ��Ϊ7200-1�����Զ�ʱ��3��Ƶ��Ϊ72M/7200=10K���Զ���װ��Ϊ50-1����ô��ʱ�����ھ���5ms      
}


//��ڲ�����  BUZZER_MODE_LOW
//           BUZZER_MODE_MIDDLE
//   _____       _____
//__| 175 |_175_| 175 |____________3000_______________ �ͼ���ms

//   _____       _____       _____
//__| 175 |_175_| 175 |_175_| 175 |____2000___________ �߼���ms

void Buzzer_Mode(u8 mode)
{
    if( mode == BUZZER_MODE_LOW )
    {
        buzzermode = BUZZER_MODE_LOW;        
    }
    else if( mode == BUZZER_MODE_MIDDLE )
    {
        buzzermode = BUZZER_MODE_MIDDLE;
    }  
    EnableBuzzer();
}

void EnableBuzzer()
{
    HAL_NVIC_EnableIRQ(TIM5_IRQn);          //����ITM5�ж� 
}
void DisableBuzzer()
{
    HAL_NVIC_DisableIRQ(TIM5_IRQn);         //�ر�ITM5�ж� 
    BUZZER =0;                              //�����ر�buzzer
}
