#ifndef __BSP_TIMER_H__
#define __BSP_TIMER_H__

#define     DEF_SOFT_TIMER_MAX      20

typedef struct {
    BOOL    b_IsOneShot;        //����ģʽ
    INT32U  ul_Dly;             //��һ�εĶ�ʱʱ��    
    INT32U  ul_Period;          //���ڶ�ʱʱ��
    void    (*cb_CallBack)(void* pv_Timer);         //�ص�����
    
    BOOL    b_Run;              //�Ƿ�����
    BOOL    b_First;            //��һ������
    INT32U  ul_Cnt;             //����ֵ
}SoftTimer_t;

typedef struct  {
    INT32U ul_Cnt; 
    INT32U ul_Tick;
    INT32U ul_CntReload;
    INT32U ul_TickReload;
}TimeSample_t;

void Bsp_DelayUs(INT32U us);
void Bsp_DelayMs(INT32U ms);

BOOL Bsp_TimerReg(SoftTimer_t* pst_Timer);

BOOL Bsp_TimerStart(SoftTimer_t* pst_Timer);

BOOL Bsp_TimerStop(SoftTimer_t* pst_Timer);

void Bsp_TimerPoll(void);

void Bsp_TimeSampleInit(void);
void Bsp_GetTimeSample(TimeSample_t* pst_Ts);
INT32U Bsp_GetInterval(TimeSample_t* pst_TsOld,TimeSample_t * pst_TsNew);


#endif
