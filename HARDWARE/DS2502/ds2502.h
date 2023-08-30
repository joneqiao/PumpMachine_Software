#ifndef __DS2502_H
#define __DS2502_H
#include "sys.h"

//IO��������
#define DS2502_IO_IN()  {GPIOC->CRH&=0XFFFF0FFF;GPIOC->CRH|=8<<12;}
#define DS2502_IO_OUT() {GPIOC->CRH&=0XFFFF0FFF;GPIOC->CRH|=3<<12;}

//IO��������											   
#define	DS2502_DQ_OUT PCout(11) //���ݶ˿�	PC11
#define	DS2502_DQ_IN  PCin(11)  //���ݶ˿�	PC11


u8 DS2502_Init(void);			//��ʼ��DS2502
void DS2502_Write_Byte(u8 dat); //д��һ���ֽ�
u8 DS2502_Read_Byte(void);		//����һ���ֽ�
void DS2502_Rst(void);			//��λDS2502 
u8 DS2502_Check(void);			//����Ƿ����DS2502
u8 DS2502_Read_Bit(void);		//����һ��λ
void DS2502_12vPulse(void);         //д��ʱ����Ҫһ��12v�ĸ�����


short DS2502_Get_Temp(void);	//��ȡ�¶�
void DS2502_Start(void);		//��ʼ�¶�ת��














#endif
