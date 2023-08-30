#ifndef _PID_H_
#define _PID_H_

struct PID
{
    float SetPoint;               //Desired Value
    float Proportion;             //Proportional Const
    float Integral;               //Integral Const
    float Derivative;             //Derivative Const
    float LastError;              //Error[-1] 
    float PrevError;              //Error[-2] 
    float SumError;               //Sums of Errors
};

extern struct PID mypid;

void Init_Pid(void);
float PIDCalc(struct PID *pp, float NextPoint);

#endif

