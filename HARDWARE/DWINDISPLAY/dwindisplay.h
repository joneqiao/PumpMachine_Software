#ifndef _dwindisplay_H
#define _dwindisplay_H
#include "sys.h"

#define RED          0xF800 
#define YELLOW       0xFc08
#define GREEN        0x0400
#define START        0
#define STOP         1

#define parameternum   26//paramete numbers need to save to flash 

#define PRESSUREDISADDR       0x0000 
#define PRESSURESETADDR       0x0002

#define PRESSUREKEYADDR       0x0004
#define FLOWKEYADDR           0x0026

#define FLOWDISADDR           0x0022 
#define FLOWSETADDR           0x0024

#define PRESSURECOLUMNADDR    0x0006 
#define FLOWCOLUMNADDR        0x0028
#define NPRESSURECOLUMNADDR   0x0080

#define XYICONADDR            0x0046
#define BJICONADDR            0x0042 

#define URT_ICON	          0x0050
#define PCN_ICON	          0x0051
#define PCN_P_ICON	          0x0052
#define UTR_ICON	          0x0053
#define ART_ICON	          0x0054
#define CYS_ICON	          0x0055

#define MOTORALARMADDR        0X0115


#define PRESSUREPRESS      USART_BUF[2]==0x06&&USART_BUF[3]==0x83&&USART_BUF[4]==0x00&&USART_BUF[5]==0x04&&USART_BUF[6]==0x01
#define FLOWPRESS          USART_BUF[2]==0x06&&USART_BUF[3]==0x83&&USART_BUF[4]==0x00&&USART_BUF[5]==0x26&&USART_BUF[6]==0x01
#define FACTORMODE         USART_BUF[2]==0x08&&USART_BUF[3]==0x83&&USART_BUF[4]==0x00&&USART_BUF[5]==0x78&&USART_BUF[6]==0x02&&USART_BUF[7]==0x00&&USART_BUF[8]==0x00&&USART_BUF[9]==0xd8&&USART_BUF[10]==0x29
#define WATERHIGHTMODE     USART_BUF[2]==0x08&&USART_BUF[3]==0x83&&USART_BUF[4]==0x00&&USART_BUF[5]==0x78&&USART_BUF[6]==0x02&&USART_BUF[7]==0x00&&USART_BUF[8]==0x00&&USART_BUF[9]==0x04&&USART_BUF[10]==0xd2
#define CHULILONGPRESS     USART_BUF[2]==0x06&&USART_BUF[3]==0x83&&USART_BUF[4]==0x01&&USART_BUF[5]==0x20&&USART_BUF[6]==0x01
#define CHULILONGHEIGH     USART_BUF[2]==0x06&&USART_BUF[3]==0x83&&USART_BUF[4]==0x01&&USART_BUF[5]==0x50&&USART_BUF[6]==0x01&&USART_BUF[7]==0x00&&USART_BUF[8]==0x03
#define XYSTARTPRESS       USART_BUF[2]==0x06&&USART_BUF[3]==0x83&&USART_BUF[4]==0x00&&USART_BUF[5]==0x44&&USART_BUF[6]==0x01&&USART_BUF[7]==0x5A&&USART_BUF[8]==0x5A
#define BJSTARTPRESS       USART_BUF[2]==0x06&&USART_BUF[3]==0x83&&USART_BUF[4]==0x00&&USART_BUF[5]==0x40&&USART_BUF[6]==0x01&&USART_BUF[7]==0x5A&&USART_BUF[8]==0x5A
#define URT_PRESS          USART_BUF[2]==0x06&&USART_BUF[3]==0x83&&USART_BUF[4]==0x00&&USART_BUF[5]==0x67&&USART_BUF[6]==0x01&&USART_BUF[7]==0x00&&USART_BUF[8]==0x02
#define PCN_PRESS          USART_BUF[2]==0x06&&USART_BUF[3]==0x83&&USART_BUF[4]==0x00&&USART_BUF[5]==0x67&&USART_BUF[6]==0x01&&USART_BUF[7]==0x00&&USART_BUF[8]==0x03	          
#define PCN_P_PRESS        USART_BUF[2]==0x06&&USART_BUF[3]==0x83&&USART_BUF[4]==0x00&&USART_BUF[5]==0x67&&USART_BUF[6]==0x01&&USART_BUF[7]==0x00&&USART_BUF[8]==0x04	          
#define UTR_PRESS          USART_BUF[2]==0x06&&USART_BUF[3]==0x83&&USART_BUF[4]==0x00&&USART_BUF[5]==0x67&&USART_BUF[6]==0x01&&USART_BUF[7]==0x00&&USART_BUF[8]==0x05	          
#define ART_PRESS          USART_BUF[2]==0x06&&USART_BUF[3]==0x83&&USART_BUF[4]==0x00&&USART_BUF[5]==0x67&&USART_BUF[6]==0x01&&USART_BUF[7]==0x00&&USART_BUF[8]==0x06	          
#define CYS_PRESS          USART_BUF[2]==0x06&&USART_BUF[3]==0x83&&USART_BUF[4]==0x00&&USART_BUF[5]==0x67&&USART_BUF[6]==0x01&&USART_BUF[7]==0x00&&USART_BUF[8]==0x07	          

void DwinDisplayInit(void);
void DwindisplayInterface( u16 id );//interface id
void DwinDisplayVariate(u16 displayaddr,u16 displayvalue);//display variate
void DwinBitIconDisplay(u16 iconadrress, u16 biticonid);//icon display
void ColumnDisplay(u16 columnadrress,u16 upleftx,u16 uplefty,u16 downrightx,u16 downrighty,u16 colour);
void DwinSetVariate(void);//set and display variate 
void ReadDwinKeyValueReturn(void);//read dwin display keyreturn value 
void ClearUsart_Buf(void);
void ModeSelect(void);
void NegColumnDisplay(void);
void PreColumnDisplay(float pressurevalue);
void ChuliDisplay(float sensorvalue );
void PressureValHandle(void);
void NegativeDisplay(void);
void FlowDisplay(u8 mode,u16 gear);
void DwinReadVariate(u16 readaddr);//read variate
void DwinDisplayText(u16 addr);
#endif

