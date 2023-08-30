#include "sys.h"
#include "usart.h"	
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"      //os ʹ��	  
#include "task.h"
#include "semphr.h"	 
#endif	 
/************************************************
 IRcontroller-HAL�⺯����
 ����֧�֣�www.logic-elec.com
 �Ͼ��弪�˵��ӿƼ����޹�˾  
 ���ߣ�jone @LOGIC-ELEC
************************************************/ 
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 


#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
u8 USART_BUF[USART_REC_LEN]; 
//����״̬
//bit15��	���յ�5a a5
//bit14��	���յ�5a
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  

u8 aRxBuffer[RXBUFFERSIZE];//HAL��ʹ�õĴ��ڽ��ջ���
UART_HandleTypeDef UART1_Handler; //UART���
  
//��ʼ��IO ����1 
//bound:������
void uart_init(u32 bound)
{	
	//UART ��ʼ������
	UART1_Handler.Instance=USART1;					    //USART1
	UART1_Handler.Init.BaudRate=bound;				    //������
	UART1_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //�ֳ�Ϊ8λ���ݸ�ʽ
	UART1_Handler.Init.StopBits=UART_STOPBITS_1;	    //һ��ֹͣλ
	UART1_Handler.Init.Parity=UART_PARITY_NONE;		    //����żУ��λ
	UART1_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //��Ӳ������
	UART1_Handler.Init.Mode=UART_MODE_TX_RX;		    //�շ�ģʽ
	HAL_UART_Init(&UART1_Handler);					    //HAL_UART_Init()��ʹ��UART1
	
	HAL_UART_Receive_IT(&UART1_Handler, (u8 *)aRxBuffer, RXBUFFERSIZE);//�ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ���������������
  
}

//UART�ײ��ʼ����ʱ��ʹ�ܣ��������ã��ж�����
//�˺����ᱻHAL_UART_Init()����
//huart:���ھ��

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    //GPIO�˿�����
	GPIO_InitTypeDef GPIO_Initure;
	
	if(huart->Instance==USART1)//����Ǵ���1�����д���1 MSP��ʼ��
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();			//ʹ��GPIOAʱ��
		__HAL_RCC_USART1_CLK_ENABLE();			//ʹ��USART1ʱ��
		__HAL_RCC_AFIO_CLK_ENABLE();
	
		GPIO_Initure.Pin=GPIO_PIN_9;			//PA9
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//�����������
		GPIO_Initure.Pull=GPIO_PULLUP;			//����
		GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//����
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//��ʼ��PA9

		GPIO_Initure.Pin=GPIO_PIN_10;			//PA10
		GPIO_Initure.Mode=GPIO_MODE_AF_INPUT;	//ģʽҪ����Ϊ��������ģʽ��	
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//��ʼ��PA10
		
#if EN_USART1_RX
		HAL_NVIC_EnableIRQ(USART1_IRQn);				//ʹ��USART1�ж�ͨ��
		HAL_NVIC_SetPriority(USART1_IRQn,1,0);			//��ռ���ȼ�3�������ȼ�3
#endif	
	}
}
 
//����1�жϷ������
//void USART1_IRQHandler(void)                	
//{ 
//	u32 timeout=0;	
//	HAL_UART_IRQHandler(&UART1_Handler);	//����HAL���жϴ����ú���
//	
//	timeout=0;
//    while (HAL_UART_GetState(&UART1_Handler) != HAL_UART_STATE_READY)//�ȴ�����
//	{
//	 timeout++;////��ʱ����
//     if(timeout>HAL_MAX_DELAY) break;		
//	
//	}
//     
//	timeout=0;
//	while(HAL_UART_Receive_IT(&UART1_Handler, (u8 *)aRxBuffer, RXBUFFERSIZE) != HAL_OK)//һ�δ������֮�����¿����жϲ�����RxXferCountΪ1
//	{
//	 timeout++; //��ʱ����
//	 if(timeout>HAL_MAX_DELAY) break;	
//	}
//} 
//#endif	

/*�����������ֱ�Ӱ��жϿ����߼�д���жϷ������ڲ���*/

extern SemaphoreHandle_t BinarySemaphore;	//��ֵ�ź������
//����1�жϷ������
void USART1_IRQHandler(void)                	
{ 
	u8 Res;
	u8 i;
	BaseType_t xHigherPriorityTaskWoken;
	
	if((__HAL_UART_GET_FLAG(&UART1_Handler,UART_FLAG_RXNE)!=RESET))  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res=USART1->DR;//��ȡ���յ�������   
        if((USART_RX_STA&0x8000)==0)//û�н��յ�������֡ͷ  5a a5
		{ 
            if(USART_RX_STA&0x4000)//���յ���0x5a
			{
				if(Res!=0xa5)USART_RX_STA=0;//�ڶ������ݲ���a5�Ļ�����ʾ���մ���,���¿�ʼ
				else 
                {
                    USART_RX_STA|=0x8000;//����ڶ�������Ҳ��a5����ʾ���յ�������������֡ͷ���ٴα�Ǳ��һ��
                    USART_RX_BUF[1]=Res;//ͬʱ�ѵڶ������ݴ������ݻ���������
					USART_RX_STA|=0x0001;//��ʾ���������ܵ������ݶ���ʼ��USART_RX_BUF[2]����洢//���յ���5a���ֽ��յ���a5
                }	                                                    
			}
            else//û�н��յ�0x5a
            {
                if(Res==0x5A)//�ж�����֡�Ŀ�ͷ�ǲ�����5a��ͷ��
                {
                    USART_RX_STA|=0x4000;//�������5A��ͷ�ģ��ͱ��һ��
                    USART_RX_BUF[0]=Res;//�������ݴ�������֡������
                }
            }        
        }         
/**********************����������֡ͷ���յ���֮�󣬾�Ҫ�����������ݴ洢����*************************/ 
		
        if(USART_RX_STA&0x8000)//���յ���5a���ֽ��յ���a5  ��ʼ���պ���������                        
        {
            USART_RX_BUF[(USART_RX_STA&0X3FFF)]=Res;        //USART_RX_BUF[2]��ʾ�������ж��ٸ����ݵȴ�����
            USART_RX_STA++;
			
            if((USART_RX_STA&0x1fff)>(USART_RX_BUF[2]+2))//���յ����ݶ����֡���ݶ���ĳ��ȣ����¿�ʼ����
            {
				for(i=0;i<USART_RX_BUF[2]+3;i++)
				{
				   USART_BUF[i]= USART_RX_BUF[i];
				}
                USART_RX_STA|=0x2000;//����������֡�Ѿ����յ���    
			}				
        }
		
		if((USART_RX_STA&0x2000) &&(BinarySemaphore!=NULL) )
		{
			xSemaphoreGiveFromISR(BinarySemaphore,&xHigherPriorityTaskWoken);	//�ͷŶ�ֵ�ź���
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);//�����Ҫ�Ļ�����һ�������л�
		}
	}
} 
#endif	


