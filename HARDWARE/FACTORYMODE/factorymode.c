#include "factorymode.h"
#include "dwindisplay.h"
#include "pwm.h"
#include "sys.h"
#include "iwdg.h"
#include "adc.h"
#include "selftestfuncation.h"
#include "stmflash.h"
#include "led.h"
#include "key.h"
#include "cmos.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usart.h"
#include "parameters.h"

extern u16 waterhight;

u8 chongxistatue=0;
extern u16 FM_motor_psc;
//               pressure   prelimit    flow
u16 URT_Preset[3]  ={0x0000,0x0000,0x0000};
u16 PCN_Preset[3]  ={0x0000,0x0000,0x0000};
u16 PCN_P_Preset[3]={0x0000,0x0000,0x0000};
u16 UTR_Preset[3]  ={0x0000,0x0000,0x0000};
u16 ART_Preset[3]  ={0x0000,0x0000,0x0000};
u16 CYS_Preset[3]  ={0x0000,0x0000,0x0000};

u16 Adreearry[]    ={0x0090,0x0092,0x0094,
                     0x0096,0x0098,0x009a,
                     0x009c,0x009e,0x0101,
                     0x0103,0x0105,0x0107,
                     0x0109,0x010b,0x010d,
                     0x010f,0x0111,0x0113};                

extern vs16 para2flash[];

void FactoryMode()//进入工厂模式
{	
	if(EXIT_FM_PRESSED)
	{
		ClearUsart_Buf();
		DwindisplayInterface( URT_INTERFACE );
		EnterFactorymode = 0;
		USART_RX_STA = 0;
	}
	
	FM_GetMode_Paramete();
	FMGetNegP_K_B();
	FMGetChuLi_K_B();
	FMGetWaterHight();
	FMNPressureDebug();
	FMChuLiDebug();
	SaveParameters();   
}

void ReadFlashDisplay()
{    
    u8 i;
    u8 j;
	
	taskENTER_CRITICAL();                                 //enter critial
    IWDG_Feed();//操作flash前要喂狗
    STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)para2flash,parameternum);//读取flash中存储的参数
	taskEXIT_CRITICAL();                                //exit critial
	
    for(i=0,j=0;i<3;i++,j++)
    {
        URT_Preset[j]=(u16)para2flash[i];
    }
    for(i=3,j=0;i<6;i++,j++)
    {
        PCN_Preset[j]=(u16)para2flash[i];
    }
    for(i=6,j=0;i<9;i++,j++)
    {
        PCN_P_Preset[j]=(u16)para2flash[i];
    }
    for(i=9,j=0;i<12;i++,j++)
    {
        UTR_Preset[j]=(u16)para2flash[i];
    }
    for(i=12,j=0;i<15;i++,j++)
    {
        ART_Preset[j]=(u16)para2flash[i];
    }
    for(i=15,j=0;i<18;i++,j++)
    {
        CYS_Preset[j]=(u16)para2flash[i];
    } 
    for(i=0;i<18;i++)
    {
        DwinDisplayVariate(Adreearry[i],(u16)para2flash[i]);
        delay_ms(10);
    }
}

void SaveParameters()
{
    u8 i;
    u8 j;
    if(SAVE_FM_PRESSED)
    {
        ClearUsart_Buf();
        for(i=0,j=0;i<3;i++,j++)
        {
            para2flash[i]=(vs16)URT_Preset[j];
        }
        for(i=3,j=0;i<6;i++,j++)
        {
            para2flash[i]=(vs16)PCN_Preset[j];
        }
        for(i=6,j=0;i<9;i++,j++)
        {
            para2flash[i]=(vs16)PCN_P_Preset[j];
        }
        for(i=9,j=0;i<12;i++,j++)
        {
            para2flash[i]=(vs16)UTR_Preset[j];
        }
        for(i=12,j=0;i<15;i++,j++)
        {
            para2flash[i]=(vs16)ART_Preset[j];
        }
        for(i=15,j=0;i<18;i++,j++)
        {
            para2flash[i]=(vs16)CYS_Preset[j];
        }
		
		taskENTER_CRITICAL();                                 //enter critial
        IWDG_Feed();//要喂狗
        STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)para2flash,parameternum);//将压力设定的值存进flash中
		taskEXIT_CRITICAL();
    }
}

void FM_GetMode_Paramete()//5A A5 06 83 00 48 01 00 EC 
{
    u16 address;
    address=(u16)USART_BUF[4]*256+(u16)USART_BUF[5];
    switch(address)
    {
        case 0: break;
        
        case URT_pSet  :URT_Preset[0]= (u16)USART_BUF[7]*256+(u16)USART_BUF[8];ClearUsart_Buf(); break;//DwinDisplayVariate(PRESSURESETADDR,URT_Preset[0]);
        case URT_pLimit:URT_Preset[1]= (u16)USART_BUF[7]*256+(u16)USART_BUF[8];ClearUsart_Buf(); break;
        case URT_fSet  :URT_Preset[2]= (u16)USART_BUF[7]*256+(u16)USART_BUF[8];ClearUsart_Buf(); break;//DwinDisplayVariate(FLOWSETADDR,URT_Preset[2]);
        
        case PCN_pSet  :PCN_Preset[0]= (u16)USART_BUF[7]*256+(u16)USART_BUF[8];ClearUsart_Buf(); break;//DwinDisplayVariate(PRESSURESETADDR,PCN_Preset[0]);
        case PCN_pLimit:PCN_Preset[1]= (u16)USART_BUF[7]*256+(u16)USART_BUF[8];ClearUsart_Buf(); break;
        case PCN_fSet  :PCN_Preset[2]= (u16)USART_BUF[7]*256+(u16)USART_BUF[8];ClearUsart_Buf(); break;//DwinDisplayVariate(FLOWSETADDR,PCN_Preset[2]);
        
        case PCN_P_pSet  :PCN_P_Preset[0]= (u16)USART_BUF[7]*256+(u16)USART_BUF[8];ClearUsart_Buf(); break;//DwinDisplayVariate(PRESSURESETADDR,PCN_P_Preset[0]);
        case PCN_P_pLimit:PCN_P_Preset[1]= (u16)USART_BUF[7]*256+(u16)USART_BUF[8];ClearUsart_Buf(); break;
        case PCN_P_fSet  :PCN_P_Preset[2]= (u16)USART_BUF[7]*256+(u16)USART_BUF[8];ClearUsart_Buf(); break;//DwinDisplayVariate(FLOWSETADDR,PCN_P_Preset[2]);
        
        case UTR_pSet  :UTR_Preset[0]= (u16)USART_BUF[7]*256+(u16)USART_BUF[8];ClearUsart_Buf(); break;//DwinDisplayVariate(PRESSURESETADDR,UTR_Preset[0]);
        case UTR_pLimit:UTR_Preset[1]= (u16)USART_BUF[7]*256+(u16)USART_BUF[8];ClearUsart_Buf(); break;
        case UTR_fSet  :UTR_Preset[2]= (u16)USART_BUF[7]*256+(u16)USART_BUF[8];ClearUsart_Buf(); break;//DwinDisplayVariate(FLOWSETADDR,UTR_Preset[2]);
        
        case ART_pSet  :ART_Preset[0]= (u16)USART_BUF[7]*256+(u16)USART_BUF[8];ClearUsart_Buf(); break;//DwinDisplayVariate(PRESSURESETADDR,ART_Preset[0]);
        case ART_pLimit:ART_Preset[1]= (u16)USART_BUF[7]*256+(u16)USART_BUF[8];ClearUsart_Buf(); break;
        case ART_fSet  :ART_Preset[2]= (u16)USART_BUF[7]*256+(u16)USART_BUF[8];ClearUsart_Buf(); break;//DwinDisplayVariate(FLOWSETADDR,ART_Preset[2]);
        
        case CYS_pSet  :CYS_Preset[0]= (u16)USART_BUF[7]*256+(u16)USART_BUF[8];ClearUsart_Buf(); break;//DwinDisplayVariate(PRESSURESETADDR,CYS_Preset[0]);
        case CYS_pLimit:CYS_Preset[1]= (u16)USART_BUF[7]*256+(u16)USART_BUF[8];ClearUsart_Buf(); break;
        case CYS_fSet  :CYS_Preset[2]= (u16)USART_BUF[7]*256+(u16)USART_BUF[8];ClearUsart_Buf(); break;//DwinDisplayVariate(FLOWSETADDR,CYS_Preset[2]);
        
        default: break;    
    }        
}


void FMNPressureDebug()//negative pressure sensor adjusting
{
    if( XY_FM_PRESSED )//judge whether xiyin motor switch pressed 
    {
        ClearUsart_Buf(); 
        ATTRACT_CMOS =~ ATTRACT_CMOS;
    }
}

void FMChuLiDebug()//chu li sensor adjusting
{
    if( CX_FM_PRESSED )//judge whether chongxi motor switch pressed 
    {
        ClearUsart_Buf(); 
        chongxistatue++;
        if( chongxistatue & 0x01 )
        {
            EnableMotorPWM();
            MotorSpeedUp( FM_motor_psc );//factory mode motor debug,the higher the value, the slower the motor speed 
        } 
        else
            DisableMotorPWM();          
    }
}

void FMGetWaterHight()
{
    u16 waterhightaddress;   
    waterhightaddress = (u16)USART_BUF[4]*256 + (u16)USART_BUF[5];    
    if( WATERHIGHTADD == waterhightaddress )
    {   
        waterhight = (u16)USART_BUF[7]*256 + (u16)USART_BUF[8];      
        ClearUsart_Buf();        
        para2flash[21] = waterhight;    
		
		taskENTER_CRITICAL();  		
        IWDG_Feed();//要喂狗        
        STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)para2flash,parameternum);//将参数存进flash中
		taskEXIT_CRITICAL();  
		
		DwinDisplayVariate(WATERHIGHTADD,waterhight);//send water high set value to water high set address     
    } 
}


extern float chulik;
extern float chulib;
extern float negativepressureK;
extern float negativepressureB;

void FMGetChuLi_K_B()
{
    u16 chulisetaddress;    
    chulisetaddress = (u16)USART_BUF[4]*256 + (u16)USART_BUF[5];    
    if( CHULIKADD == chulisetaddress )
    {   
        chulik = (float)USART_BUF[7]*256 + (float)USART_BUF[8];      
        ClearUsart_Buf();        
        para2flash[22] = (vs16)chulik;   
		
		taskENTER_CRITICAL();  			
        IWDG_Feed();//要喂狗        
        STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)para2flash,parameternum);//将参数存进flash中
		taskEXIT_CRITICAL();  
//		DwinDisplayVariate(WATERHIGHTADD,chulik);//send water high set value to water high set address  
        if( chulik > 10000 ) chulik = 0 - chulik + 10000;
        chulik = (float)chulik/(float)100;        
    } 
    else if( CHULIBADD == chulisetaddress)
    {   
        chulib = (float)USART_BUF[7]*256 + (float)USART_BUF[8];      
        ClearUsart_Buf();        
        para2flash[23] = chulib;  
		
		taskENTER_CRITICAL();  	
        IWDG_Feed();//要喂狗        
        STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)para2flash,parameternum);//将参数存进flash中
		taskEXIT_CRITICAL();  
//		DwinDisplayVariate(WATERHIGHTADD,waterhight);//send water high set value to water high set address   
        if( chulib > 10000 ) chulib = 0 - chulib + 10000;
		chulib = (float)chulib/(float)100;
		
    }      
}

void FMGetNegP_K_B()
{
    u16 negpresetaddress;    
    negpresetaddress = (u16)USART_BUF[4]*256 + (u16)USART_BUF[5];  
    if( NPKADD == negpresetaddress )
    {   
        negativepressureK = (float)USART_BUF[7]*256 + (float)USART_BUF[8];      
        ClearUsart_Buf();        
        para2flash[24] = (vs16)negativepressureK;     
		
	    taskENTER_CRITICAL();  		
        IWDG_Feed();//要喂狗        
        STMFLASH_Write( FLASH_SAVE_ADDR, (u16*)para2flash, parameternum );//将参数存进flash中
		taskEXIT_CRITICAL();                                //exit critial
		
		
//		DwinDisplayVariate(WATERHIGHTADD,chulik);//send water high set value to water high set address  
        if( negativepressureK > 10000 ) negativepressureK = 0 - negativepressureK + 10000;
        negativepressureK = (float)negativepressureK / (float)100;        
    } 
    else if( NPBADD == negpresetaddress )
    {   
        negativepressureB = (float)USART_BUF[7]*256 + (float)USART_BUF[8];      
        ClearUsart_Buf();        
        para2flash[25] = negativepressureB; 

		taskENTER_CRITICAL();  			
        IWDG_Feed();//要喂狗        
        STMFLASH_Write( FLASH_SAVE_ADDR, (u16*)para2flash, parameternum);//将参数存进flash中
		taskEXIT_CRITICAL();                                //exit critial
//		DwinDisplayVariate(WATERHIGHTADD,waterhight);//send water high set value to water high set address   
        if( negativepressureB > 10000 ) negativepressureB = 0 - negativepressureB + 10000;
		negativepressureB = (float)negativepressureB / (float)100;
    }    
}


