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
 IRcontroller-HAL库函数版
 技术支持：www.logic-elec.com
 南京洛吉克电子科技有限公司  
 作者：jone @LOGIC-ELEC
************************************************/ 									  
////////////////////////////////////////////////////////////////////////////////// 

//捕获状态
//[7]:0,没有成功的捕获;1,成功捕获到一次.
//[6]:0,还没捕获到低电平;1,已经捕获到低电平了.
//[5:0]:捕获低电平后溢出的次数
u8  TIM4CH4_CAPTURE_STA=0;							//输入捕获状态		    				
u32	TIM4CH4_CAPTURE_VAL;							//输入捕获值(TIM4是16位)

u8 mototimeconutflag=0;
u8 mototimeconut=0;
extern u8 xyiconstatue;
extern u8 bjiconstatue;
extern u8 buzzermode;
extern u8 xiyinonceflag;


u8 getpressureflag =1;
u8 getflowflag     =1;

extern IWDG_HandleTypeDef IWDG_Handler; //独立看门狗句柄
TIM_HandleTypeDef TIM2_Handler;         //定时器2句柄 
TIM_HandleTypeDef TIM4_Handler;         //定时器4句柄
TIM_HandleTypeDef TIM3_Handler;         //定时器3句柄 
TIM_HandleTypeDef TIM5_Handler;         //定时器5句柄 
//通用定时器3中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!
void TIM2_Init(u16 arr,u16 psc)
{  
    TIM2_Handler.Instance=TIM2;                          //通用定时器2
    TIM2_Handler.Init.Prescaler=psc;                     //分频系数
    TIM2_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
    TIM2_Handler.Init.Period=arr;                        //自动装载值
    TIM2_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//时钟分频因子
    HAL_TIM_Base_Init(&TIM2_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM2_Handler); //使能定时器3和定时器3更新中断：TIM_IT_UPDATE   
}

void TIM3_Init(u16 arr,u16 psc)
{  
    TIM3_Handler.Instance=TIM3;                          //通用定时器3
    TIM3_Handler.Init.Prescaler=psc;                     //分频系数
    TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
    TIM3_Handler.Init.Period=arr;                        //自动装载值
    TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//时钟分频因子
    HAL_TIM_Base_Init(&TIM3_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM3_Handler); //使能定时器3和定时器3更新中断：TIM_IT_UPDATE   
}

//定时器4通道4输入捕获配置
//arr：自动重装值(TIM4是16位的!!)
//psc：时钟预分频数
void TIM4_CH4_Cap_Init(u32 arr,u16 psc)
{  
    TIM_IC_InitTypeDef TIM4_CH4Config;  
    
    TIM4_Handler.Instance=TIM4;                          //通用定时器4
    TIM4_Handler.Init.Prescaler=psc;                     //分频系数
    TIM4_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
    TIM4_Handler.Init.Period=arr;                        //自动装载值
    TIM4_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//时钟分频银子
    HAL_TIM_IC_Init(&TIM4_Handler);						//初始化输入捕获时基参数
    
    TIM4_CH4Config.ICPolarity=TIM_ICPOLARITY_FALLING;    //下降沿捕获
    TIM4_CH4Config.ICSelection=TIM_ICSELECTION_DIRECTTI;//映射到TI1上
    TIM4_CH4Config.ICPrescaler=TIM_ICPSC_DIV1;          //配置输入分频，不分频
    TIM4_CH4Config.ICFilter=0;                          //配置输入滤波器，不滤波
    HAL_TIM_IC_ConfigChannel(&TIM4_Handler,&TIM4_CH4Config,TIM_CHANNEL_4);//配置TIM4通道4
	
    HAL_TIM_IC_Start_IT(&TIM4_Handler,TIM_CHANNEL_4);   //开启TIM4的捕获通道4，并且开启捕获中断
    HAL_TIM_Base_Start_IT(&TIM4_Handler); //使能定时器4和定时器4更新中断：TIM_IT_UPDATE  
	HAL_NVIC_SetPriority(TIM4_IRQn,3,0);    //设置中断优先级，抢占优先级2，子优先级0
    HAL_NVIC_EnableIRQ(TIM4_IRQn);          //开启ITM4中断通道  
}

//定时器4底层驱动，时钟使能，引脚配置
//此函数会被HAL_TIM_IC_Init()调用
//htim:定时器4句柄
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_TIM4_CLK_ENABLE();            //使能TIM4时钟
    __HAL_RCC_GPIOB_CLK_ENABLE();			//开启GPIOB时钟
	
    GPIO_Initure.Pin=GPIO_PIN_9;            //PB9
    GPIO_Initure.Mode=GPIO_MODE_AF_INPUT; 	//复用推挽输入
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//高速
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);

    HAL_NVIC_SetPriority(TIM4_IRQn,4,0);    //设置中断优先级，抢占优先级2，子优先级0
    HAL_NVIC_EnableIRQ(TIM4_IRQn);          //开启ITM4中断通道  
   
}

void TIM5_Init(u16 arr,u16 psc)
{  
    TIM5_Handler.Instance=TIM5;                          //通用定时器5
    TIM5_Handler.Init.Prescaler=psc;                     //分频系数
    TIM5_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
    TIM5_Handler.Init.Period=arr;                        //自动装载值
    TIM5_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//时钟分频因子
    HAL_TIM_Base_Init(&TIM5_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM5_Handler); //使能定时器3和定时器3更新中断：TIM_IT_UPDATE   
}

//定时器底册驱动，开启时钟，设置中断优先级
//此函数会被HAL_TIM_Base_Init()函数调用
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM2)
	{
		__HAL_RCC_TIM2_CLK_ENABLE();            //使能TIM2时钟
		HAL_NVIC_SetPriority(TIM3_IRQn,4,0);    //设置中断优先级，抢占优先级1，子优先级3
		HAL_NVIC_EnableIRQ(TIM2_IRQn);          //开启ITM2中断   
	}
    if(htim->Instance==TIM3)
	{
		__HAL_RCC_TIM3_CLK_ENABLE();            //使能TIM3时钟
		HAL_NVIC_SetPriority(TIM3_IRQn,5,0);    //设置中断优先级，抢占优先级1，子优先级3
		HAL_NVIC_EnableIRQ(TIM3_IRQn);          //开启ITM3中断   
	}
        if(htim->Instance==TIM5)
	{
		__HAL_RCC_TIM5_CLK_ENABLE();            //使能TIM5时钟
		HAL_NVIC_SetPriority(TIM5_IRQn,6,0);    //设置中断优先级，抢占优先级1，子优先级3
//		HAL_NVIC_EnableIRQ(TIM5_IRQn);          //开启ITM5中断   
	}
}

//定时器2中断服务函数
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

//回调函数，定时器中断服务函数调用
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim==(&TIM2_Handler))
    {
        if(mototimeconutflag==1)
        {
            mototimeconut++;
            if(mototimeconut>20)
            {
                xyiconstatue=RESET;//关闭泵
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
        if(0  <   millisec5ms && millisec5ms < 35)   BUZZER =1;                      //高175ms  BUZZER     高电平buzzer叫
        if(35 <=  millisec5ms && millisec5ms < 70)   BUZZER =0;                      //低175ms
        if(70 <=  millisec5ms && millisec5ms < 105)  BUZZER =1;                      //高175ms
        if(175<=  millisec5ms && millisec5ms <= 6000)BUZZER =0;                      //低3000ms  
        if(millisec5ms>6000)millisec5ms=0;         
    }
    if(htim==(&TIM5_Handler) && buzzermode==BUZZER_MODE_MIDDLE)
    {             
        static u16 millisec5ms;
        millisec5ms++;
        if(0  <   millisec5ms && millisec5ms < 35)   BUZZER =1;                      //高175ms  BUZZER     高电平buzzer叫
        if(35 <=  millisec5ms && millisec5ms < 70)   BUZZER =0;                      //低175ms
        if(70 <=  millisec5ms && millisec5ms < 105)  BUZZER =1;                      //高175ms
        if(105<= millisec5ms  && millisec5ms < 140)  BUZZER =0;                      //低175ms
        if(140<= millisec5ms  && millisec5ms < 175)  BUZZER =1;                      //高175ms
        if(175<= millisec5ms  && millisec5ms <= 4000)BUZZER =0;                      //低2000ms  
        if(millisec5ms>4000)millisec5ms=0;        
    }
}

//定时器4中断服务函数
void TIM4_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM4_Handler);				//定时器共用处理函数
}

//定时器输入捕获中断处理回调函数，该函数在HAL_TIM_IRQHandler中会被调用
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)//捕获中断发生时执行
{
	if((TIM4CH4_CAPTURE_STA&0X80)==0)				//还未成功捕获
	{
		if(TIM4CH4_CAPTURE_STA&0X40)				//捕获到一个下降沿 		
		{	  			
			TIM4CH4_CAPTURE_STA|=0X80;				//标记成功捕获到一次低电平脉宽
            xyiconstatue=0;//泵停止工作
            DwinBitIconDisplay(XYICONADDR,0);//display xiyin icon  
            mototimeconut=0;
            mototimeconutflag=0;
            TIM4CH4_CAPTURE_VAL=HAL_TIM_ReadCapturedValue(&TIM4_Handler,TIM_CHANNEL_4);//获取当前的捕获值.
			TIM_RESET_CAPTUREPOLARITY(&TIM4_Handler,TIM_CHANNEL_4);   //一定要先清除原来的设置！！
            TIM_SET_CAPTUREPOLARITY(&TIM4_Handler,TIM_CHANNEL_4,TIM_ICPOLARITY_FALLING);//配置TIM4通道4下降沿捕获
		}else  										//还未开始,第一次捕获下降沿
		{
			TIM4CH4_CAPTURE_STA=0;					//清空
			TIM4CH4_CAPTURE_VAL=0;
			TIM4CH4_CAPTURE_STA|=0X40;				//标记捕获到了下降沿
            xyiconstatue=1;//泵开始工作 
            xiyinonceflag = RESET;
            DwinBitIconDisplay(XYICONADDR,1);//display xiyin icon  
            mototimeconutflag=1;//启动泵计时，如果超过20秒，就停止泵
			__HAL_TIM_DISABLE(&TIM4_Handler);      	//关闭定时器4
			__HAL_TIM_SET_COUNTER(&TIM4_Handler,0);
			TIM_RESET_CAPTUREPOLARITY(&TIM4_Handler,TIM_CHANNEL_4);   //一定要先清除原来的设置！！
			TIM_SET_CAPTUREPOLARITY(&TIM4_Handler,TIM_CHANNEL_4,TIM_ICPOLARITY_RISING);//定时器4通道4设置为上升沿捕获
			__HAL_TIM_ENABLE(&TIM4_Handler);		//使能定时器4
		}		    
	}		
}



