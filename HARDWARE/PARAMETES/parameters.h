/*
 * @fileName: 
 * @Author: Jone
 * @LastEditors: Jone
 * @description: page description
 * @Date: 2023-08-30 10:08:09
 * @LastEditTime: 2023-08-30 11:46:16
 */
#ifndef _PARAMETERS_H
#define _PARAMETERS_H

#include "exti.h" 
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "pwm.h"
#include "key.h"
#include "adc.h"
#include "myiic.h"
#include "iwdg.h"
#include "timer.h"
#include "textdisplay.h"
#include "dwindisplay.h"
#include "selftestfuncation.h"
#include "stmflash.h"
#include "ds2502.h"
#include "factorymode.h"
#include "control.h"
#include "buzzer.h"
#include "cmos.h"
#include "PID.h"
#include "stdlib.h"

extern float negative_pressure_current_val;
extern float chuli_current_val;
extern u8 ChuliDispflag;
extern float chulisensorvalue;
extern u8 motogear;
extern float presetnpressure;
extern u16 flowsetvalue;
extern u16 pressuresetvalue;
extern u8 speedupflag;
extern u8 EnterFactorymode;
extern u8 flowmode;
extern u8 xyiconstatue;  
extern u8 bjiconstatue;
extern float PWM_out_Vla;
extern u8 pidcalflag;
extern const u16 FLOW_01L_SPEED;//666
extern const u16 FLOW_02L_SPEED;//333
extern const u16 FLOW_03L_SPEED;//222
extern const u16 FLOW_04L_SPEED;//166
extern const u16 FLOW_05L_SPEED;//133
extern const u16 FLOW_06L_SPEED;//111
extern const u16 FLOW_07L_SPEED;//95
extern const u16 FLOW_08L_SPEED;//83
extern const u16 FLOW_9L_SPEED;//74
extern const u16 FLOW_10L_SPEED;//66
extern const u16 FLOW_11L_SPEED;//60
extern const u16 FLOW_12L_SPEED;//55
extern const u16 FLOW_13L_SPEED;//51

//≤Œ ˝…Ë÷√
#define         MOTORINITPSC            (1000000/1500)//666
#define         NUM                      25

#define         VASION                  "V1.0.0.2212"                                        

#endif

