#include "pwm.h"
#include "led.h"
#include "parameters.h"
#include "FreeRTOS.h"
#include "task.h"

extern TIM_HandleTypeDef 	TIM4_Handler;      	//定时器句柄 
TIM_OC_InitTypeDef 	TIM4_CH2Handler;	//定时器4通道2句柄

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
    __HAL_RCC_GPIOB_CLK_ENABLE();           	//开启GPIOB时钟
	
    GPIO_InitureOutput.Pin=GPIO_PIN_3|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7; 	
    GPIO_InitureOutput.Mode=GPIO_MODE_OUTPUT_PP;  	//推挽输出
    GPIO_InitureOutput.Pull=GPIO_PULLUP;          	//上拉
    GPIO_InitureOutput.Speed=GPIO_SPEED_FREQ_HIGH;    //高速
    HAL_GPIO_Init(GPIOB,&GPIO_InitureOutput);
	
	GPIO_InitureInput.Pin=GPIO_PIN_4; 	
    GPIO_InitureInput.Mode=GPIO_MODE_INPUT;  	//推挽输出
    GPIO_InitureInput.Pull=GPIO_PULLUP;          	//上拉
    GPIO_InitureInput.Speed=GPIO_SPEED_FREQ_HIGH;    //高速
    HAL_GPIO_Init(GPIOB,&GPIO_InitureInput);

	
    CTRL_DIR=1;//逆时针
    CTRL_ENA=0;
//    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET);	//pc6置1，默认初始化后灯灭
}


    
//TIM1 PWM部分初始化 
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
void TIM4_PWM_Init(u16 arr,u16 psc)
{  
    TIM4_Handler.Instance=TIM4;         	//定时器4
    TIM4_Handler.Init.Prescaler=psc;       	//定时器分频
    TIM4_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;//向上计数模式
    TIM4_Handler.Init.Period=arr;          	//自动重装载值
    TIM4_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&TIM4_Handler);       	//初始化PWM

    TIM4_CH2Handler.OCMode=TIM_OCMODE_PWM1; //模式选择PWM1
    TIM4_CH2Handler.Pulse=arr/2;            //设置比较值,此值用来确定占空比，默认比较值为自动重装载值的一半,即占空比为50%
    TIM4_CH2Handler.OCPolarity=TIM_OCPOLARITY_HIGH; //输出比较极性为高 
    HAL_TIM_PWM_ConfigChannel(&TIM4_Handler,&TIM4_CH2Handler,TIM_CHANNEL_2);//配置TIM4通道2
    
    HAL_TIM_Base_Start_IT(&TIM4_Handler); //使能定时器4和定时器4更新中断：TIM_IT_UPDATE  
	HAL_NVIC_SetPriority(TIM4_IRQn,1,0);    //设置中断优先级，抢占优先级2，子优先级0
    HAL_NVIC_EnableIRQ(TIM4_IRQn);          //开启ITM4中断通道  


}

//定时器底层驱动，时钟使能，引脚配置
//此函数会被HAL_TIM_PWM_Init()调用
//htim:定时器句柄
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
	GPIO_InitTypeDef GPIO_Initure;
	
    if(htim->Instance==TIM4)
	{
		__HAL_RCC_TIM4_CLK_ENABLE();			//使能定时器4
		__HAL_RCC_GPIOB_CLK_ENABLE();			//开启GPIOB时钟
		
		GPIO_Initure.Pin=GPIO_PIN_7;           	//PB7
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;  	//复用推挽输出
		GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
		GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//高速
		HAL_GPIO_Init(GPIOB,&GPIO_Initure); 	
	}
}

//设置TIM4通道2的占空比
//compare:比较值
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
	HAL_NVIC_EnableIRQ(TIM4_IRQn);          //开启ITM4中断通道
	EnableMotorPWM(); 	
	CTRL_DIR=1;
    while((u16)chuli_current_val <= (pressuresetvalue - 15) && ((bjiconstatue&0x01) == 1))
    {
        XiYinControl();//在加速过程中也能对吸引泵进行控制
        delay_ms(15);
        speedtimecount++;
        if(speedtimecount%2==0)
            motorpscstart--;
		timecount++;
		if(  motorpscstart < Motor_PscCount( motogear )) //保证加速退出时的速度不大于当前设定挡位的速度
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
	
//    HAL_NVIC_DisableIRQ(TIM4_IRQn);          //关闭ITM4中断通道      
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
				DwinBitIconDisplay(MOTORALARMADDR,19);//显示电机报警弹窗
			}        
        }
    }       
    else
    {
        if(motorAlarmflag == SET)
		{
            motorAlarmflag = RESET; 
			DwinBitIconDisplay(MOTORALARMADDR,0);//清除电机报警弹窗       
        }
    }
}
