#include "parameters.h"

float negative_pressure_current_val;
float chuli_current_val;
u8 ChuliDispflag;
u8 pidcalflag;
u8 EnterFactorymode = 0;
u16 presetnpressure = 400;           //negative pressure threshold value 

const float MXP2100offset       = 1000;          //点位器值
const float MXP2100sensitivity  = 0.4;           
const float FSS1500offset       = 1000;          //点位器值

float chulik              = 1;       //chuli sensor k  //传感器值和表显示值建立函数关系     
float chulib              = 0;       //chuli sensor b

float negativepressureK   = 1;        //negative pressure k
float negativepressureB   = 0;       //negative pressure b


const u16 FM_motor_psc          =1000000/1500;  //factory mode, try to find relationship between flow and motor speed                                      

const u16 FLOW_01L_SPEED_CW     =1000000/200;//5000    //反转的转速
const u16 FLOW_01L_SPEED_DOWN   =1000000/200;    //接近目标值时的减速1000000/1600; 

const u16 FLOW_01L_SPEED       =1000000/1166.7;    //857.1  			//666//1000000/1500
const u16 FLOW_02L_SPEED       =1000000/2900;      //344.8      		//333//1000000/3000
const u16 FLOW_03L_SPEED       =1000000/3666.65;   //272.7				//222//1000000/4500
const u16 FLOW_04L_SPEED       =1000000/4999.98;   //200.0				//166//1000000/6000
const u16 FLOW_05L_SPEED       =1000000/6499.98;   //151.5				//133//1000000/7500
const u16 FLOW_06L_SPEED       =1000000/8166.65;   //122.4				//111//1000000/9000
const u16 FLOW_07L_SPEED       =1000000/9583.31;   //104.3				//95//1000000/10500
const u16 FLOW_08L_SPEED       =1000000/11500;	   //86.9				//83//1000000/12000
const u16 FLOW_9L_SPEED        =1000000/13500;     //74					//74//1000000/13500
const u16 FLOW_10L_SPEED       =1000000/15500;	   //64					//66//1000000/15000
const u16 FLOW_11L_SPEED       =1000000/17166.68;  //58					//60//1000000/16500
const u16 FLOW_12L_SPEED       =1000000/19333.36;  //51					//55//1000000/18000
const u16 FLOW_13L_SPEED       =1000000/21500;     //46					//51//1000000/19500
