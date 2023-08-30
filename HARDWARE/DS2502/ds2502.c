#include "ds2502.h"
#include "delay.h"								  
////////////////////////////////////////////////////////////////////////////////// 	
//初始化DS2502的IO口 DQ 同时检测DS的存在
//返回1:不存在
//返回0:存在    	 
u8 DS2502_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOC_CLK_ENABLE();			//开启GPIOC时钟
	
    GPIO_Initure.Pin=GPIO_PIN_10|GPIO_PIN_11;           	//PA0
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//高速
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);     //初始化
 
	DS2502_Rst();
	return DS2502_Check();
}
//复位DS2502
void DS2502_Rst(void)	   
{                 
	DS2502_IO_OUT();   //设置为输出
	DS2502_DQ_OUT=0;  	//拉低DQ
	delay_us(750);      //拉低750us
	DS2502_DQ_OUT=1;  	//DQ=1 
	delay_us(15);       //15US
}
//等待DS2502的回应
//返回1:未检测到DS2502的存在
//返回0:存在
u8 DS2502_Check(void) 	   
{   
	u8 retry=0;
	DS2502_IO_IN();    //设置为输入 
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
//写一个字节到DS2502
//dat：要写入的字节
void DS2502_Write_Byte(u8 dat)     
 {             
    u8 j;
    u8 testb;
    DS2502_IO_OUT();     //设置为输出
    for (j=1;j<=8;j++) 
	{
        testb=dat&0x01;
        dat=dat>>1;
        if(testb)       // 写1
        {
            DS2502_DQ_OUT=0;
            delay_us(2);                            
            DS2502_DQ_OUT=1;
            delay_us(60);             
        }
        else            //写0
        {
            DS2502_DQ_OUT=0;
            delay_us(60);             
            DS2502_DQ_OUT=1;
            delay_us(2);                          
        }
    }
}
//从DS2502读取一个位
//返回值：1/0
u8 DS2502_Read_Bit(void) 
{
	u8 data;
	DS2502_IO_OUT();   //设置为输出
	DS2502_DQ_OUT=0; 
	delay_us(2);
	DS2502_DQ_OUT=1; 
	DS2502_IO_IN();    //设置为输入
	delay_us(12);
	if(DS2502_DQ_IN)data=1;
	else data=0;	 
	delay_us(50);           
	return data;
}
//从DS2502读取一个字节
//返回值：读到的数据
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

//适当的提供12v编程电压
void DS2502_12vPulse()
{
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_SET);
    delay_ms(1000);
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_RESET);
    delay_ms(1000);   
}

//开始DS2502
void DS2502_Start(void)
{   						               
    DS2502_Rst();	   
    DS2502_Check();	 
    DS2502_Write_Byte(0xcc);// skip rom
    DS2502_Write_Byte(0x44);// convert
}









//从DS2502得到温度值
//精度：0.1C
//返回值：温度值 （-550~1250） 
short DS2502_Get_Temp(void)
{
    u8 temp;
    u8 TL,TH;
    short tem;
    DS2502_Start ();           //开始转换
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
        temp=0;//温度为负  
    }else temp=1;//温度为正	  	  
    tem=TH; //获得高八位
    tem<<=8;    
    tem+=TL;//获得底八位
    tem=(double)tem*0.625;//转换     
	if(temp)return tem; //返回温度值
	else return -tem;    
}
