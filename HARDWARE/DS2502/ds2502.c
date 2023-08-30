#include "ds2502.h"
#include "delay.h"								  
////////////////////////////////////////////////////////////////////////////////// 	
//��ʼ��DS2502��IO�� DQ ͬʱ���DS�Ĵ���
//����1:������
//����0:����    	 
u8 DS2502_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOC_CLK_ENABLE();			//����GPIOCʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_10|GPIO_PIN_11;           	//PA0
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//����
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);     //��ʼ��
 
	DS2502_Rst();
	return DS2502_Check();
}
//��λDS2502
void DS2502_Rst(void)	   
{                 
	DS2502_IO_OUT();   //����Ϊ���
	DS2502_DQ_OUT=0;  	//����DQ
	delay_us(750);      //����750us
	DS2502_DQ_OUT=1;  	//DQ=1 
	delay_us(15);       //15US
}
//�ȴ�DS2502�Ļ�Ӧ
//����1:δ��⵽DS2502�Ĵ���
//����0:����
u8 DS2502_Check(void) 	   
{   
	u8 retry=0;
	DS2502_IO_IN();    //����Ϊ���� 
    while (DS2502_DQ_IN&&retry<200)
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=200)return 1;
	else retry=0;
    while (!DS2502_DQ_IN&&retry<240)
	{
		retry++;
		delay_us(1);
	};
	if(retry>=240)return 1;	    
	return 0;
}
//дһ���ֽڵ�DS2502
//dat��Ҫд����ֽ�
void DS2502_Write_Byte(u8 dat)     
 {             
    u8 j;
    u8 testb;
    DS2502_IO_OUT();     //����Ϊ���
    for (j=1;j<=8;j++) 
	{
        testb=dat&0x01;
        dat=dat>>1;
        if(testb)       // д1
        {
            DS2502_DQ_OUT=0;
            delay_us(2);                            
            DS2502_DQ_OUT=1;
            delay_us(60);             
        }
        else            //д0
        {
            DS2502_DQ_OUT=0;
            delay_us(60);             
            DS2502_DQ_OUT=1;
            delay_us(2);                          
        }
    }
}
//��DS2502��ȡһ��λ
//����ֵ��1/0
u8 DS2502_Read_Bit(void) 
{
	u8 data;
	DS2502_IO_OUT();   //����Ϊ���
	DS2502_DQ_OUT=0; 
	delay_us(2);
	DS2502_DQ_OUT=1; 
	DS2502_IO_IN();    //����Ϊ����
	delay_us(12);
	if(DS2502_DQ_IN)data=1;
	else data=0;	 
	delay_us(50);           
	return data;
}
//��DS2502��ȡһ���ֽ�
//����ֵ������������
u8 DS2502_Read_Byte(void)   
{        
	u8 i,j,dat;
	dat=0;
	for (i=1;i<=8;i++) 
	{
        j=DS2502_Read_Bit();
        dat=(j<<7)|(dat>>1);
    }						    
	return dat;
}

//�ʵ����ṩ12v��̵�ѹ
void DS2502_12vPulse()
{
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_SET);
    delay_ms(1000);
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_RESET);
    delay_ms(1000);   
}

//��ʼDS2502
void DS2502_Start(void)
{   						               
    DS2502_Rst();	   
    DS2502_Check();	 
    DS2502_Write_Byte(0xcc);// skip rom
    DS2502_Write_Byte(0x44);// convert
}









//��DS2502�õ��¶�ֵ
//���ȣ�0.1C
//����ֵ���¶�ֵ ��-550~1250�� 
short DS2502_Get_Temp(void)
{
    u8 temp;
    u8 TL,TH;
    short tem;
    DS2502_Start ();           //��ʼת��
    DS2502_Rst();
    DS2502_Check();	 
    DS2502_Write_Byte(0xcc);   // skip rom
    DS2502_Write_Byte(0xbe);   // convert	    
    TL=DS2502_Read_Byte();     // LSB   
    TH=DS2502_Read_Byte();     // MSB   
    if(TH>7)
    {
        TH=~TH;
        TL=~TL; 
        temp=0;//�¶�Ϊ��  
    }else temp=1;//�¶�Ϊ��	  	  
    tem=TH; //��ø߰�λ
    tem<<=8;    
    tem+=TL;//��õװ�λ
    tem=(double)tem*0.625;//ת��     
	if(temp)return tem; //�����¶�ֵ
	else return -tem;    
}
