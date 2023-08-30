#include "pwm.h"
#include "led.h"
#include "parameters.h"
#include "FreeRTOS.h"
#include "task.h"

extern TIM_HandleTypeDef 	TIM4_Handler;      	//��ʱ����� 
TIM_OC_InitTypeDef 	TIM4_CH2Handler;	//��ʱ��4ͨ��2���

volatile u16  motorpscstart = MOTORINITPSC;
extern const u8 speeduptime;


extern const u16 FLOW_01L_SPEED_CW;
extern const u16 FLOW_01L_SPEED_DOWN;
extern const u16 FLOW_01L_SPEED;
extern const u16 FLOW_02L_SPEED;
extern const u16 FLOW_03L_SPEED;
extern const u16 FLOW_04L_SPEED;
extern const u16 FLOW_05L_SPEED;
extern const u16 FLOW_06L_SPEED;
extern const u16 FLOW_07L_SPEED;
extern const u16 FLOW_08L_SPEED;
extern const u16 FLOW_9L_SPEED;
extern const u16 FLOW_10L_SPEED;
extern const u16 FLOW_11L_SPEED;
extern const u16 FLOW_12L_SPEED;
extern const u16 FLOW_13L_SPEED;

extern u8 bjiconstatue;
extern u8 bujinflag;

extern vs16 para2flash[parameternum];



void MotorInit()
{
    GPIO_InitTypeDef GPIO_InitureOutput;
	GPIO_InitTypeDef GPIO_InitureInput;
    __HAL_RCC_GPIOB_CLK_ENABLE();           	//����GPIOBʱ��
	
    GPIO_InitureOutput.Pin=GPIO_PIN_3|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7; 	
    GPIO_InitureOutput.Mode=GPIO_MODE_OUTPUT_PP;  	//�������
    GPIO_InitureOutput.Pull=GPIO_PULLUP;          	//����
    GPIO_InitureOutput.Speed=GPIO_SPEED_FREQ_HIGH;    //����
    HAL_GPIO_Init(GPIOB,&GPIO_InitureOutput);
	
	GPIO_InitureInput.Pin=GPIO_PIN_4; 	
    GPIO_InitureInput.Mode=GPIO_MODE_INPUT;  	//�������
    GPIO_InitureInput.Pull=GPIO_PULLUP;          	//����
    GPIO_InitureInput.Speed=GPIO_SPEED_FREQ_HIGH;    //����
    HAL_GPIO_Init(GPIOB,&GPIO_InitureInput);

	
    CTRL_DIR=1;//��ʱ��
    CTRL_ENA=0;
//    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET);	//pc6��1��Ĭ�ϳ�ʼ�������
}


    
//TIM1 PWM���ֳ�ʼ�� 
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
void TIM4_PWM_Init(u16 arr,u16 psc)
{  
    TIM4_Handler.Instance=TIM4;         	//��ʱ��4
    TIM4_Handler.Init.Prescaler=psc;       	//��ʱ����Ƶ
    TIM4_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;//���ϼ���ģʽ
    TIM4_Handler.Init.Period=arr;          	//�Զ���װ��ֵ
    TIM4_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&TIM4_Handler);       	//��ʼ��PWM

    TIM4_CH2Handler.OCMode=TIM_OCMODE_PWM1; //ģʽѡ��PWM1
    TIM4_CH2Handler.Pulse=arr/2;            //���ñȽ�ֵ,��ֵ����ȷ��ռ�ձȣ�Ĭ�ϱȽ�ֵΪ�Զ���װ��ֵ��һ��,��ռ�ձ�Ϊ50%
    TIM4_CH2Handler.OCPolarity=TIM_OCPOLARITY_HIGH; //����Ƚϼ���Ϊ�� 
    HAL_TIM_PWM_ConfigChannel(&TIM4_Handler,&TIM4_CH2Handler,TIM_CHANNEL_2);//����TIM4ͨ��2
    
    HAL_TIM_Base_Start_IT(&TIM4_Handler); //ʹ�ܶ�ʱ��4�Ͷ�ʱ��4�����жϣ�TIM_IT_UPDATE  
	HAL_NVIC_SetPriority(TIM4_IRQn,1,0);    //�����ж����ȼ�����ռ���ȼ�2�������ȼ�0
    HAL_NVIC_EnableIRQ(TIM4_IRQn);          //����ITM4�ж�ͨ��  


}

//��ʱ���ײ�������ʱ��ʹ�ܣ���������
//�˺����ᱻHAL_TIM_PWM_Init()����
//htim:��ʱ�����
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
	GPIO_InitTypeDef GPIO_Initure;
	
    if(htim->Instance==TIM4)
	{
		__HAL_RCC_TIM4_CLK_ENABLE();			//ʹ�ܶ�ʱ��4
		__HAL_RCC_GPIOB_CLK_ENABLE();			//����GPIOBʱ��
		
		GPIO_Initure.Pin=GPIO_PIN_7;           	//PB7
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;  	//�����������
		GPIO_Initure.Pull=GPIO_PULLUP;          //����
		GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//����
		HAL_GPIO_Init(GPIOB,&GPIO_Initure); 	
	}
}

//����TIM4ͨ��2��ռ�ձ�
//compare:�Ƚ�ֵ
void TIM_SetTIM4Compare2(u32 compare)
{
	TIM4->CCR2=compare; 
}

void EnableMotorPWM()
{
    HAL_TIM_PWM_Start(&TIM4_Handler,TIM_CHANNEL_2);//turn on PWM channel 2 
}

void DisableMotorPWM()
{
    HAL_TIM_PWM_Stop(&TIM4_Handler,TIM_CHANNEL_2); //turn off PWM channel 2
}

u32 speedtimecount;
u16 timecount;
u16 linetype;
void MotorSpeedUp(u16 motorarrup)//  motorarr >= 2500  DM422 max300 khz
{
	HAL_NVIC_EnableIRQ(TIM4_IRQn);          //����ITM4�ж�ͨ��
	EnableMotorPWM(); 	
	CTRL_DIR=1;
    while((u16)chuli_current_val <= (pressuresetvalue - 15) && ((bjiconstatue&0x01) == 1))
    {
        XiYinControl();//�ڼ��ٹ�����Ҳ�ܶ������ý��п���
        delay_ms(15);
        speedtimecount++;
        if(speedtimecount%2==0)
            motorpscstart--;
		timecount++;
		if(  motorpscstart < Motor_PscCount( motogear )) //��֤�����˳�ʱ���ٶȲ����ڵ�ǰ�趨��λ���ٶ�
		{
			motorpscstart = Motor_PscCount( motogear );
		}
    }
	
	if(  motorpscstart < Motor_PscCount( motogear )) 
	{
		motorpscstart = Motor_PscCount( motogear );
	}
		
//	if( timecount < 600 )
//	{
//		linetype = 0;//xi
//	}
//	else
//	{
//		linetype = 1;//cu 
//	}
//		
//	if(linetype == 0)
//	{		
//		motorpscstart += 150;
//		mypid.Proportion = 8;//1.4625;
//	}
//	else
//	{
		 mypid.Proportion = 11;//1.4625;
//	}
	timecount = 0;
	
//    HAL_NVIC_DisableIRQ(TIM4_IRQn);          //�ر�ITM4�ж�ͨ��      
}

void Motor0_1lmin()//0.1l/min
{
    motorpscstart = FLOW_01L_SPEED_CW;//reloading value    Y Hz = 1000000/motorpscstart 
} 

void MotorSpeedDownlmin()//0.1l/min
{
    motorpscstart = FLOW_01L_SPEED_DOWN;//reloading value    Y Hz = 1000000/motorpscstart 
}    

u16 Motor_PscCount(u16 pscmotogear)
{
    u16 motorpsc;
    switch( pscmotogear )
    {
        case 1:motorpsc=FLOW_01L_SPEED;break;
        case 2:motorpsc=FLOW_02L_SPEED;break;
        case 3:motorpsc=FLOW_03L_SPEED;break;
        case 4:motorpsc=FLOW_04L_SPEED;break;
        case 5:motorpsc=FLOW_05L_SPEED;break;
        case 6:motorpsc=FLOW_06L_SPEED;break;
        case 7:motorpsc=FLOW_07L_SPEED;break;
        case 8:motorpsc=FLOW_08L_SPEED;break;
        case 9:motorpsc=FLOW_9L_SPEED;break;
        case 10:motorpsc=FLOW_10L_SPEED;break;
        case 11:motorpsc=FLOW_11L_SPEED;break;
        case 12:motorpsc=FLOW_12L_SPEED;break;
        case 13:motorpsc=FLOW_13L_SPEED;break;
        default:motorpsc=FLOW_01L_SPEED;break;        
    }
    return motorpsc;
}
        
void MotorAlarm()
{
	static u8 motorAlarmflag = 0; 
	
    if( CTRL_ALM == 1 )
    {
        delay_ms(10);
        if( CTRL_ALM == 1 )
        {
			if(motorAlarmflag == RESET)
			{
				motorAlarmflag = SET;
				DwinBitIconDisplay(MOTORALARMADDR,19);//��ʾ�����������
			}        
        }
    }       
    else
    {
        if(motorAlarmflag == SET)
		{
            motorAlarmflag = RESET; 
			DwinBitIconDisplay(MOTORALARMADDR,0);//��������������       
        }
    }
}
