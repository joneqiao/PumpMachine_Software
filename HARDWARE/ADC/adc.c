#include "adc.h"
#include "delay.h"
#include "math.h"
/************************************************
 IRcontroller-HAL库函数版
 技术支持：www.logic-elec.com
 南京洛吉克电子科技有限公司  
 作者：jone @LOGIC-ELEC
************************************************/ 
extern float MXP2100offset;
extern float MXP2100sensitivity;
extern float FSS1500offset;

extern float chulik;
extern float chulib;
extern u16 waterhight;
extern float negativepressureK;
extern float negativepressureB;

ADC_HandleTypeDef ADC1_Handler;		                            //ADC句柄
//初始化ADC
//ch: ADC_channels 
//通道值 0~16取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_16
void ADC_Init(void)
{ 
	RCC_PeriphCLKInitTypeDef ADC_CLKInit;
	
	ADC_CLKInit.PeriphClockSelection=RCC_PERIPHCLK_ADC;			//ADC外设时钟
	ADC_CLKInit.AdcClockSelection=RCC_ADCPCLK2_DIV6;			//分频因子6时钟为72M/6=12MHz
	HAL_RCCEx_PeriphCLKConfig(&ADC_CLKInit);					//设置ADC时钟
	
    ADC1_Handler.Instance=ADC1;
    ADC1_Handler.Init.DataAlign=ADC_DATAALIGN_RIGHT;             //右对齐
    ADC1_Handler.Init.ScanConvMode=DISABLE;                      //非扫描模式
    ADC1_Handler.Init.ContinuousConvMode=DISABLE;                //关闭连续转换
    ADC1_Handler.Init.NbrOfConversion=1;                         //1个转换在规则序列中 也就是只转换规则序列1 
    ADC1_Handler.Init.DiscontinuousConvMode=DISABLE;             //禁止不连续采样模式
    ADC1_Handler.Init.NbrOfDiscConversion=0;                     //不连续采样通道数为0
    ADC1_Handler.Init.ExternalTrigConv=ADC_SOFTWARE_START;       //软件触发
    HAL_ADC_Init(&ADC1_Handler);                                 //初始化 
	
	HAL_ADCEx_Calibration_Start(&ADC1_Handler);					 //校准ADC
}

//ADC底层驱动，引脚配置，时钟使能
//此函数会被HAL_ADC_Init()调用
//hadc:ADC句柄
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_ADC1_CLK_ENABLE();                                //使能ADC1时钟
    __HAL_RCC_GPIOC_CLK_ENABLE();			                    //开启GPIOC时钟
	
    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1;                     //PC0->负压传感器    PC1->触力传感器
    GPIO_Initure.Mode=GPIO_MODE_ANALOG;                         //模拟
    GPIO_Initure.Pull=GPIO_NOPULL;                              //不带上下拉
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
}

//获得ADC值
//ch: 通道值 0~16，取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_16
//返回值:转换结果
u16 Get_Adc(u32 ch)   
{
    ADC_ChannelConfTypeDef ADC1_ChanConf;
    
    ADC1_ChanConf.Channel=ch;                                   //通道
    ADC1_ChanConf.Rank=1;                                       //第1个序列，序列1
    ADC1_ChanConf.SamplingTime=ADC_SAMPLETIME_239CYCLES_5;      //采样时间               
    HAL_ADC_ConfigChannel(&ADC1_Handler,&ADC1_ChanConf);        //通道配置
	
    HAL_ADC_Start(&ADC1_Handler);                               //开启ADC
	
    HAL_ADC_PollForConversion(&ADC1_Handler,10);                //轮询转换
 
	return (u16)HAL_ADC_GetValue(&ADC1_Handler);	        	//返回最近一次ADC1规则组的转换结果
}
//获取指定通道的转换值，取times次,然后平均 
//times:获取次数
//返回值:通道ch的times次转换结果平均值
u16 Get_Adc_Average(u32 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(5);
	}
	return temp_val/times;
} 
//返回值:通道ch的电阻值
u16 Get_R_Value(u32 ch)
{   
    u16 adcx;
	float temp;
    adcx=Get_Adc_Average(ch,20);                            //获取通道x的转换值，20次取平均
    temp=(float)adcx*(3.3/4096);                            //获取计算后的带小数的实际电压值，比如3.1111  12位的ad  2的12次方
    return temp;
}

float NegativePrussure()
{
    u8 i;
    float negativevalue = 0;  
    float negaticetotal;   
    for( i=0; i<20; i++ )
    {    
        negativevalue = Get_Adc( ADC_CHANNEL_10 );//pc0  负压传感器  电位器调到1000
        negativevalue = ( negativevalue )*( 3.3/4096 ) * 1000;//mv
        negativevalue = negativevalue - MXP2100offset;
        negativevalue = negativevalue / MXP2100sensitivity;
        negativevalue = negativepressureK * negativevalue + negativepressureB;
        if( negativevalue < 0 ) negativevalue = 0;   
        negaticetotal += negativevalue; 
    }
    negaticetotal = negaticetotal / 20; 
    return negaticetotal;
}

float ChuliSensorPrussure()
{
    u8 i;
    float chulivalue=0;
    float chlivaluetotal=0;
    for(i=0;i<20;i++)
    {
        chulivalue=Get_Adc(ADC_CHANNEL_11);//pc1  触力传感器     电位器调到1000
        chulivalue=chulivalue-FSS1500offset;//减去点位器的值   
        chulivalue=chulik*chulivalue+chulib;
        
        if( waterhight <= 100 )chulivalue = chulivalue - 0.75*waterhight;
        else if( 200 < waterhight && waterhight <= 300 )chulivalue = chulivalue + 0.75*(waterhight-200);//键盘没有负数，以百位的2为负数符号，所以需要减去200 
                
        if(chulivalue<30)chulivalue=0;
        chlivaluetotal+=chulivalue;
    }
    chlivaluetotal=chlivaluetotal/20;
    if(chlivaluetotal<32)chlivaluetotal=0;    
    return chlivaluetotal;
}
