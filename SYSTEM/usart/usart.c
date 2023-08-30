#include "sys.h"
#include "usart.h"	
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"      //os 使用	  
#include "task.h"
#include "semphr.h"	 
#endif	 
/************************************************
 IRcontroller-HAL库函数版
 技术支持：www.logic-elec.com
 南京洛吉克电子科技有限公司  
 作者：jone @LOGIC-ELEC
************************************************/ 
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
////////////////////////////////////////////////////////////////////////////////// 	  

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 


#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u8 USART_BUF[USART_REC_LEN]; 
//接收状态
//bit15，	接收到5a a5
//bit14，	接收到5a
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  

u8 aRxBuffer[RXBUFFERSIZE];//HAL库使用的串口接收缓冲
UART_HandleTypeDef UART1_Handler; //UART句柄
  
//初始化IO 串口1 
//bound:波特率
void uart_init(u32 bound)
{	
	//UART 初始化设置
	UART1_Handler.Instance=USART1;					    //USART1
	UART1_Handler.Init.BaudRate=bound;				    //波特率
	UART1_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	UART1_Handler.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	UART1_Handler.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位
	UART1_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
	UART1_Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&UART1_Handler);					    //HAL_UART_Init()会使能UART1
	
	HAL_UART_Receive_IT(&UART1_Handler, (u8 *)aRxBuffer, RXBUFFERSIZE);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量
  
}

//UART底层初始化，时钟使能，引脚配置，中断配置
//此函数会被HAL_UART_Init()调用
//huart:串口句柄

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    //GPIO端口设置
	GPIO_InitTypeDef GPIO_Initure;
	
	if(huart->Instance==USART1)//如果是串口1，进行串口1 MSP初始化
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();			//使能GPIOA时钟
		__HAL_RCC_USART1_CLK_ENABLE();			//使能USART1时钟
		__HAL_RCC_AFIO_CLK_ENABLE();
	
		GPIO_Initure.Pin=GPIO_PIN_9;			//PA9
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
		GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
		GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//高速
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA9

		GPIO_Initure.Pin=GPIO_PIN_10;			//PA10
		GPIO_Initure.Mode=GPIO_MODE_AF_INPUT;	//模式要设置为复用输入模式！	
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA10
		
#if EN_USART1_RX
		HAL_NVIC_EnableIRQ(USART1_IRQn);				//使能USART1中断通道
		HAL_NVIC_SetPriority(USART1_IRQn,1,0);			//抢占优先级3，子优先级3
#endif	
	}
}
 
//串口1中断服务程序
//void USART1_IRQHandler(void)                	
//{ 
//	u32 timeout=0;	
//	HAL_UART_IRQHandler(&UART1_Handler);	//调用HAL库中断处理公用函数
//	
//	timeout=0;
//    while (HAL_UART_GetState(&UART1_Handler) != HAL_UART_STATE_READY)//等待就绪
//	{
//	 timeout++;////超时处理
//     if(timeout>HAL_MAX_DELAY) break;		
//	
//	}
//     
//	timeout=0;
//	while(HAL_UART_Receive_IT(&UART1_Handler, (u8 *)aRxBuffer, RXBUFFERSIZE) != HAL_OK)//一次处理完成之后，重新开启中断并设置RxXferCount为1
//	{
//	 timeout++; //超时处理
//	 if(timeout>HAL_MAX_DELAY) break;	
//	}
//} 
//#endif	

/*下面代码我们直接把中断控制逻辑写在中断服务函数内部。*/

extern SemaphoreHandle_t BinarySemaphore;	//二值信号量句柄
//串口1中断服务程序
void USART1_IRQHandler(void)                	
{ 
	u8 Res;
	u8 i;
	BaseType_t xHigherPriorityTaskWoken;
	
	if((__HAL_UART_GET_FLAG(&UART1_Handler,UART_FLAG_RXNE)!=RESET))  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res=USART1->DR;//读取接收到的数据   
        if((USART_RX_STA&0x8000)==0)//没有接收到完整的帧头  5a a5
		{ 
            if(USART_RX_STA&0x4000)//接收到了0x5a
			{
				if(Res!=0xa5)USART_RX_STA=0;//第二个数据不是a5的话，表示接收错误,重新开始
				else 
                {
                    USART_RX_STA|=0x8000;//如果第二个数据也是a5，表示接收到了完整的数据帧头，再次标记标记一下
                    USART_RX_BUF[1]=Res;//同时把第二个数据存在数据缓存数组中
					USART_RX_STA|=0x0001;//表示接下来接受到的数据都开始从USART_RX_BUF[2]往后存储//接收到了5a后又接收到了a5
                }	                                                    
			}
            else//没有接收到0x5a
            {
                if(Res==0x5A)//判断数据帧的开头是不是以5a开头的
                {
                    USART_RX_STA|=0x4000;//如果是以5A开头的，就标记一下
                    USART_RX_BUF[0]=Res;//并将数据存在数据帧缓存中
                }
            }        
        }         
/**********************完整的数据帧头都收到了之后，就要将后续的数据存储下来*************************/ 
		
        if(USART_RX_STA&0x8000)//接收到了5a后又接收到了a5  开始接收后续的数据                        
        {
            USART_RX_BUF[(USART_RX_STA&0X3FFF)]=Res;        //USART_RX_BUF[2]表示后续还有多少个数据等待接收
            USART_RX_STA++;
			
            if((USART_RX_STA&0x1fff)>(USART_RX_BUF[2]+2))//接收的数据多过这帧数据定义的长度，重新开始接收
            {
				for(i=0;i<USART_RX_BUF[2]+3;i++)
				{
				   USART_BUF[i]= USART_RX_BUF[i];
				}
                USART_RX_STA|=0x2000;//完整的数据帧已经接收到了    
			}				
        }
		
		if((USART_RX_STA&0x2000) &&(BinarySemaphore!=NULL) )
		{
			xSemaphoreGiveFromISR(BinarySemaphore,&xHigherPriorityTaskWoken);	//释放二值信号量
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);//如果需要的话进行一次任务切换
		}
	}
} 
#endif	


