#include "selftestfuncation.h"
#include "adc.h"
#include "key.h"
#include "textdisplay.h"
#include "dwindisplay.h"
#include "pwm.h"
#include "myiic.h"
#include "delay.h"
#include "led.h" 
#include "parameters.h"

extern TIM_HandleTypeDef 	TIM4_Handler;      	//��ʱ����� 
extern TIM_OC_InitTypeDef 	TIM4_CH4Handler;	//��ʱ��4ͨ��4���      

u8 sendonlyonce = RESET;//��ѹ���㷢��һ�εı�־λ����Ҫ��������Ϣ��reset��������������������
extern u8 varsendonlyonce;//ʹ��һ�Σ�������һ�α��Сͼ��
extern u8 flag500ms;
extern float flowb;

vs16 autopwmvalue=1000;
//4.1.1	�������ѹ�����أ���Ͽ�������E03,��ʾ����Դѹ��������ܻ����������׼�������ܽ��У��Ƿ������YES NO��
//      ѡYES������һ����ѡNO��ֹͣ��������ѡ����һֱѯ���У�һֱ���ѹ�����أ��絼ͨ����ֱ�ӽ�����һ����
void PowerOnSelfTestPressureSwitch()
{
    u8 key;
    if( key == 0 )//��⵽ѹ�������ǶϿ���    0��ʾѹ�������ǶϿ���               
    {
        u8 keepworking =0;//Ĭ������ѹ�����ضϿ���ʱ��ֱ����ת��E03����
        E03InsufficientCO2SourcePressure(0);//����E03;��ʾ��E03��Դѹ��������ܻ����������׼�������ܽ��У��Ƿ������YES NO��
        while( keepworking != YES )//û��ѡ��yes��
        {
            keepworking = ReadDwinKeyValueReturn();//��ȡdwin��Ļ�ϵİ������أ��Ƿ�yes����������
            if( keepworking == YES || key == 1 )//һֱѯ���У�һֱ���ѹ�����أ��絼ͨ����ֱ�ӽ�����һ��//ѹ������Сͼ��ָ�����    
            {
                DisableBuzzer();
                if(key == 1)DwinBitIconDisplay(RRESSUREICONADDR,ICONGREEN);//����ǿ��ص�ͨ�˾ͻָ�ѹ�����ؼ���Сͼ��
                break;
            }
            if( keepworking == MUTE)DisableBuzzer();//�رձ�����
        }
        DwindisplayInterface( MAININTERFACE );//����������
        PrintfText(CLEAR,INFORADRESS);//�����ʾ��Ϣ
        varsendonlyonce=0;
    }
    else
    {}   
}

//ʹ�ù����г�����Դѹ�����㣬��ʾE03 ��Դѹ�����㣬����ʾѡ��
void InProcessTestPressureSwitch()
{
    u8 key; 
    if( key == 0 && sendonlyonce == 0 )//��⵽ѹ�������ǶϿ���                   
    {
        E03InsufficientCO2SourcePressure(1);//����E03;��ʾ��E03��Դѹ�����㡱
        DwinBitIconDisplay(RRESSUREICONADDR,ICONDARKGREEN);//Сͼ����û�е�ͨ������
        sendonlyonce = SET;
    }
    else if( key == 1 && sendonlyonce == 1 )
    {
        sendonlyonce =RESET;
        DwinBitIconDisplay(RRESSUREICONADDR,ICONGREEN);//����ǿ��ص�ͨ�˾ͻָ�ѹ�����ؼ���Сͼ��
        PrintfText(CLEAR,INFORADRESS);//�����ʾ��Ϣ
    }        
}

//��ȡ������������ֵ��Ȼ����趨��ֵ�Ƚϣ���������Ĵ�����ֵ���趨��С��������ռ�ձȣ�
void AddPWM(u8 readflowvalue)
{
    if(flag500ms)
    {
        flag500ms=0;
        if(readflowvalue<=para2flash[1]-2)
        {
            autopwmvalue++;
        }
        else if(readflowvalue>=para2flash[1]+2)
        {
            autopwmvalue--;
        }
        else
            autopwmvalue=autopwmvalue;             
    }
}
//4.1.2	������������������Լ췧����������10L/min��Ӧ��ռ�ձȳ���2��T0-T2��Сֵ�� ���Fa��0��������һ����Fa=0��
//       �򱨴�E01��ֹͣ��������ʾ �Լ�δ��ɣ�  
void FlowSensorSelfTest()
{
    u16 i;
    u8 cafs300value =0;
    u8 dwinkeyreturnvalue;
    TIM_SetTIM4Compare4(2000);//�޸ıȽ�ֵ���ı�ռ�ձ�  ��ֵԽ��ռ�ձ�Խ�� ��ʱ��2000
    EnableProportion();//����������
    for( i=0;i<T0_T2_Min*2;i++ )
    {
        delay_ms(1);
    }
    cafs300value = Cafs300ReadData();//��ȡ������������ֵ
    DwinDisplayFlow(cafs300value);
    while( cafs300value == 0)//���������������ֵΪ0
    {
        E01InflationPipelineFailure();
        dwinkeyreturnvalue = ReadDwinKeyValueReturn();//��ȡdwin��Ļ�ϵİ������أ��Ƿ�yes����������
        if(dwinkeyreturnvalue==MUTE)DisableBuzzer();//�رձ�����
    } 
    DisableProportion();//�رձ�����������T0-T2��Сֵ��������������
    DwinDisplayVariate(FLOWSENSORADDR,CLEAR);
}


//4.1.3	���ѹ�����������Լ췧�����������ڹر�״̬����������10L/min  T0-T2��Сֵ�����������ѹ����򿪷�������
//      �ټ��ѹ���������رշ����������Pa�ɸߵ��ͱ仯������һ�������򱨾�E02ֹͣ��������ʾ �Լ�δ��ɣ�
void PressureSensorSelfTest()
{
    u16 i;
    u16 firstpressure=0;
    u16 secondpressure=0;
    u8 dwinkeyreturnvalue=0;
    TIM_SetTIM4Compare4(1850);//�޸ıȽ�ֵ���ı�ռ�ձ�  ��ֵԽ��ռ�ձ�Խ��,��������10L/min����
    EnableProportion();//����������
    for(i=0;i<T0_T2_Min;i++)//����T0_T2_Minʱ��
    {
        delay_ms(1);            
    }
    DisableProportion();//�رձ�������T0-T2��Сֵ��������������
    firstpressure=PositivePrussure();//��ʱ����һ��ѹ��
    delay_ms(50);
    secondpressure=PositivePrussure();//��ʱ���ڶ���ѹ��
    if(firstpressure-secondpressure<=30)//�򿪷�����йѹʧ�ܣ�����ǰ�����ε�ѹ������
    {
        E02InflationPipelineFailure(); 
        while(1)
        {
            dwinkeyreturnvalue = ReadDwinKeyValueReturn();//��ȡdwin��Ļ�ϵİ������أ��Ƿ�yes����������
            if(dwinkeyreturnvalue==MUTE)DisableBuzzer();//�رձ�����
        }
    }
    else   
    {
        DisableBuzzer();//�رձ�����
        DwindisplayInterface( MAININTERFACE );//����������
        PrintfText(CLEAR,INFORADRESS);//�����ʾ��Ϣ
    } 
}

//4.1.4	���CO2���壺һ�����Ϳ�ʼ���ȴ�������������󣬱��� E06 ��ʾ�� �����������ȷ����CO2�� YES   NO��
//      ѡYES ����Լ췧����ʾ�Լ���ɡ� ѡNO��ֹͣ��������ʾ �Լ�δ��ɣ�
void CO2SelfTest()
{
    if((para2flash[2]&0x0001)==1)
    {
                
    } 
    DisableBuzzer();//�رձ�����
    DwindisplayInterface( MAININTERFACE );//����������
    PrintfText(CLEAR,INFORADRESS);//�����ʾ��Ϣ
    SelfChecking( SELF_TEST_COMPLETE );    
}

//4.1.5	��������·����û�м�⵽��·�����ɳ����� ��ʾ��װ������·��������·����·����100�Σ����5��ʱ��Ϣ��ʾ
//      ������׼����������100��ֹͣ��������ʾ����������·�Ƿ�ԭ���������������ػ�һ�ι�·������һ�Σ���Ҫ��
//      ���·����˿�ʹ������ĺû���
void LoadingLineSelfTest()
{
}








