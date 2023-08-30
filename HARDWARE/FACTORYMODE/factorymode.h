#ifndef _FACTORYMODE_H
#define _FACTORYMODE_H
#include "sys.h"

extern u8 USART_BUF[];       //receive buffer

#define NPDISADDR       0x006E
#define CLDISADDR       0x00AA

#define URT_pSet        0x0090
#define URT_pLimit      0x0092
#define URT_fSet        0x0094

#define PCN_pSet        0x0096
#define PCN_pLimit      0x0098
#define PCN_fSet        0x009a

#define PCN_P_pSet      0x009c
#define PCN_P_pLimit    0x009e
#define PCN_P_fSet      0x0101

#define UTR_pSet        0x0103
#define UTR_pLimit      0x0105
#define UTR_fSet        0x0107

#define ART_pSet        0x0109
#define ART_pLimit      0x010b
#define ART_fSet        0x010d

#define CYS_pSet        0x010f
#define CYS_pLimit      0x0111
#define CYS_fSet        0x0113

#define WATERHIGHTADD   0x0115
#define VERSIONADD      0x1117

#define CHULIKADD   0x00A0
#define CHULIBADD   0x00A4
#define NPKADD      0x0072
#define NPBADD      0x00A8

#define EXIT_FM_PRESSED  ( USART_BUF[2]==0x06&&USART_BUF[3]==0x83&&USART_BUF[4]==0x00&&USART_BUF[5]==0x66&&USART_BUF[6]==0x01&&USART_BUF[7]==0x5A&&USART_BUF[8]==0x5A )
#define SAVE_FM_PRESSED  ( USART_BUF[2]==0x06&&USART_BUF[3]==0x83&&USART_BUF[4]==0x00&&USART_BUF[5]==0x6c&&USART_BUF[6]==0x01&&USART_BUF[7]==0x00&&USART_BUF[8]==0x01 )

#define XY_FM_PRESSED    ( USART_BUF[2]==0x06&&USART_BUF[3]==0x83&&USART_BUF[4]==0x00&&USART_BUF[5]==0x44&&USART_BUF[6]==0x01&&USART_BUF[7]==0x5A&&USART_BUF[8]==0x5A )
#define CX_FM_PRESSED    ( USART_BUF[2]==0x06&&USART_BUF[3]==0x83&&USART_BUF[4]==0x00&&USART_BUF[5]==0xAA&&USART_BUF[6]==0x01&&USART_BUF[7]==0x55&&USART_BUF[8]==0x55 )

void FMGetWaterHight(void);
void FactoryMode(void);//进入工厂模式
void FM_GetMode_Paramete(void);//5A A5 06 83 00 48 01 00 EC
void SaveParameters(void);
void ReadFlashDisplay(void);
void FMNPressureDebug(void);//negative pressure sensor adjusting
void FMChuLiDebug(void);//chu li sensor adjusting   
void FMGetNegP_K_B(void);
void FMGetChuLi_K_B(void);
#endif
