#ifndef     __APP_OS_H__
#define     __APP_OS_H__

#include  <cpu.h>
#include  <lib_math.h>
#include  <lib_mem.h>
#include  <os.h>
#include  <os_app_hooks.h>


#define  APP_OS_ALLOC   FALSE

#if (APP_OS_ALLOC == FALSE)
    #define APP_OS_EXTERN   extern
#else
    #define APP_OS_EXTERN
#endif



//==================================================================================
//                                   �������ȼ�
//==================================================================================
#define  TASK_START_PRIO                OS_CFG_PRIO_MAX-4u

/*  Ӳ���������   */
#define  TASK_USB_HOST_PRIO             4u
#define  TASK_STDBUSLASER_PRIO          7u
#define  TASK_STDBUSMASTER_PRIO         8u
#define  TASK_STDBUSSLAVE_PRIO          9u

/* ���������� */

#define  TASK_MEASURE_PRIO              6u
#define  TASK_GASPROC_PRIO              10u
#define  TASK_GREYPROC_PRIO             11u
#define  TASK_LASER_PRIO                12u
#define  TASK_MEASSPEED_PRIO            13u

#define  TASK_DISBOARD_PRIO             20u


#define  TASK_CML_RECV_PRIO             20u
#define  TASK_CML_SEND_PRIO             21u

//==================================================================================
//                                   �����ջ��С
//==================================================================================

#define  TASK_START_STK_SIZE            256u

#define  TASK_USB_HOST_STK_SIZE         4096u
#define  TASK_STDBUSLASER_STK_SIZE      512u
#define  TASK_STDBUSMASTER_STK_SIZE     512u
#define  TASK_STDBUSSLAVE_STK_SIZE      512u

#define  TASK_MEASURE_STK_SIZE          512u
#define  TASK_GASPROC_STK_SIZE          512u
#define  TASK_GREYPROC_STK_SIZE         512u
#define  TASK_LASER_STK_SIZE            512u
#define  TASK_MEASSPEED_STK_SIZE        1024u

#define  TASK_DISBOARD_STK_SIZE         256u

#define  TASK_CML_SEND_STK_SIZE         256u
#define  TASK_CML_RECV_STK_SIZE         256u

/*
//
#define  TASK_START_STK_SIZE            256u

#define  TASK_USB_HOST_STK_SIZE         1024u
#define  TASK_STDBUSLASER_STK_SIZE      256u
#define  TASK_STDBUSMASTER_STK_SIZE     256u
#define  TASK_STDBUSSLAVE_STK_SIZE      256u

#define  TASK_MEASURE_STK_SIZE          512u
#define  TASK_GASPROC_STK_SIZE          256u
#define  TASK_GREYPROC_STK_SIZE         256u
#define  TASK_LASER_STK_SIZE            256u

#define  TASK_MEASSPEED_STK_SIZE        1024u

#define  TASK_DISBOARD_STK_SIZE         256u

#define  TASK_CML_SEND_STK_SIZE         256u
#define  TASK_CML_RECV_STK_SIZE         256u
*/

//==================================================================================
//                                   �����ջ����
//==================================================================================
APP_OS_EXTERN  CPU_STK      TaskStartStk   [TASK_START_STK_SIZE];                  /*  ��ʼ����    */
APP_OS_EXTERN  CPU_STK      TaskUsbHostStk [TASK_USB_HOST_STK_SIZE];               /*  ����������    */
APP_OS_EXTERN  CPU_STK      TaskStdBusLaserStk  [TASK_STDBUSLASER_STK_SIZE];       /*  STDBUS���������    */
APP_OS_EXTERN  CPU_STK      TaskStdBusMasterStk  [TASK_STDBUSMASTER_STK_SIZE];     /*  STDBUS��������    */
APP_OS_EXTERN  CPU_STK      TaskStdBusSlaveStk  [TASK_STDBUSSLAVE_STK_SIZE];       /*  STDBUS�ӻ�����    */
 
APP_OS_EXTERN  CPU_STK      TaskMeasureStk[TASK_MEASURE_STK_SIZE];                 /*  �ۺϲ�������    */
APP_OS_EXTERN  CPU_STK      TaskGasProcStk [TASK_GASPROC_STK_SIZE];                /*  ����⴦������    */
APP_OS_EXTERN  CPU_STK      TaskGreyProcStk[TASK_GREYPROC_STK_SIZE];               /*  �̹�⴦������    */
APP_OS_EXTERN  CPU_STK      TaskLaserStk[TASK_LASER_STK_SIZE];                     /*  ���������    */
APP_OS_EXTERN  CPU_STK      TaskMeasSpeedStk[TASK_MEASSPEED_STK_SIZE];             /*  ��������    */
APP_OS_EXTERN  CPU_STK      TaskDisBoardStk[TASK_DISBOARD_STK_SIZE];               /*  ��ʾ������    */
               
APP_OS_EXTERN  CPU_STK      TaskCmlSendStk [TASK_CML_SEND_STK_SIZE];               /*  �����е�������    */
APP_OS_EXTERN  CPU_STK      TaskCmlRecvStk [TASK_CML_RECV_STK_SIZE];               /*  �����е�������    */
//==================================================================================
//                                   ������ƿ�����
//==================================================================================
APP_OS_EXTERN  OS_TCB       TaskStartTCB;           /*  ��ʼ����    */
APP_OS_EXTERN  OS_TCB       TaskUsbHostTCB;         /*  Usb������ͨѶ����    */
APP_OS_EXTERN  OS_TCB       TaskStdBusLaserTCB;     /*  STDBUS���������    */
APP_OS_EXTERN  OS_TCB       TaskStdBusMasterTCB;    /*  STDBUS��������    */
APP_OS_EXTERN  OS_TCB       TaskStdBusSlaveTCB;     /*  STDBUS�ӻ�����    */

APP_OS_EXTERN  OS_TCB       TaskMeasureTCB;        /*  �ۺϲ�������    */
APP_OS_EXTERN  OS_TCB       TaskGasProcTCB;         /*  ����⴦������    */
APP_OS_EXTERN  OS_TCB       TaskGreyProcTCB;        /*  �̹�⴦������    */
APP_OS_EXTERN  OS_TCB       TaskLaserTCB;           /*  ���������    */
APP_OS_EXTERN  OS_TCB       TaskMeasSpeedTCB;       /*  ��������    */
APP_OS_EXTERN  OS_TCB       TaskDisBoardTCB;        /*  ��ʾ������    */

APP_OS_EXTERN  OS_TCB       TaskCmlSendTCB;         /*  �����е�������    */
APP_OS_EXTERN  OS_TCB       TaskCmlRecvTCB;         /*  �����е�������    */
//==================================================================================
//                                   ����������
//==================================================================================
APP_OS_EXTERN  void Task_Start (void  *p_arg);               /*  ��ʼ����    */
APP_OS_EXTERN  void Task_UsbHost (void  *p_arg);             /*  Usb������ͨѶ���� */
APP_OS_EXTERN  void Task_StdBusLaser (void  *p_arg);         /*  STDBUS���������    */
APP_OS_EXTERN  void Task_StdBusMaster (void  *p_arg);        /*  STDBUS����ͨѶ    */
APP_OS_EXTERN  void Task_StdBusSlave (void  *p_arg);         /*  STDBUS����ͨѶ    */

APP_OS_EXTERN  void Task_Measure (void *p_arg);              /*  �ۺϲ�������    */
APP_OS_EXTERN  void Task_GasProc (void  *p_arg);             /*  ����⴦������    */
APP_OS_EXTERN  void Task_GreyProc (void  *p_arg);            /*  �̹�⴦������    */
APP_OS_EXTERN  void Task_Laser (void *p_arg);                /*  ���������    */
APP_OS_EXTERN  void Task_MeasSpeed (void *p_arg);            /*  ��������    */
APP_OS_EXTERN  void Task_DisBoard (void *p_arg);             /*  ��ʾ������    */

APP_OS_EXTERN  void Task_TransCml (void  *p_arg);            /*  �����е�������    */
APP_OS_EXTERN  void Task_RecvCml (void  *p_arg);             /*  �����е�������    */

//==================================================================================
//                                   ��������
//==================================================================================
#if (OS_CFG_Q_EN > 0u)
APP_OS_EXTERN  OS_Q         QCmlRecv;
APP_OS_EXTERN  OS_Q         QCmlTrans;
APP_OS_EXTERN  OS_Q         QSpeRecv;
APP_OS_EXTERN  OS_Q         QSpeTrans;
#endif

//==================================================================================
//                                   �ź���
//==================================================================================
#if (OS_CFG_SEM_EN > 0u)
APP_OS_EXTERN  OS_SEM       Sem_Rs485;
APP_OS_EXTERN  OS_SEM       Sem_Laser;
APP_OS_EXTERN  OS_SEM       Sem_Maser;
APP_OS_EXTERN  OS_SEM       Sem_LaserRecv;
#endif

//==================================================================================
//                                   �����ź���
//==================================================================================
#if (OS_CFG_MUTEX_EN > 0u)
static  OS_MUTEX     AppTaskObjMutex;
#endif

//==================================================================================
//                                   ����
//==================================================================================
#if (OS_CFG_Q_EN > 0u)
static  OS_Q         AppTaskObjQ;
#endif

//==================================================================================
//                                   �¼����
//==================================================================================
#if (OS_CFG_FLAG_EN > 0u)
static  OS_FLAG_GRP  AppTaskObjFlag;
#endif

//==================================================================================
//                                 ���ٵ�������
//==================================================================================
#ifndef  TRACE_LEVEL_OFF
#define  TRACE_LEVEL_OFF                        0u
#endif

#ifndef  TRACE_LEVEL_INFO
#define  TRACE_LEVEL_INFO                       1u
#endif

#ifndef  TRACE_LEVEL_DBG
#define  TRACE_LEVEL_DBG                        2u
#endif

/*
#define  TRACE_LEVEL                            TRACE_LEVEL_DBG
#define  TRACE(x)                               printf(x)//Task_CmlSendMsg(x,strlen(x))  printf(x)


#define  TRACE_INFO(x)                          ((TRACE_LEVEL >= TRACE_LEVEL_INFO)  ? (void)(TRACE(x)) : (void)0)
#define  TRACE_DBG(x)                           ((TRACE_LEVEL >= TRACE_LEVEL_DBG)   ? (void)(TRACE(x)) : (void)0)
*/

#define  TRACE_DBG(...)                             do {                                \
                                                        OS_ERR os_err;                  \
                                                        OSSchedLock(&os_err);           \
                                                        printf(__VA_ARGS__);            \
                                                        OSSchedUnlock(&os_err);         \
                                                    }while(0)


#endif
