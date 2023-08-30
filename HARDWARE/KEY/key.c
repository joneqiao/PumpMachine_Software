#include "key.h"
#include "parameters.h"
/************************************************
 IRcontroller-HAL库函数版
 技术支持：www.logic-elec.com
 南京洛吉克电子科技有限公司  
 作者：jone @LOGIC-ELEC
************************************************/ 
extern u8 xyiconstatue;
u8 JiaoTaFlag = 0;

//按键初始化函数
void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOA_CLK_ENABLE();                   //开启GPIOA时钟  PA5 ->吸引
    __HAL_RCC_GPIOB_CLK_ENABLE();                   //开启GPIOB时钟  PB9 ->脚踏开关
    __HAL_RCC_GPIOC_CLK_ENABLE();                   //开启GPIOC时钟  PC12->冲洗 
    
    GPIO_Initure.Pin=GPIO_PIN_15|GPIO_PIN_9|GPIO_PIN_12;  
    GPIO_Initure.Mode=GPIO_MODE_INPUT;                   //输入
    GPIO_Initure.Pull=GPIO_PULLUP;                       //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;             //高速
    
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
}

void FeetSwitch()
{
//	static u8 jiaotaflag = 0; 
    static u16 timesec = 0;
	
    if( KEY_JIAOTA == 0 )
    {
        delay_ms(10);
        timesec++;
        if( KEY_JIAOTA == 0 )
        {
			if(JiaoTaFlag == RESET)
			{
				JiaoTaFlag = SET;
				DwinBitIconDisplay(XYICONADDR,1);//display xiyin icon //图标变成使能状态
			}        
        }
        if( timesec >= 2000 )
        {
            timesec = 2000;
            JiaoTaFlag = RESET;
			DwinBitIconDisplay(XYICONADDR,0);//display xiyin icon //图标变成失能状态
        }
    }       
    else
    {
        timesec = 0;
        if(JiaoTaFlag == SET)//jiaota flag
        {
            JiaoTaFlag = RESET; 
			DwinBitIconDisplay(XYICONADDR,0);//display xiyin icon //图标变成失能状态
			xyiconstatue = RESET;//关闭泵         
        }
    }
}

void XiYinControl()
{
	static u8 deadzoneflag = RESET;
	
	if( xyiconstatue&0x01 || JiaoTaFlag == SET)
	{
		if( negative_pressure_current_val  <=  presetnpressure && deadzoneflag == RESET )// if negative pressure below negative pressuer preset and dead zone flag reset;   
		{
			ATTRACT_CMOS = SET;
		}
		else
		{
			if( negative_pressure_current_val > (presetnpressure - 10 ))
			{
				deadzoneflag = SET;
				ATTRACT_CMOS = RESET;	
			}
			else
				deadzoneflag = RESET;
		}		
	}
	else
	{
		deadzoneflag = RESET;
		ATTRACT_CMOS = RESET;	 
	}
}
