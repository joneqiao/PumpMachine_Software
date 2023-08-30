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

extern TIM_HandleTypeDef 	TIM4_Handler;      	//定时器句柄 
extern TIM_OC_InitTypeDef 	TIM4_CH4Handler;	//定时器4通道4句柄      

u8 sendonlyonce = RESET;//气压不足发送一次的标志位，需要再其他信息中reset！！！！！！！！！！
extern u8 varsendonlyonce;//使能一次，即发送一次变更小图标
extern u8 flag500ms;
extern float flowb;

vs16 autopwmvalue=1000;
//4.1.1	开机检测压力开关，如断开，报警E03,提示“气源压力不足可能会造成流量不准或工作不能进行，是否继续？YES NO”
//      选YES继续下一步，选NO，停止工作。不选择，则一直询问中，一直检测压力开关，如导通，则直接进行下一步，
void PowerOnSelfTestPressureSwitch()
{
    u8 key;
    if( key == 0 )//检测到压力开关是断开的    0表示压力开关是断开的               
    {
        u8 keepworking =0;//默认遇到压力开关断开的时候，直接跳转到E03报警
        E03InsufficientCO2SourcePressure(0);//报警E03;提示“E03气源压力不足可能会造成流量不准或工作不能进行，是否继续？YES NO”
        while( keepworking != YES )//没有选择yes，
        {
            keepworking = ReadDwinKeyValueReturn();//读取dwin屏幕上的按键返回，是否yes按键被按下
            if( keepworking == YES || key == 1 )//一直询问中，一直检测压力开关，如导通，则直接进行下一步//压力开关小图标恢复正常    
            {
                DisableBuzzer();
                if(key == 1)DwinBitIconDisplay(RRESSUREICONADDR,ICONGREEN);//如果是开关导通了就恢复压力开关检测的小图标
                break;
            }
            if( keepworking == MUTE)DisableBuzzer();//关闭报警音
        }
        DwindisplayInterface( MAININTERFACE );//返回主界面
        PrintfText(CLEAR,INFORADRESS);//清除提示信息
        varsendonlyonce=0;
    }
    else
    {}   
}

//使用过程中出现气源压力不足，提示E03 气源压力不足，不提示选择。
void InProcessTestPressureSwitch()
{
    u8 key; 
    if( key == 0 && sendonlyonce == 0 )//检测到压力开关是断开的                   
    {
        E03InsufficientCO2SourcePressure(1);//报警E03;提示“E03气源压力不足”
        DwinBitIconDisplay(RRESSUREICONADDR,ICONDARKGREEN);//小图标变成没有导通的样子
        sendonlyonce = SET;
    }
    else if( key == 1 && sendonlyonce == 1 )
    {
        sendonlyonce =RESET;
        DwinBitIconDisplay(RRESSUREICONADDR,ICONGREEN);//如果是开关导通了就恢复压力开关检测的小图标
        PrintfText(CLEAR,INFORADRESS);//清除提示信息
    }        
}

//读取流量传感器的值，然后和设定的值比较，如果读到的传感器值比设定的小，就曾加占空比！
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
//4.1.2	检测流量传感器：打开自检阀，比例阀以10L/min对应的占空比充气2倍T0-T2最小值， 检测Fa＞0，进行下一步，Fa=0，
//       则报错E01，停止工作，显示 自检未完成；  
void FlowSensorSelfTest()
{
    u16 i;
    u8 cafs300value =0;
    u8 dwinkeyreturnvalue;
    TIM_SetTIM4Compare4(2000);//修改比较值，改变占空比  数值越大，占空比越大 暂时是2000
    EnableProportion();//开启比例阀
    for( i=0;i<T0_T2_Min*2;i++ )
    {
        delay_ms(1);
    }
    cafs300value = Cafs300ReadData();//读取流量传感器的值
    DwinDisplayFlow(cafs300value);
    while( cafs300value == 0)//如果流量传感器的值为0
    {
        E01InflationPipelineFailure();
        dwinkeyreturnvalue = ReadDwinKeyValueReturn();//读取dwin屏幕上的按键返回，是否yes按键被按下
        if(dwinkeyreturnvalue==MUTE)DisableBuzzer();//关闭报警音
    } 
    DisableProportion();//关闭比例阀以两倍T0-T2最小值充气，充气结束
    DwinDisplayVariate(FLOWSENSORADDR,CLEAR);
}


//4.1.3	检测压力传感器：自检阀，放气阀处于关闭状态，比例阀以10L/min  T0-T2最小值充气，检测完压力后打开放气阀，
//      再检测压力，检测完关闭放气阀。检测Pa由高到低变化，则下一步，否则报警E02停止工作，显示 自检未完成；
void PressureSensorSelfTest()
{
    u16 i;
    u16 firstpressure=0;
    u16 secondpressure=0;
    u8 dwinkeyreturnvalue=0;
    TIM_SetTIM4Compare4(1850);//修改比较值，改变占空比  数值越大，占空比越大,比例阀以10L/min充气
    EnableProportion();//开启比例阀
    for(i=0;i<T0_T2_Min;i++)//充气T0_T2_Min时间
    {
        delay_ms(1);            
    }
    DisableProportion();//关闭比例阀以T0-T2最小值充气，充气结束
    firstpressure=PositivePrussure();//此时检测第一次压力
    delay_ms(50);
    secondpressure=PositivePrussure();//此时检测第二次压力
    if(firstpressure-secondpressure<=30)//打开放气阀泄压失败，导致前后两次的压力差不多大
    {
        E02InflationPipelineFailure(); 
        while(1)
        {
            dwinkeyreturnvalue = ReadDwinKeyValueReturn();//读取dwin屏幕上的按键返回，是否yes按键被按下
            if(dwinkeyreturnvalue==MUTE)DisableBuzzer();//关闭报警音
        }
    }
    else   
    {
        DisableBuzzer();//关闭报警音
        DwindisplayInterface( MAININTERFACE );//返回主界面
        PrintfText(CLEAR,INFORADRESS);//清除提示信息
    } 
}

//4.1.4	检测CO2气体：一开机就开始加热传感器，如检测错误，报警 E06 提示： 气体检测错误，请确认是CO2？ YES   NO，
//      选YES 则打开自检阀，显示自检完成。 选NO，停止工作，显示 自检未完成；
void CO2SelfTest()
{
    if((para2flash[2]&0x0001)==1)
    {
                
    } 
    DisableBuzzer();//关闭报警音
    DwindisplayInterface( MAININTERFACE );//返回主界面
    PrintfText(CLEAR,INFORADRESS);//清除提示信息
    SelfChecking( SELF_TEST_COMPLETE );    
}

//4.1.5	检测充气管路，如没有检测到管路，不可充气， 提示安装充气管路，并检测管路，管路寿命100次，最后5次时信息显示
//      框弹提醒准备更换。到100次停止工作，提示更换（检测管路是否原厂，检测次数，开关机一次管路寿命少一次，并要检
//      测管路发热丝和传感器的好坏。
void LoadingLineSelfTest()
{
}








