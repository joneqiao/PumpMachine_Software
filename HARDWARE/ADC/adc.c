#include "adc.h"
#include "delay.h"
#include "math.h"

extern float MXP2100offset;
extern float MXP2100sensitivity;
extern float FSS1500offset;

extern float chulik;
extern float chulib;
extern u16 waterhight;
extern float negativepressureK;
extern float negativepressureB;

ADC_HandleTypeDef ADC1_Handler;

/**
 * @brief 初始化ADC模块
 *
 * 该函数用于初始化ADC模块，包括时钟配置、ADC参数设置等。
 *
 * @param 无
 * @return 无
 */
void ADC_Init(void)
{ 
	RCC_PeriphCLKInitTypeDef ADC_CLKInit;
	
	// 设置ADC时钟源
	ADC_CLKInit.PeriphClockSelection=RCC_PERIPHCLK_ADC;			//ADC时钟源选择
	ADC_CLKInit.AdcClockSelection=RCC_ADCPCLK2_DIV6;			//将PCLK2时钟分频6倍后为ADC时钟，即72M/6=12MHz
	HAL_RCCEx_PeriphCLKConfig(&ADC_CLKInit);					//配置ADC时钟
	
    // 配置ADC1
    ADC1_Handler.Instance=ADC1;
    // 配置ADC数据对齐方式
    ADC1_Handler.Init.DataAlign=ADC_DATAALIGN_RIGHT;             //右对齐
    // 配置扫描模式
    ADC1_Handler.Init.ScanConvMode=DISABLE;                      //禁用扫描模式
    // 配置连续转换模式
    ADC1_Handler.Init.ContinuousConvMode=DISABLE;                //禁用连续转换模式
    // 配置转换序列长度
    ADC1_Handler.Init.NbrOfConversion=1;                         //转换序列长度为1，即每次只转换一个通道
    // 配置不连续转换模式
    ADC1_Handler.Init.DiscontinuousConvMode=DISABLE;             //禁用不连续转换模式
    // 配置不连续转换序列长度
    ADC1_Handler.Init.NbrOfDiscConversion=0;                     //不连续转换序列长度为0
    // 配置外部触发
    ADC1_Handler.Init.ExternalTrigConv=ADC_SOFTWARE_START;       //软件触发
    HAL_ADC_Init(&ADC1_Handler);                                 //初始化ADC
	
	// ADC校准
	HAL_ADCEx_Calibration_Start(&ADC1_Handler);					 //启动ADC校准
}

/**
 * @brief 初始化ADC硬件抽象层（HAL）的Msp部分
 *
 * 初始化ADC硬件抽象层（HAL）的Msp部分，包括使能ADC1时钟、GPIOC时钟，并初始化ADC1相关的GPIOC引脚为模拟输入模式。
 *
 * @param hadc ADC句柄指针
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    GPIO_InitTypeDef GPIO_Initure;
    // 使能ADC1时钟
    __HAL_RCC_ADC1_CLK_ENABLE();                                //启用ADC1时钟
    // 使能GPIOC时钟
    __HAL_RCC_GPIOC_CLK_ENABLE();			                    //使能GPIOC时钟
	
    // 配置PC0和PC1引脚
    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1;                 //PC0->模拟输入引脚    PC1->模拟输入引脚
    // 设置引脚模式为模拟输入
    GPIO_Initure.Mode = GPIO_MODE_ANALOG;                         //模式
    // 设置引脚无上拉/下拉电阻
    GPIO_Initure.Pull = GPIO_NOPULL;                              //无上拉/下拉
    // 初始化GPIOC端口
    HAL_GPIO_Init(GPIOC, &GPIO_Initure);
}

/**
 * @brief 获取指定通道的ADC值
 *
 * 获取指定ADC通道的ADC值，并返回该值。
 ADC初始化部分添加通道配置 
 */
  void ADC_Init(void)
{

    // 配置ADC通道
    ADC_ChannelConfTypeDef ADC1_ChanConf = {0};
    ADC1_ChanConf.Channel = ADC_CHANNEL_0; // 示例通道，实际应根据需要设置
    ADC1_ChanConf.Rank = ADC_REGULAR_RANK_1;
    ADC1_ChanConf.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
    HAL_ADC_ConfigChannel(&ADC1_Handler, &ADC1_ChanConf);
}

u16 Get_Adc(u32 ch) {
    // 假设ADC通道已经在ADC_Init中配置好，这里不再重新配置
    // 只需选择对应的通道，启动ADC，并等待转换完成

    // 设置当前ADC通道（如果需要动态改变通道）
    // 注意：根据硬件和库的实现，这一步可能是必要的，也可能不必要
    // ADC1_Handler.Instance->CHSELR = (1UL << ch); // 示例代码，具体实现可能不同

    // 启动ADC
    HAL_ADC_Start(&ADC1_Handler);

    // 等待转换完成
    if (HAL_ADC_PollForConversion(&ADC1_Handler, 10) != HAL_OK) {
        // 处理错误，例如返回特定错误码或重试
        return 0; // 示例错误处理
    }

    // 返回ADC转换结果
    return (u16)HAL_ADC_GetValue(&ADC1_Handler);
}
 * @param ch ADC通道号
 *
 * @return 转换后的ADC值，返回类型为u16
 */
u16 Get_Adc(u32 ch)   
{
    ADC_ChannelConfTypeDef ADC1_ChanConf;
    
    // 设置ADC通道配置
    ADC1_ChanConf.Channel = ch;                                   //通道
    ADC1_ChanConf.Rank = 1;                                       //设置为第1个转换序列，优先级为1
    ADC1_ChanConf.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;      //采样时间               
    HAL_ADC_ConfigChannel(&ADC1_Handler, &ADC1_ChanConf);        //配置通道

    // 启动ADC
    HAL_ADC_Start(&ADC1_Handler);                               //启动ADC

    // 轮询等待转换完成
    HAL_ADC_PollForConversion(&ADC1_Handler, 10);                //轮询查询转换

    // 返回ADC转换结果
    return (u16)HAL_ADC_GetValue(&ADC1_Handler);	        	//获取ADC1的转换结果
}
//��ȡָ��ͨ����ת��ֵ��ȡtimes��,Ȼ��ƽ�� 
//times:��ȡ����
//����ֵ:ͨ��ch��times��ת�����ƽ��ֵ
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
//����ֵ:ͨ��ch�ĵ���ֵ
u16 Get_R_Value(u32 ch)
{   
    u16 adcx;
	float temp;
    adcx=Get_Adc_Average(ch,20);                            //��ȡͨ��x��ת��ֵ��20��ȡƽ��
    temp=(float)adcx*(3.3/4096);                            //��ȡ�����Ĵ�С����ʵ�ʵ�ѹֵ������3.1111  12λ��ad  2��12�η�
    return temp;
}

float NegativePrussure()
{
    u8 i;
    float negativevalue = 0;  
    float negaticetotal;   
    for( i=0; i<20; i++ )
    {    
        negativevalue = Get_Adc( ADC_CHANNEL_10 );//pc0  ��ѹ������  ��λ������1000
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
        chulivalue=Get_Adc(ADC_CHANNEL_11);//pc1  ����������     ��λ������1000
        chulivalue=chulivalue-FSS1500offset;//��ȥ��λ����ֵ   
        chulivalue=chulik*chulivalue+chulib;
        
        if( waterhight <= 100 )chulivalue = chulivalue - 0.75*waterhight;
        else if( 200 < waterhight && waterhight <= 300 )chulivalue = chulivalue + 0.75*(waterhight-200);//����û�и������԰�λ��2Ϊ�������ţ�������Ҫ��ȥ200 
                
        if(chulivalue<30)chulivalue=0;
        chlivaluetotal+=chulivalue;
    }
    chlivaluetotal=chlivaluetotal/20;
    if(chlivaluetotal<32)chlivaluetotal=0;    
    return chlivaluetotal;
}
