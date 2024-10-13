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

ADC_HandleTypeDef ADC1_Handler;		                            //ADC���


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

//ADC�ײ��������������ã�ʱ��ʹ��
//�˺����ᱻHAL_ADC_Init()����
//hadc:ADC���
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_ADC1_CLK_ENABLE();                                //ʹ��ADC1ʱ��
    __HAL_RCC_GPIOC_CLK_ENABLE();			                    //����GPIOCʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1;                     //PC0->��ѹ������    PC1->����������
    GPIO_Initure.Mode=GPIO_MODE_ANALOG;                         //ģ��
    GPIO_Initure.Pull=GPIO_NOPULL;                              //����������
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
}

//���ADCֵ
//ch: ͨ��ֵ 0~16��ȡֵ��ΧΪ��ADC_CHANNEL_0~ADC_CHANNEL_16
//����ֵ:ת�����
u16 Get_Adc(u32 ch)   
{
    ADC_ChannelConfTypeDef ADC1_ChanConf;
    
    ADC1_ChanConf.Channel=ch;                                   //ͨ��
    ADC1_ChanConf.Rank=1;                                       //��1�����У�����1
    ADC1_ChanConf.SamplingTime=ADC_SAMPLETIME_239CYCLES_5;      //����ʱ��               
    HAL_ADC_ConfigChannel(&ADC1_Handler,&ADC1_ChanConf);        //ͨ������
	
    HAL_ADC_Start(&ADC1_Handler);                               //����ADC
	
    HAL_ADC_PollForConversion(&ADC1_Handler,10);                //��ѯת��
 
	return (u16)HAL_ADC_GetValue(&ADC1_Handler);	        	//�������һ��ADC1�������ת�����
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
