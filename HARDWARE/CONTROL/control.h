#ifndef _CONTROL_H
#define _CONTROL_H
#include "sys.h"

void SystemRun(void);
void XiYinControl(void);
float filter(void);
float KalmanFilter(float inData);

void Ps_PrGreaterFive(u16 FLset3);
void ZeroLessPs_PrlessFive(void);
void ZerolessPr_PsLessTwo(void);
void TwolessPr_PsLessFive(void);
void FivelessPr_PsLessThirty(void);
void ThirtylessPr_Ps(void);
void MotorControl(void);

#endif

