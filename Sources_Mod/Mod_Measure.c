#include "App_Include.h"

#define     DEF_MEASURE_WAIT        (0xEE)
#define     DEF_MEASURE_START       (0xAA)
#define     DEF_MEASURE_END         (0x55)
#define     DEF_MEASURE_TIMEOUT     (0x77)

#define     DEF_MEASURE_DBG_EN           TRUE

#if (DEF_MEASURE_DBG_EN == TRUE)
    #define MEASURE_DBG(...)            do {                                \
                                            OS_ERR os_err;                  \
                                            OSSchedLock(&os_err);           \
                                            printf(__VA_ARGS__);            \
                                            OSSchedUnlock(&os_err);         \
                                        }while(0)
#else
    #define MEASURE_DBG(...)
#endif

Measure_t st_Measure = {
  
    0,          /* ������� */
    0,          /* ����ʱ�� */
    0,          /* ����ʱ�� */
    
    {0},        /* ���� */
    0.0,        /* �ٶ� */
    0.0,        /* ���ٶ� */

    0.0,        /* CO2Ũ�� */
    0.0,        /* CO1Ũ�� */
    0.0,        /* NOŨ�� */
    0.0,        /* HCŨ�� */
    0.0,        /* �̶� */
};

void DoorCloseFnct (void *p_tmr,  void *p_arg);
OS_TMR CloseDoorTmr;

static void InitSem(void)
{
    OS_ERR os_err;
    OSTaskSemSet(&TaskMeasureTCB,0U,&os_err);
}

static void PostSem(void)
{
    OS_ERR os_err;
    OSTaskSemPost(&TaskMeasureTCB,OS_OPT_POST_NONE,&os_err);
}

static BOOL PendSem(void)
{
    OS_ERR os_err;
    OSTaskSemPend(0,OS_OPT_PEND_BLOCKING,NULL,&os_err);
    if (os_err != OS_ERR_NONE)
        return FALSE;
    return TRUE;
}

static void PostMsg(void* pv_Msg)
{
    OS_ERR  os_err;
    OSTaskQPost(&TaskMeasureTCB,(void*)pv_Msg,1,OS_OPT_POST_FIFO ,&os_err);
    if(os_err != OS_ERR_NONE)
    {
    }
}

static void* PendMeg(void)
{
    OS_ERR  os_err;
    INT16U  uin_Size = 0;
    void * pv_Msg;
    pv_Msg = OSTaskQPend(0,OS_OPT_PEND_BLOCKING,&uin_Size,0,&os_err);

    if(os_err == OS_ERR_NONE)
        return pv_Msg;
    else
        return 0;
}

static void StartTimeOutCheck(void)
{
    OS_ERR os_err;
    OSTmrStart(&CloseDoorTmr,&os_err);         //������ʱ���
}

static void StopTimeOutCheck(void)
{
    OS_ERR os_err;
    OSTmrStop(&CloseDoorTmr,OS_OPT_TMR_NONE,NULL,&os_err);
}


static void InitTimeOutCheck(void)
{
    OS_ERR os_err;
    OSTmrCreate(&CloseDoorTmr,          /* p_tmr */
                "MeasSpeedTimeOut",     /* p_name */
                300,                    /* dly 300* 0.01s = 3s*/
                0,                      /* period */
                OS_OPT_TMR_ONE_SHOT,    /* opt */
                DoorCloseFnct,          /* p_callback */
                0,                      /* p_callback_arg */
                &os_err);               /* p_err */   
}

void Mod_MeasureInit(Measure_t* pst_Meas)
{
    InitSem();
    InitTimeOutCheck();  
}

void Mod_MeasurePoll(Measure_t* pst_Meas)
{
    INT32U ul_Msg = (INT32U)PendMeg();
    switch(ul_Msg)
    {
    case DEF_MEASURE_WAIT:
        /* ��ͷ��ס��һ�������� ֹͣ���±��� */
        MEASURE_DBG(">>MEASURE DBG:   ��������׼����ʼ ֹͣ���±���\r\n");
        
        StartTimeOutCheck();                                    //��ʼ��ʱ���
        
        Mod_GreyGotoWait(&st_Grey);                             //�̹�ȴ���ʼ����
        Mod_GasMeasureGotoAbsMeasure(&st_GasMeasure);           //���⿪ʼ��ֲ��� ��ʼ���Բ���
        
        break;
    case DEF_MEASURE_START:
        /* ������ʱ��� ��������ģʽ */
        MEASURE_DBG(">>MEASURE DBG:   ��������ʼ ��ʼ��ֲ���\r\n");
        
        Mod_GreyGotoMeas(&st_Grey);                             //�̹⿪ʼ����
        Mod_GasMeasureGotoDiffMeasure(&st_GasMeasure);          //���⿪ʼ��ֲ��� ��ʼ���Բ���
        break;
    case DEF_MEASURE_END:
        /* �������� ��ȡ�����ݼ��� Ũ�� */
        MEASURE_DBG(">>MEASURE DBG:   ����������� �����ȡһ�β��ٽ��\r\n");
        
        StopTimeOutCheck();                                     //ֹͣ��ʱ���
        
        Mod_GreyGotoIdle(&st_Grey);                             //�̹��������  
        Mod_GasMeasureGotoAbsMeasure(&st_GasMeasure);           //���������ֲ��� ��ʼ���Բ���
        
        Mod_MeasSpeedRequest(&st_MeasSpeed);
        if(PendSem() != TRUE)
            break;

        MEASURE_DBG(">>MEASURE DBG:   һ�β������\r\n");
        MEASURE_DBG(">>MEASURE DBG:   �����ٶ�:     %f\r\n",pst_Meas->st_Result.f_Speed);
        MEASURE_DBG(">>MEASURE DBG:   �������ٶ�:   %f\r\n",pst_Meas->st_Result.f_Acc);
        MEASURE_DBG(">>MEASURE DBG:   �����������\r\n");
        break;
    case DEF_MEASURE_TIMEOUT:
        /* �ָ�����ģʽ  */
        MEASURE_DBG(">>MEASURE DBG:   ��������ʱ\r\n");
        
        StopTimeOutCheck();                 //ֹͣ��ʱ��� 
        break;
    default:
        break;
    }
}
void Mod_MeasSpeedReply(MeasSpeed_t* pst_Meas)
{
    st_Measure.st_Result.f_Acc = pst_Meas->f_Acc_mps2;
    st_Measure.st_Result.f_Speed = pst_Meas->f_Speed_mph;
    PostSem();
}

void Bsp_GpioEventHandle(GpioEvent_t* pst_Event)
{
# if 1
    if(pst_Event->uin_GpioPin == GPIO_PIN_14 && pst_Event->b_IsRising == TRUE )         //��ͷ��ס��һ��������
    {
        PostMsg((void*)DEF_MEASURE_WAIT);
    }
    if(pst_Event->uin_GpioPin == GPIO_PIN_14 && pst_Event->b_IsRising == FALSE )        //��λ�뿪��һ��������
    {
        PostMsg((void*)DEF_MEASURE_START);
    } 
    else if(pst_Event->uin_GpioPin == GPIO_PIN_13 && pst_Event->b_IsRising == FALSE )   //��λ�뿪�ڶ���������
    {
        PostMsg((void*)DEF_MEASURE_END);
    }
#else
    MEASURE_DBG("����:0x%x,����:%x\r\n", pst_Event->uin_GpioPin, pst_Event->b_IsRising);
#endif
}

void DoorCloseFnct (void *p_tmr,  void *p_arg)
{
    
}
