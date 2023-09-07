#include "key.h"
#include "parameters.h"
#include "exti.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
/************************************************
 IRcontroller-HAL库函数版
 技术支持：www.logic-elec.com
 南京洛吉克电子科技有限公司  
 作者：jone @LOGIC-ELEC
************************************************/ 
extern u8 xyiconstatue;
u8 JiaoTaFlag = 0;
u8 xiyinflag;
u8 bujinflag;
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


u8 XIYIN_BUTTON_Scan(u8 mode)
{
    static u8 key_up = 1; // 按键松开标志
    if (mode == 1)
        key_up = 1; // 支持连按
    if (key_up && KEY_XIYIN == 0)
    {
        key_up = 0;
        if (KEY_XIYIN == 0)
        {
            xyiconstatue++;
            DwinBitIconDisplay(XYICONADDR,xyiconstatue&0x01);//display xiyin icon  
            return 1;
        }
    }
    else if (KEY_XIYIN == 1)
        key_up = 1;
    return 0; // 无按键按下
}

extern float value_buf[NUM];
extern vs16 para2flash[parameternum];
extern float speedoffset;
u8 CHONGXI_BUTTON_Scan(u8 mode)
{
    static u8 key_up = 1; // 按键松开标志
    if (mode == 1)
        key_up = 1; // 支持连按
    if (key_up && KEY_CHONGXI == 0)
    {
        delay_ms(10);
        key_up = 0;
        if (KEY_CHONGXI == 0)
        {
			bjiconstatue++;
			memset(value_buf,0,NUM);	
			DwinBitIconDisplay(BJICONADDR,bjiconstatue&0x01);//display bujin icon  
			para2flash[19]= pressuresetvalue;
			para2flash[20]= flowsetvalue;
			IWDG_Feed();//feed watch dog befor read data from flash
			STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)para2flash,parameternum);//read parameters from flash  
            if((bjiconstatue&0x01)==0)
                speedoffset=0;   
            return 1;
        }
    }
    else if (KEY_CHONGXI == 1)
        key_up = 1;
    return 0; // 无按键按下
}

void ExternalButtonSwitch()
{
    XIYIN_BUTTON_Scan(0);  
    CHONGXI_BUTTON_Scan(0);
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
