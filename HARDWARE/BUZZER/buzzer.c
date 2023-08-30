#include "buzzer.h"
#include "timer.h"
u8 buzzermode; 
//BUZZER IO初始化
void BUZZER_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOC_CLK_ENABLE();           	//开启GPIOC时钟
	
    GPIO_Initure.Pin=GPIO_PIN_5; 				//PC5
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  	//推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          	//上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;    //高速
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);

    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_5,GPIO_PIN_RESET);	//Pc5置1，默认初始化后蜂鸣器不叫，高电平buzzer叫
    
    TIM5_Init(50-1,7200-1);//定时器5初始化，定时器时钟为72M，分频系数为7200-1，所以定时器3的频率为72M/7200=10K，自动重装载为50-1，那么定时器周期就是5ms      
}


//入口参数：  BUZZER_MODE_LOW
//           BUZZER_MODE_MIDDLE
//   _____       _____
//__| 175 |_175_| 175 |____________3000_______________ 低级：ms

//   _____       _____       _____
//__| 175 |_175_| 175 |_175_| 175 |____2000___________ 高级：ms

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
    HAL_NVIC_EnableIRQ(TIM5_IRQn);          //开启ITM5中断 
}
void DisableBuzzer()
{
    HAL_NVIC_DisableIRQ(TIM5_IRQn);         //关闭ITM5中断 
    BUZZER =0;                              //立即关闭buzzer
}
