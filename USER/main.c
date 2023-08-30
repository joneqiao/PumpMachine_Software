#include "parameters.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
/************************************************
proportional-HAL
technical suppotr��www.logic-elec.com
nanjing logic-electronic technical company  
writer��jone @LOGIC-ELEC
************************************************/ 
//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		128  
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

//�������ȼ�
#define REAL_TIME_TASK_PRIO		2
//�����ջ��С	
#define REAL_TIME_STK_SIZE 		500  
//������
TaskHandle_t REAL_TIME_Task_Handler;
//������
void real_time_task(void *pvParameters);

//�������ȼ�
#define DELAY_TASK_PRIO		 3
//�����ջ��С	
#define DELAY_STK_SIZE 		500  
//������
TaskHandle_t DELAY_Task_Handler;
//������
void delay_task(void *pvParameters);


//�������ȼ�
#define COMM_TASK_PRIO		4
//�����ջ��С	
#define COMM_STK_SIZE 		500  
//������
TaskHandle_t COMMTask_Handler;
//������
void comm_task(void *pvParameters);

SemaphoreHandle_t BinarySemaphore;	//��ֵ�ź������
TimerHandle_t 	AutoReloadTimer_Handle;			          //Cycle timer handle
void AutoReloadCallback(TimerHandle_t xTimer); 	        //The periodic timer callback function


int main(void)
{ 	
    HAL_Init();                    	 	            //init HAL library   
    Stm32_Clock_Init(RCC_PLL_MUL9);   	            //set clock,72M
    delay_init(72);               		            //init delay function
    MotorInit();                                    //init motor
    LED_Init();							            //init LED	
    MOS_Init();                                     //init MOS control DC moto
    KEY_Init();							            //init key
	EXTI_Init();						            //init exit interrupt
    BUZZER_Init();                                  //init buzzer 
//    IWDG_Init(IWDG_PRESCALER_64,500);  	            //the fractional frequency is 64, the overload value is 500, and the overflow time is 1s   
    TIM3_Init(5000-1,7200-1); 	                    //init timer3, timer3 clock 72M, frequency divison coefficien 7200-1,frequency 72M/7200=10K, automatic reload is 5000-1, timer cycle is 500ms,feed watchdog 
	uart_init(115200);					            //init Usart
	ADC_Init();                  	                //init ADC1 channel 10,11      
    TIM2_Init(10000-1,7200-1); 	                    //init timer2, timer2 clock 72M,frequency divison coefficien 7200-1,frequency 72M/7200=10K, automatic reload is10000-1, timer cycle is 1000ms
    TIM4_PWM_Init(65535-1,72-1);		            //72M/72=1M fractional frequency��reload = 2500��so PWM frequency 1M/2500=400HZ
//    TIM4_CH4_Cap_Init(2500-1,72-1);		            //frequency 1Mhz count
    delay_ms(6000);                                 //delay 1s then init display, in case some parameters not init
    DwinDisplayInit();                              //initial display
     
	//������ʼ����
    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������              
    vTaskStartScheduler();          //�����������
				
}

void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
	
	//������ֵ�ź���
	BinarySemaphore=xSemaphoreCreateBinary();	
	
	//Create a software cycle timer, cycle time 50ms (1000 ticks )
    AutoReloadTimer_Handle=xTimerCreate((const char* )"AutoReloadTimer",
									    (TickType_t  )50,
							            (UBaseType_t )pdTRUE,
							            (void*		 )1,
							            (TimerCallbackFunction_t)AutoReloadCallback); // Cycle timer, cycle 1s(1000 ticks), cycle mode
																				
    //����real_time_task����
    xTaskCreate((TaskFunction_t )real_time_task,     	
                (const char*    )"real_time_task",   	
                (uint16_t       )REAL_TIME_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )REAL_TIME_TASK_PRIO,	
                (TaskHandle_t*  )&REAL_TIME_Task_Handler);   
                
    //����delay_task����
    xTaskCreate((TaskFunction_t )delay_task,     
                (const char*    )"delay_task",   
                (uint16_t       )DELAY_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )DELAY_TASK_PRIO,
                (TaskHandle_t*  )&DELAY_Task_Handler);   
				
	//����COMM����
    xTaskCreate((TaskFunction_t )comm_task,     
                (const char*    )"comm_task",   
                (uint16_t       )COMM_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )COMM_TASK_PRIO,
                (TaskHandle_t*  )&COMMTask_Handler); 
				
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
				
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//real_time������ 
void real_time_task(void *pvParameters)
{
	Init_Pid();
	
	if( AutoReloadTimer_Handle != NULL )
    {
        xTimerStart( AutoReloadTimer_Handle, 0 );	        // Start the cycle timer use to collect adc data
    } 
	
    while(1)
    {
		FeetSwitch();
		MotorControl();  
		XiYinControl();
//		PressureValHandle();  
		taskYIELD();
    }
}   

extern u8 chulilongpressbuzzerflag;
//delay������
void delay_task(void *pvParameters)
{
    while(1)
    {
		LED0 = ~LED0;
		ChuliDisplay( chuli_current_val );
		NegativeDisplay();
		MotorAlarm();
		if(flowmode)
		{
			FlowDisplay( 1, motogear );
		}
		else
		{
			FlowDisplay(0,0); 
		}

		if (EnterFactorymode)
		{
			DwinDisplayVariate(CLDISADDR, (u16)chuli_current_val);   // display chu li sensor value
			if (xyiconstatue & 0x00 && JiaoTaFlag == RESET)     	 // attract pump disable
			{
				DwinDisplayVariate(NPDISADDR, (u16)presetnpressure); // display negative pressure set value
			}
			else// attract pump enable
			{
				DwinDisplayVariate(NPDISADDR, (u16)negative_pressure_current_val); // display negative pressure sensor value
			}
		}
		if(chulilongpressbuzzerflag)
        {
            BUZZER = 1;
            delay_ms(200);
            BUZZER = 0;
            chulilongpressbuzzerflag = 0;
        }
		delay_ms(500);
    }
}

//COMM������
void comm_task(void *pvParameters)
{
	BaseType_t err = pdFALSE;
	
    while(1)
    {
		if(BinarySemaphore != NULL)
		{
			err = xSemaphoreTake(BinarySemaphore,portMAX_DELAY);	//��ȡ�ź���
			if(err == pdTRUE)										//��ȡ�ź����ɹ�
			{
				ReadDwinKeyValueReturn();
				USART_RX_STA=0;
			}
		}
		else if(err == pdFALSE)
		{
			delay_ms(1);
		}
    }
}

// Periodic timer callback function
void AutoReloadCallback(TimerHandle_t xTimer)
{
	static u8 timecount500 = 0;
	static u8 timeconut250 = 0;
	
	if(timecount500 < 0xff)
	{
		timecount500++;
		if(timecount500 >= 10)//50*10
		{
			LED_RED = ~LED_RED;
			timecount500 = 0;
		}
	}
	
	if(timeconut250 < 0xff)
	{
		timeconut250++;
		if(timeconut250 >= 5)//50*5
		{
			timeconut250 = 0;
			negative_pressure_current_val = NegativePrussure();
			LED1 = ~LED1;
		}
	}
    chuli_current_val = KalmanFilter( filter() );
}

