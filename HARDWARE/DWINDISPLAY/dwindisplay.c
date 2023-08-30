#include "parameters.h"
#include "string.h"

extern float chulik;
extern float chulib;
extern float negativepressureK;
extern float negativepressureB;
    

extern u8 getpressureflag;  
extern u8 getflowflag;
extern u8 motogear;
extern u8 USART_BUF[];       //receive buffer
extern u8 xiyinflag;
extern u8 bujinflag;

extern u8 xiyinonceflag;
extern u16 URT_Preset[3];

u16 runmode;
u16 pressuresetvalue;
u16 pressuresetlimt;
u16 flowsetvalue;
u8 xyiconstatue=0;  
u8 bjiconstatue=0;
u16 waterhight=0;
                                //0     1       2      3      4     5     6       7      8      9      10     11     12     13     14     15    16      17    18     19      20     21     22CK    23CB     24NK     25NB  26attract pump setvalue
vs16 para2flash[parameternum]={0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0050,0x0000,0x0000,0x0000,0x0001,0x0000,0x0001,0x0000,0x0190};//parameters need to save in flash
u8 interfacediarray[] = {0x5a,0xa5,0x04,0x80,0x03,0x00,0x00};//0x04,0x80,0x03//5A A5 07 82 0084 5A01 0001
u8 variatearray[]     = {0x5a,0xa5,0x05,0x82,0x00,0x00,0x00,0x00}; 
u8 iconarray[]        = {0x5a,0xa5,0x05,0x82,0x00,0x00,0x00,0x00};
u8 columnarray[]      = {0x5a,0xa5,0x13,0x82,0x00,0x00,0x00,0x04,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00};
u8 readarrary[]       = {0x5a,0xa5,0x04,0x83,0x00,0x00,0x01};
u8 textarray[]        = {0x5a,0xa5,0x54,0x82,0x00,0x00,0x00,0x00,0x00,0x0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; 
    //example: 5A A5 13 82 5440 0004 0001 011E 012C 01AA 018C F800 FF00
//0x0004 矩形填充
//0x0001 填充一个矩形 
//011E 012C 左上坐标
//01AA 018C 右下坐标  
//F800 颜色       
void DwinDisplayInit()                              //initial display 
{
   
    DwinBitIconDisplay(XYICONADDR,0);
    DwinBitIconDisplay(BJICONADDR,0);
    IWDG_Feed();//feed watch dog befor read data from flash
    STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)para2flash,parameternum);//read parameters from flash 

    pressuresetvalue = para2flash[19];
    flowsetvalue     = para2flash[20];
	motogear = flowsetvalue;//将流量设置暂存到一个变量中
    waterhight       = para2flash[21];
    chulik           =(float) para2flash[22]/100;
    chulib           =(float) para2flash[23]/100;
    negativepressureK=(float) para2flash[24]/100;
    negativepressureB=(float) para2flash[25]/100;

	presetnpressure  =(float) para2flash[26];//read attract pump set value from flash
	RANGE_VARIABLE(0,600,presetnpressure);
   
    switch(para2flash[18])
    {
        case URT_ICON:      DwindisplayInterface(URT_INTERFACE);DwinBitIconDisplay(URT_ICON,0); pressuresetlimt =para2flash[1];  	break;
        case PCN_ICON:      DwindisplayInterface(PCN_INTERFACE);DwinBitIconDisplay(PCN_ICON,0); pressuresetlimt =para2flash[4];     break;
        case PCN_P_ICON:    DwindisplayInterface(PCN_P_INTERFACE);DwinBitIconDisplay(PCN_P_ICON,0); pressuresetlimt =para2flash[7]; break;
        case UTR_ICON:      DwindisplayInterface(UTR_INTERFACE);DwinBitIconDisplay(UTR_ICON,0); pressuresetlimt =para2flash[10];    break;
        case ART_ICON:      DwindisplayInterface(ART_INTERFACE);DwinBitIconDisplay(ART_ICON,0); pressuresetlimt =para2flash[13];    break;
        case CYS_ICON:      DwindisplayInterface(CYS_INTERFACE);DwinBitIconDisplay(CYS_ICON,0); pressuresetlimt =para2flash[16];    break;   
        default:break;
    }
    
    DwinDisplayText(VERSIONADD);
	DwinDisplayVariate(WATERHIGHTADD,waterhight);//send water high set value to water high set address  
    DwinDisplayVariate(PRESSURESETADDR,pressuresetvalue);//display pressure set value     
    DwinDisplayVariate(FLOWSETADDR,flowsetvalue);//display flow set value  
    DwinDisplayVariate(PRESSUREKEYADDR,pressuresetvalue);//send pressure set value to pressure set address
    DwinDisplayVariate(FLOWKEYADDR,flowsetvalue);//send flow set value to flow set address 
    
    DwinDisplayVariate(CHULIKADD,chulik*100);//send chuli k value to flow set address 
    DwinDisplayVariate(CHULIBADD,chulib*100);//send chuli b value to flow set address 
    DwinDisplayVariate(NPKADD,negativepressureK*100);//send negativepressure K value to flow set address 
    DwinDisplayVariate(NPBADD,negativepressureB*100);//send negativepressure B value to flow set address   
} 

void DwindisplayInterface( u16 id )//interface id
{
    interfacediarray[5]=(u8)((id&0xff00)>>8);
    interfacediarray[6]=(u8)((id&0x00ff)); 
    HAL_UART_Transmit(&UART1_Handler,(uint8_t*)interfacediarray,7,1000); //send 7 data
    while(__HAL_UART_GET_FLAG(&UART1_Handler,UART_FLAG_TC)!=SET);	     //wait send finish 
}

void DwinDisplayVariate(u16 displayaddr,u16 displayvalue)//display variate
{
    variatearray[4]=(u8)((displayaddr&0xff00)>>8);
    variatearray[5]=(u8)((displayaddr&0x00ff)); 
    variatearray[6]=(u8)((displayvalue&0xff00)>>8);
    variatearray[7]=(u8)((displayvalue&0x00ff)); 
    HAL_UART_Transmit(&UART1_Handler,(uint8_t*)variatearray,8,1000);//send data array
    while(__HAL_UART_GET_FLAG(&UART1_Handler,UART_FLAG_TC)!=SET);//wait send finish 
}

void DwinBitIconDisplay(u16 iconadrress, u16 biticonid)//icon display ...maybe have some more easier way to do this
{
    iconarray[4]=(u8)((iconadrress&0xff00)>>8);
    iconarray[5]=(u8)((iconadrress&0x00ff)); 
    iconarray[6]=(u8)((biticonid&0xff00)>>8);
    iconarray[7]=(u8)((biticonid&0x00ff)); 
    HAL_UART_Transmit(&UART1_Handler,(uint8_t*)iconarray,8,1000);//send array
    while(__HAL_UART_GET_FLAG(&UART1_Handler,UART_FLAG_TC)!=SET);//wait send finish 
}


void ColumnDisplay(u16 columnadrress,u16 upleftx,u16 uplefty,u16 downrightx,u16 downrighty,u16 colour)
{
//                              0    1    2    3    4    5    6     7   8    9    10---11   12---13   14---15   16---17   18---19  
//    u8 columnarray[]      = {0x5a,0xa5,0x13,0x82,0x00,0x00,0x00,0x04,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00};            
    columnarray[4]=(u8)((columnadrress&0xff00)>>8);
    columnarray[5]=(u8)((columnadrress&0x00ff));
       
    columnarray[10]=(u8)((upleftx&0xff00)>>8);
    columnarray[11]=(u8)((upleftx&0x00ff));
    columnarray[12]=(u8)((uplefty&0xff00)>>8);
    columnarray[13]=(u8)((uplefty&0x00ff));   
    columnarray[14]=(u8)((downrightx&0xff00)>>8);
    columnarray[15]=(u8)((downrightx&0x00ff));
    columnarray[16]=(u8)((downrighty&0xff00)>>8);
    columnarray[17]=(u8)((downrighty&0x00ff));
    columnarray[18]=(u8)((colour&0xff00)>>8);
    columnarray[19]=(u8)((colour&0x00ff));

    HAL_UART_Transmit(&UART1_Handler,(uint8_t*)columnarray,22,1000);//send array
    while(__HAL_UART_GET_FLAG(&UART1_Handler,UART_FLAG_TC)!=SET);//wait send finish 
}

void DwinReadVariate(u16 readaddr)//read variate
{
    readarrary[4]=(u8)((readaddr&0xff00)>>8);
    readarrary[5]=(u8)((readaddr&0x00ff));
    HAL_UART_Transmit(&UART1_Handler,(uint8_t*)readarrary,7,1000);//send data array
    while(__HAL_UART_GET_FLAG(&UART1_Handler,UART_FLAG_TC)!=SET);//wait send finish 
}

void DwinSetVariate()//set and display variate 
{
	if( USART_RX_STA&0x2000) 
	{
		if( PRESSUREPRESS )//if pressure any set button pressed 
		{   
			pressuresetvalue = USART_BUF[7]*256 + USART_BUF[8];
			if( pressuresetvalue < 30 )pressuresetvalue = 30;
			if( pressuresetvalue > 500 )pressuresetvalue = 500;
			if( pressuresetvalue >= pressuresetlimt )
			{
				pressuresetvalue = pressuresetlimt;
				DwinDisplayVariate( PRESSUREKEYADDR, pressuresetlimt );//send flow set value to flow set address  
			}            
			DwinDisplayVariate( PRESSURESETADDR, pressuresetvalue );//display pressure set value 
			ClearUsart_Buf(); 
			USART_RX_STA=0;
		}
		if (FLOWPRESS) // if flow any set button pressed
		{
			flowsetvalue = USART_BUF[7]*256 + USART_BUF[8];
			motogear = flowsetvalue;
			DwinDisplayVariate( FLOWSETADDR,flowsetvalue );//display flow set value 
			ClearUsart_Buf(); 
			USART_RX_STA=0;
		}
		if (ATTRACT_PUMP_SETVAL_ADDRESS) // get the attract pump set value massage
		{
			presetnpressure = (u16)USART_BUF[7] * 256 + (u16)USART_BUF[8];
			RANGE_VARIABLE(0, 600, presetnpressure);
			DwinDisplayVariate(NPDISADDR, (u16)presetnpressure);
			ClearUsart_Buf();
			STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)para2flash,parameternum);//将参数存进flash中
		}
	}    
}

u8 chulilongpressbuzzerflag;
extern float value_buf[NUM];
extern float speedoffset;
float chulilongconutnew = 0;
float chulilongconutold = 0;
void ReadDwinKeyValueReturn()//read dwin display keyvalue return 
{	
    DwinSetVariate();//set and display pressure or flow set value
	
	if(USART_RX_STA&0x2000)
	{
		if( FACTORMODE )//enter factory mode 
		{
			ClearUsart_Buf();              
			DwindisplayInterface( FMINTERFACT );//jump interface to factory mode 
			ReadFlashDisplay();
			EnterFactorymode = 1;
			USART_RX_STA = 0;
		} 
		if( WATERHIGHTMODE )
		{			
			ClearUsart_Buf();              
			DwindisplayInterface( WATER_HEIGHT_INTERFACE );//jump interface to water height set mode 
			ReadFlashDisplay();
			EnterFactorymode = 1;
			USART_RX_STA = 0;
		}
		if( CHULILONGPRESS )//if chuli sensor long time pressed 
		{
            chulilongconutnew = (float)USART_BUF[7]*256 + (float)USART_BUF[8];
            ClearUsart_Buf(); 
			if(chulilongconutnew - chulilongconutold > 30)//long time pressed lasts 3 second   one message need 150ms
			{
                chulilongconutold = chulilongconutnew;
                chulilongpressbuzzerflag = 1;
				if(chuli_current_val > 0)
                {
					chulib = chulib - chuli_current_val;
                    
                }                    
                if(chulilongconutnew >= 30)
                { 
                    chulilongconutnew = 0;
                    DwinDisplayVariate(0x0120,0);
                }
//                if(chulilongconutold >= 30)
//                {
                    chulilongconutold = 0;
//                    DwinDisplayVariate(0x0120,0);
//                }     
			}
//            if(chulilongconutnew >= 30)
//            { 
//                chulilongconutnew = 0;
//                DwinDisplayVariate(0x0120,0);
//            }
//            if(chulilongconutold >= 30)
//            {
//                chulilongconutold = 0;
//                DwinDisplayVariate(0x0120,0);
//            } 
			USART_RX_STA = 0;
		}
		if((XYSTARTPRESS)||xiyinflag)//xiyin start button pressed 
		{
			if(XYSTARTPRESS)USART_RX_STA = 0;
			xyiconstatue++;
			xiyinflag=RESET;
			ClearUsart_Buf();
			xiyinonceflag=RESET;
			DwinBitIconDisplay(XYICONADDR,xyiconstatue&0x01);//display xiyin icon         
		} 
		if((BJSTARTPRESS)||bujinflag)//bujin start button pressed 
		{
            
			if(BJSTARTPRESS)USART_RX_STA = 0;
			bjiconstatue++;
			bujinflag=RESET;
			ClearUsart_Buf();
			memset(value_buf,0,NUM);	
			DwinBitIconDisplay(BJICONADDR,bjiconstatue&0x01);//display bujin icon  
			para2flash[19]= pressuresetvalue;
			para2flash[20]= flowsetvalue;
			IWDG_Feed();//feed watch dog befor read data from flash
			STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)para2flash,parameternum);//read parameters from flash  
            if(bjiconstatue&0x01==0)
                speedoffset=0;   
		}
		
		FactoryMode();
	}
    ModeSelect();    
	USART_RX_STA = 0;
}



void ClearUsart_Buf()
{
    u8 i;
    for(i=0;i<20;i++)
    {
        USART_BUF[i]=0; 
    }
}

void ModeSelect()
{
	if(USART_RX_STA&0x2000)
	{
		if(URT_PRESS)//urt mode button pressed 
		{
			ClearUsart_Buf();              
			DwinBitIconDisplay(URT_ICON,0);//display bujin icon 
			pressuresetvalue=para2flash[0];
			pressuresetlimt =para2flash[1];
			flowsetvalue    =para2flash[2];
			para2flash[18]   = URT_ICON;
			DwinDisplayVariate(PRESSURESETADDR,para2flash[0]);//display pre_pressure set 
			DwinDisplayVariate(FLOWSETADDR,para2flash[2]);//display pre_flow set
			DwinDisplayVariate(PRESSUREKEYADDR,para2flash[0]);//send pressure set value to pressure set address
			DwinDisplayVariate(FLOWKEYADDR,para2flash[2]);//send flow set value to flow set address     
			USART_RX_STA = 0;
		}
		if(PCN_PRESS)//pcn mode button pressed 
		{
			ClearUsart_Buf();              
			DwinBitIconDisplay(PCN_ICON,0);//display bujin icon 
			pressuresetvalue=para2flash[3];
			pressuresetlimt =para2flash[4];
			flowsetvalue    =para2flash[5];
			para2flash[18]   = PCN_ICON;
			DwinDisplayVariate(PRESSURESETADDR,para2flash[3]);//display pre_pressure set 
			DwinDisplayVariate(FLOWSETADDR,para2flash[5]);//display pre_flow set
			DwinDisplayVariate(PRESSUREKEYADDR,para2flash[3]);//send pressure set value to pressure set address
			DwinDisplayVariate(FLOWKEYADDR,para2flash[5]);//send flow set value to flow set address   
			USART_RX_STA = 0;			
		} 
		if(PCN_P_PRESS)//pcn_p mode button pressed 
		{
			ClearUsart_Buf();              
			DwinBitIconDisplay(PCN_P_ICON,0);//display bujin icon 
			pressuresetvalue=para2flash[6];
			pressuresetlimt =para2flash[7];
			flowsetvalue    =para2flash[8];
			para2flash[18]   = PCN_P_ICON;
			DwinDisplayVariate(PRESSURESETADDR,para2flash[6]);//display pre_pressure set       
			DwinDisplayVariate(FLOWSETADDR,para2flash[8]);//display pre_flow set
			DwinDisplayVariate(PRESSUREKEYADDR,para2flash[6]);//send pressure set value to pressure set address
			DwinDisplayVariate(FLOWKEYADDR,para2flash[8]);//send flow set value to flow set address 
			USART_RX_STA = 0;
		} 
		if(UTR_PRESS)//utr mode button pressed 
		{
			ClearUsart_Buf();              
			DwinBitIconDisplay(UTR_ICON,0);//display bujin icon 
			pressuresetvalue=para2flash[9];
			pressuresetlimt =para2flash[10];        
			flowsetvalue    =para2flash[11]; 
			para2flash[18]   = UTR_ICON;        
			DwinDisplayVariate(PRESSURESETADDR,para2flash[9]);//display pre_pressure set
			DwinDisplayVariate(FLOWSETADDR,para2flash[11]);//display pre_flow set
			DwinDisplayVariate(PRESSUREKEYADDR,para2flash[9]);//send pressure set value to pressure set address
			DwinDisplayVariate(FLOWKEYADDR,para2flash[11]);//send flow set value to flow set address
			USART_RX_STA = 0;
		} 
		if(ART_PRESS)//art mode button pressed 
		{
			ClearUsart_Buf();              
			DwinBitIconDisplay(ART_ICON,0);//display bujin icon 
			pressuresetvalue=para2flash[12];
			pressuresetlimt =para2flash[13];        
			flowsetvalue    =para2flash[14]; 
			para2flash[18]   = ART_ICON;        
			DwinDisplayVariate(PRESSURESETADDR,para2flash[12]);//display pre_pressure set
			DwinDisplayVariate(FLOWSETADDR,para2flash[14]);//display pre_flow set
			DwinDisplayVariate(PRESSUREKEYADDR,para2flash[12]);//send pressure set value to pressure set address
			DwinDisplayVariate(FLOWKEYADDR,para2flash[14]);//send flow set value to flow set address      
			USART_RX_STA = 0;			
		} 
		if(CYS_PRESS)//cys mode button pressed 
		{
			ClearUsart_Buf();              
			DwinBitIconDisplay(CYS_ICON,0);//display bujin icon 
			pressuresetvalue=para2flash[15];
			pressuresetlimt =para2flash[16];        
			flowsetvalue    =para2flash[17]; 
			para2flash[18]   = CYS_ICON;        
			DwinDisplayVariate(PRESSURESETADDR,para2flash[15]);//display pre_pressure set
			DwinDisplayVariate(FLOWSETADDR,para2flash[17]);//display pre_flow set
			DwinDisplayVariate(PRESSUREKEYADDR,para2flash[15]);//send pressure set value to pressure set address
			DwinDisplayVariate(FLOWKEYADDR,para2flash[17]);//send flow set value to flow set address
			USART_RX_STA = 0;
		}   
	}		
}

extern float chulival_gal;
extern u8 display_delay_flag;
u16 display_delay = 0;
float display_delay_val;
float chulisensorvalue;


void PressureValHandle()
{   
    if( bjiconstatue&0x01 )
	{
/******************************************过压后，延时显示实际压力处理*******************************************************************/  
		if( display_delay_flag == 1 )
        {
            chulisensorvalue = display_delay_val;
            delay_ms(1);
            display_delay++;//
            if( display_delay >= 300 )
            {
               display_delay = 0;             
               display_delay_flag = 0;//??3y??è?ifμ?±ê????
            }            
        }
        else
        {
			chulisensorvalue = chulival_gal;//显示减过之后的值
			display_delay_val = chulival_gal;
		}
	}
/******************************************过压后，延时显示实际压力处理*******************************************************************/  			
    else
	{
		chulisensorvalue = chuli_current_val;//read chuli sensor value (need to transition)
	}		


}

void ChuliDisplay(float sensorvalue )
{    
	static u16 oldsensorvalue=0;
	
	if(oldsensorvalue != (u16)sensorvalue)
	{
		if( sensorvalue < 32 )sensorvalue = 0;
		
		DwinDisplayVariate(PRESSUREDISADDR,(u16)sensorvalue);//display variate
        
        if(pressuresetvalue<50)//当压力Fa＜50mmHg时，压力误差为2.5mmHg; 
        {
            if( 0 <= sensorvalue && sensorvalue <= (float)pressuresetvalue+2.5 )//pressure under nomal condition
            {
                ColumnDisplay(PRESSURECOLUMNADDR,69,(u16)(231-sensorvalue*179/500),85,231,GREEN);//Correspond voltage values to column and column display green
            }
            else if( (float)pressuresetvalue+2.5 < sensorvalue && sensorvalue <= pressuresetvalue+30)//over pressure 5mmhg  
            {
                ColumnDisplay(PRESSURECOLUMNADDR,69,(u16)(231-sensorvalue*179/500),85,230,YELLOW);//Correspond voltage values to column and column display green
            }
            else if(pressuresetvalue+30 < sensorvalue)//over pressure 30mmhg
            { 
                ColumnDisplay(PRESSURECOLUMNADDR,69,(u16)(231-sensorvalue*179/500),85,230,RED);//Correspond voltage values to column and column display green
            }
        }
        else
        {       
            if( 0 <= sensorvalue && sensorvalue <= (float)pressuresetvalue*1.05 )//pressure under nomal condition
            {
                ColumnDisplay(PRESSURECOLUMNADDR,69,(u16)(231-sensorvalue*179/500),85,231,GREEN);//Correspond voltage values to column and column display green
            }
            else if( (float)pressuresetvalue*1.05 < sensorvalue&&sensorvalue <= pressuresetvalue+30)//over pressure 5mmhg  
            {
                ColumnDisplay(PRESSURECOLUMNADDR,69,(u16)(231-sensorvalue*179/500),85,230,YELLOW);//Correspond voltage values to column and column display green
            }
            else if(pressuresetvalue+30 < sensorvalue)//over pressure 30mmhg
            { 
                ColumnDisplay(PRESSURECOLUMNADDR,69,(u16)(231-sensorvalue*179/500),85,230,RED);//Correspond voltage values to column and column display green
            }
        }
	}
	oldsensorvalue = sensorvalue;
}


void FlowDisplay( u8 mode,u16 gear )
{
    if( getflowflag )//flow under nomal condition
    {
        getflowflag=0;
        if( mode )
        {
			DwinDisplayVariate(FLOWDISADDR,gear);//display flow set
			ColumnDisplay(FLOWCOLUMNADDR,240,(u16)(230-gear*179/13),263,230,GREEN); //the column display green
        }   
        else  
        {
            DwinDisplayVariate(FLOWDISADDR,0);//display flow set
            ColumnDisplay(FLOWCOLUMNADDR,240,230,263,230,GREEN); //the column display green
        }            
    }
}


void NegativeDisplay()
{        
	if( negative_pressure_current_val >= 0 )
	{
		if( negative_pressure_current_val < presetnpressure+5 )
			ColumnDisplay( NPRESSURECOLUMNADDR,450,(u16)( 86 - ( negative_pressure_current_val*41/presetnpressure) ),456,86,GREEN ); //the column display green 
		else
			ColumnDisplay( NPRESSURECOLUMNADDR,450,45,456,86,GREEN ); //the column display green
	} 
}

void DwinDisplayText(u16 addr)
{
    u8 addressh;
    u8 addressl;
    addressh =(u8)((addr&0xff00)>>8);
    addressl = (u8)((addr&0x00ff));  
    textarray[4]=addressh;
    textarray[5]=addressl;
		
    HAL_UART_Transmit(&UART1_Handler,(uint8_t*)textarray,6,1000);	    //发送帧头
    while(__HAL_UART_GET_FLAG(&UART1_Handler,UART_FLAG_TC)!=SET);//等待发送结束
    
    printf(VASION);
	
}


//u8 textarray[]        = {0x5a,0xa5,0x54,0x82,0x00,0x00,0x00,0x04,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00}; 
