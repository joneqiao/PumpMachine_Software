#include "PID.h"
#include "parameters.h"

struct PID mypid;
float PWM_out_Vla;     

void Init_Pid(void)
{
    mypid.SetPoint   = pressuresetvalue;
    mypid.Proportion = 30;//1.4625;
    mypid.Integral   = 0;//0.045;//0.027;0.054
    mypid.Derivative = 8;//23.2337
}

float PIDCalc(struct PID *pp, float NextPoint)
{
    float dError, Error;
    Error         = pp->SetPoint - NextPoint;
//    pp->SumError += Error;
    dError        = pp->LastError - pp->PrevError;
    pp->PrevError = pp->LastError;
    pp->LastError = Error;
    return ( pp->Proportion*Error + pp->Integral*pp->SumError + pp->Derivative*dError );                                                                                           
}

//void MotorControl(float Nowtemp)
//{

//    PWM_out_Vla = PIDCalc( &mypid,chuli_current_val );
//	
//	
//        if( tempout>=100 )   tempout = 100;
//        else if( tempout <= 0 )tempout = 0;
//    }
//    else if ( Nowtemp-mypid.SetPoint >= 1 )
//    {
//        tempout = 0;            
//    }
//    else
//        tempout = 100;
//}

//void OilHeatingControl(float Nowtemp)
//{                       
//    if( Nowtemp < mypid.SetPoint-35 )//full speed :if now temperature under set temperature -35
//    {
//        tempout = 80;
//    }
//    else if( mypid.SetPoint-35 <= Nowtemp && Nowtemp <= mypid.SetPoint+35 )
//    {
//        if ( mypid.SetPoint-35 <= Nowtemp && Nowtemp < mypid.SetPoint-25 )
//        {
//            tempout = 0.15*mypid.SetPoint; 
//        }
//        else if ( mypid.SetPoint-25 <= Nowtemp && Nowtemp < mypid.SetPoint-15 )
//        {
//            tempout = 0.12*mypid.SetPoint; 
//        }
//        else if ( mypid.SetPoint-15 <= Nowtemp && Nowtemp < mypid.SetPoint-5 )
//        {
//            tempout = 0.075*mypid.SetPoint; 
//        }
//        else if ( mypid.SetPoint-5  <= Nowtemp && Nowtemp < mypid.SetPoint+1 )
//        {                
//            tempout = PIDCalc( &mypid,Nowtemp );
//            if( tempout >= 100 )   tempout = 80;
//            else if( tempout <= 0 )tempout = 0;                  
//        }
//        if ( Nowtemp-mypid.SetPoint >= 1 )
//        {
//            tempout = 0;            
//        }  
//    }
//    else//stop :if now temperature beyond set temperature + 15
//        tempout = 0;                
//}

//void TempControl( float Nowtemp )
//{
//    if( Nowtemp > tempmax )//if the temperature data is higher than max temperature, turn off the heating
//    {
//        Func_GTJDQ_OFF;
//    }
//    else                   //if the temperature data is lower than max temperature, carry on PID algorithm
//    {
//        switch(Sys.Set.RongYe)
//        {
//            case WATER :WaterHeatingControl(Nowtemp); break;
//            case OIL   :OilHeatingControl(Nowtemp);   break;
//            default:    break;
//        }
//    }
//}

//void PID_PWM( unsigned char  pid_output )
//{
//    static float  i=0;
//	Sys.Sta.PID_pid_output=pid_output;	
//    
//    if( i<=pid_output )
//	{
//		Func_GTJDQ_ON;// heating
//	}
//    else
//	{
//		Func_GTJDQ_OFF;// cooling
//	}
//    
//    i++;
//    if( i>100 )i=0;//every 100 times reset i; according to pid output value calculate pwm
//}

//void vTaskPID(void * x)
//{
//	unsigned short int Timer16s=0;
//	mypid.SetPoint=Sys.Sta.SetTemputer;
//	Func_GTJDQ_OFF;
//	TempControl(Sys.Sta.MainTemputer);
//	
//	while(1)
//	{  
//        switch(Sys.Set.RongYe)
//        {
//            case WATER:Init_Pid_Water();break;//if mdeium is water,so we init p.i.d for water
//            case OIL  :Init_Pid_Oil();  break;//if mdeium is oil,so we init p.i.d for oil  
//            default:break;
//        }
//		mypid.SetPoint=Sys.Sta.SetTemputer;

//        if(Sys.Sta.RunTemp)
//		{
//			if(Timer16s>=100)//4s
//			{
//				Timer16s=0;
//				TempControl(Sys.Sta.MainTemputer);//16S调用
//			}
//			PID_PWM(tempout);//40ms调用
//			Timer16s++;
//		}
//		else
//		{
//			Func_GTJDQ_OFF;
//		}
//		Delay_ms(40);
//	}	
//}

