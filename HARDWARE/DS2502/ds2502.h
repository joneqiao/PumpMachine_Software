#ifndef __DS2502_H
#define __DS2502_H
#include "sys.h"

//IO方向设置
#define DS2502_IO_IN()  {GPIOC->CRH&=0XFFFF0FFF;GPIOC->CRH|=8<<12;}
#define DS2502_IO_OUT() {GPIOC->CRH&=0XFFFF0FFF;GPIOC->CRH|=3<<12;}

//IO操作函数											   
#define	DS2502_DQ_OUT PCout(11) //数据端口	PC11
#define	DS2502_DQ_IN  PCin(11)  //数据端口	PC11


u8 DS2502_Init(void);			//初始化DS2502
void DS2502_Write_Byte(u8 dat); //写入一个字节
u8 DS2502_Read_Byte(void);		//读出一个字节
void DS2502_Rst(void);			//复位DS2502 
u8 DS2502_Check(void);			//检测是否存在DS2502
u8 DS2502_Read_Bit(void);		//读出一个位
void DS2502_12vPulse(void);         //写的时候需要一个12v的高脉冲


short DS2502_Get_Temp(void);	//获取温度
void DS2502_Start(void);		//开始温度转换














#endif
