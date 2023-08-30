#include "timer.h"
#include "buzzer.h"
#include "led.h"
#include "usart.h"
#include "cmos.h"
#include "dwindisplay.h"
#include "delay.h"
#include "key.h"
#include "control.h"

extern volatile u16 motorpscstart;
/************************************************
 IRcontroller-HAL�⺯����
 ����֧�֣�www.logic-elec.com
 �Ͼ��弪�˵��ӿƼ����޹�˾  
 ���ߣ�jone @LOGIC-ELEC
************************************************/ 									  
////////////////////////////////////////////////////////////////////////////////// 

//����״̬
//[7]:0,û�гɹ��Ĳ���;1,�ɹ�����һ��.
//[6]:0,��û���񵽵͵�ƽ;1,�Ѿ����񵽵͵�ƽ��.
//[5:0]:����͵�ƽ������Ĵ���
u8  TIM4CH4_CAPTURE_STA=0;							//���벶��״̬		    				
u32	TIM4CH4_CAPTURE_VAL;							//���벶��ֵ(TIM4��16λ)

u8 mototimeconutflag=0;
u8 mototimeconut=0;
extern u8 xyiconstatue;
extern u8 bjiconstatue;
extern u8 buzzermode;
extern u8 xiyinonceflag;


u8 getpressureflag =1;
u8 getflowflag     =1;

extern IWDG_HandleTypeDef IWDG_Handler; //�������Ź����
TIM_HandleTypeDef TIM2_Handler;         //��ʱ��2��� 
TIM_HandleTypeDef TIM4_Handler;         //��ʱ��4���
TIM_HandleTypeDef TIM3_Handler;         //��ʱ��3��� 
TIM_HandleTypeDef TIM5_Handler;         //��ʱ��5��� 
//ͨ�ö�ʱ��3�жϳ�ʼ��
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��3!
void TIM2_Init(u16 arr,u16 psc)
{  
    TIM2_Handler.Instance=TIM2;                          //ͨ�ö�ʱ��2
    TIM2_Handler.Init.Prescaler=psc;                     //��Ƶϵ��
    TIM2_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    TIM2_Handler.Init.Period=arr;                        //�Զ�װ��ֵ
    TIM2_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//ʱ�ӷ�Ƶ����
    HAL_TIM_Base_Init(&TIM2_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM2_Handler); //ʹ�ܶ�ʱ��3�Ͷ�ʱ��3�����жϣ�TIM_IT_UPDATE   
}

void TIM3_Init(u16 arr,u16 psc)
{  
    TIM3_Handler.Instance=TIM3;                          //ͨ�ö�ʱ��3
    TIM3_Handler.Init.Prescaler=psc;                     //��Ƶϵ��
    TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    TIM3_Handler.Init.Period=arr;                        //�Զ�װ��ֵ
    TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//ʱ�ӷ�Ƶ����
    HAL_TIM_Base_Init(&TIM3_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM3_Handler); //ʹ�ܶ�ʱ��3�Ͷ�ʱ��3�����жϣ�TIM_IT_UPDATE   
}

//��ʱ��4ͨ��4���벶������
//arr���Զ���װֵ(TIM4��16λ��!!)
//psc��ʱ��Ԥ��Ƶ��
void TIM4_CH4_Cap_Init(u32 arr,u16 psc)
{  
    TIM_IC_InitTypeDef TIM4_CH4Config;  
    
    TIM4_Handler.Instance=TIM4;                          //ͨ�ö�ʱ��4
    TIM4_Handler.Init.Prescaler=psc;                     //��Ƶϵ��
    TIM4_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    TIM4_Handler.Init.Period=arr;                        //�Զ�װ��ֵ
    TIM4_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//ʱ�ӷ�Ƶ����
    HAL_TIM_IC_Init(&TIM4_Handler);						//��ʼ�����벶��ʱ������
    
    TIM4_CH4Config.ICPolarity=TIM_ICPOLARITY_FALLING;    //�½��ز���
    TIM4_CH4Config.ICSelection=TIM_ICSELECTION_DIRECTTI;//ӳ�䵽TI1��
    TIM4_CH4Config.ICPrescaler=TIM_ICPSC_DIV1;          //���������Ƶ������Ƶ
    TIM4_CH4Config.ICFilter=0;                          //���������˲��������˲�
    HAL_TIM_IC_ConfigChannel(&TIM4_Handler,&TIM4_CH4Config,TIM_CHANNEL_4);//����TIM4ͨ��4
	
    HAL_TIM_IC_Start_IT(&TIM4_Handler,TIM_CHANNEL_4);   //����TIM4�Ĳ���ͨ��4�����ҿ��������ж�
    HAL_TIM_Base_Start_IT(&TIM4_Handler); //ʹ�ܶ�ʱ��4�Ͷ�ʱ��4�����жϣ�TIM_IT_UPDATE  
	HAL_NVIC_SetPriority(TIM4_IRQn,3,0);    //�����ж����ȼ�����ռ���ȼ�2�������ȼ�0
    HAL_NVIC_EnableIRQ(TIM4_IRQn);          //����ITM4�ж�ͨ��  
}

//��ʱ��4�ײ�������ʱ��ʹ�ܣ���������
//�˺����ᱻHAL_TIM_IC_Init()����
//htim:��ʱ��4���
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_TIM4_CLK_ENABLE();            //ʹ��TIM4ʱ��
    __HAL_RCC_GPIOB_CLK_ENABLE();			//����GPIOBʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_9;            //PB9
    GPIO_Initure.Mode=GPIO_MODE_AF_INPUT; 	//������������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//����
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);

    HAL_NVIC_SetPriority(TIM4_IRQn,4,0);    //�����ж����ȼ�����ռ���ȼ�2�������ȼ�0
    HAL_NVIC_EnableIRQ(TIM4_IRQn);          //����ITM4�ж�ͨ��  
   
}

void TIM5_Init(u16 arr,u16 psc)
{  
    TIM5_Handler.Instance=TIM5;                          //ͨ�ö�ʱ��5
    TIM5_Handler.Init.Prescaler=psc;                     //��Ƶϵ��
    TIM5_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    TIM5_Handler.Init.Period=arr;                        //�Զ�װ��ֵ
    TIM5_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//ʱ�ӷ�Ƶ����
    HAL_TIM_Base_Init(&TIM5_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM5_Handler); //ʹ�ܶ�ʱ��3�Ͷ�ʱ��3�����жϣ�TIM_IT_UPDATE   
}

//��ʱ���ײ�����������ʱ�ӣ������ж����ȼ�
//�˺����ᱻHAL_TIM_Base_Init()��������
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM2)
	{
		__HAL_RCC_TIM2_CLK_ENABLE();            //ʹ��TIM2ʱ��
		HAL_NVIC_SetPriority(TIM3_IRQn,4,0);    //�����ж����ȼ�����ռ���ȼ�1�������ȼ�3
		HAL_NVIC_EnableIRQ(TIM2_IRQn);          //����ITM2�ж�   
	}
    if(htim->Instance==TIM3)
	{
		__HAL_RCC_TIM3_CLK_ENABLE();            //ʹ��TIM3ʱ��
		HAL_NVIC_SetPriority(TIM3_IRQn,5,0);    //�����ж����ȼ�����ռ���ȼ�1�������ȼ�3
		HAL_NVIC_EnableIRQ(TIM3_IRQn);          //����ITM3�ж�   
	}
        if(htim->Instance==TIM5)
	{
		__HAL_RCC_TIM5_CLK_ENABLE();            //ʹ��TIM5ʱ��
		HAL_NVIC_SetPriority(TIM5_IRQn,6,0);    //�����ж����ȼ�����ռ���ȼ�1�������ȼ�3
//		HAL_NVIC_EnableIRQ(TIM5_IRQn);          //����ITM5�ж�   
	}
}

//��ʱ��2�жϷ�����
void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM2_Handler);
}
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM3_Handler);
}
void TIM5_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM5_Handler);
}

//�ص���������ʱ���жϷ���������
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim==(&TIM2_Handler))
    {
        if(mototimeconutflag==1)
        {
            mototimeconut++;
            if(mototimeconut>20)
            {
                xyiconstatue=RESET;//�رձ�
            }
        }
    }
    if(htim==(&TIM3_Handler))
    {
        getpressureflag  =1;
        getflowflag      =1;
    }
    if(htim==(&TIM4_Handler))
    {
        TIM4->ARR=(u16)motorpscstart;//reloading value    Y Hz = 1000000/motorpscstart 
        TIM4->CCR2=(u16)motorpscstart/2;//low50%,high 50%   compare value = reload value / 2
    }
    if(htim==(&TIM5_Handler) && buzzermode==BUZZER_MODE_LOW)
    {             
        static u16 millisec5ms;
        millisec5ms++;
        if(0  <   millisec5ms && millisec5ms < 35)   BUZZER =1;                      //��175ms  BUZZER     �ߵ�ƽbuzzer��
        if(35 <=  millisec5ms && millisec5ms < 70)   BUZZER =0;                      //��175ms
        if(70 <=  millisec5ms && millisec5ms < 105)  BUZZER =1;                      //��175ms
        if(175<=  millisec5ms && millisec5ms <= 6000)BUZZER =0;                      //��3000ms  
        if(millisec5ms>6000)millisec5ms=0;         
    }
    if(htim==(&TIM5_Handler) && buzzermode==BUZZER_MODE_MIDDLE)
    {             
        static u16 millisec5ms;
        millisec5ms++;
        if(0  <   millisec5ms && millisec5ms < 35)   BUZZER =1;                      //��175ms  BUZZER     �ߵ�ƽbuzzer��
        if(35 <=  millisec5ms && millisec5ms < 70)   BUZZER =0;                      //��175ms
        if(70 <=  millisec5ms && millisec5ms < 105)  BUZZER =1;                      //��175ms
        if(105<= millisec5ms  && millisec5ms < 140)  BUZZER =0;                      //��175ms
        if(140<= millisec5ms  && millisec5ms < 175)  BUZZER =1;                      //��175ms
        if(175<= millisec5ms  && millisec5ms <= 4000)BUZZER =0;                      //��2000ms  
        if(millisec5ms>4000)millisec5ms=0;        
    }
}

//��ʱ��4�жϷ�����
void TIM4_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM4_Handler);				//��ʱ�����ô�����
}

//��ʱ�����벶���жϴ���ص��������ú�����HAL_TIM_IRQHandler�лᱻ����
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)//�����жϷ���ʱִ��
{
	if((TIM4CH4_CAPTURE_STA&0X80)==0)				//��δ�ɹ�����
	{
		if(TIM4CH4_CAPTURE_STA&0X40)				//����һ���½��� 		
		{	  			
			TIM4CH4_CAPTURE_STA|=0X80;				//��ǳɹ�����һ�ε͵�ƽ����
            xyiconstatue=0;//��ֹͣ����
            DwinBitIconDisplay(XYICONADDR,0);//display xiyin icon  
            mototimeconut=0;
            mototimeconutflag=0;
            TIM4CH4_CAPTURE_VAL=HAL_TIM_ReadCapturedValue(&TIM4_Handler,TIM_CHANNEL_4);//��ȡ��ǰ�Ĳ���ֵ.
			TIM_RESET_CAPTUREPOLARITY(&TIM4_Handler,TIM_CHANNEL_4);   //һ��Ҫ�����ԭ�������ã���
            TIM_SET_CAPTUREPOLARITY(&TIM4_Handler,TIM_CHANNEL_4,TIM_ICPOLARITY_FALLING);//����TIM4ͨ��4�½��ز���
		}else  										//��δ��ʼ,��һ�β����½���
		{
			TIM4CH4_CAPTURE_STA=0;					//���
			TIM4CH4_CAPTURE_VAL=0;
			TIM4CH4_CAPTURE_STA|=0X40;				//��ǲ������½���
            xyiconstatue=1;//�ÿ�ʼ���� 
            xiyinonceflag = RESET;
            DwinBitIconDisplay(XYICONADDR,1);//display xiyin icon  
            mototimeconutflag=1;//�����ü�ʱ���������20�룬��ֹͣ��
			__HAL_TIM_DISABLE(&TIM4_Handler);      	//�رն�ʱ��4
			__HAL_TIM_SET_COUNTER(&TIM4_Handler,0);
			TIM_RESET_CAPTUREPOLARITY(&TIM4_Handler,TIM_CHANNEL_4);   //һ��Ҫ�����ԭ�������ã���
			TIM_SET_CAPTUREPOLARITY(&TIM4_Handler,TIM_CHANNEL_4,TIM_ICPOLARITY_RISING);//��ʱ��4ͨ��4����Ϊ�����ز���
			__HAL_TIM_ENABLE(&TIM4_Handler);		//ʹ�ܶ�ʱ��4
		}		    
	}		
}



