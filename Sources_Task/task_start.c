#include "App_Include.h"

void AppTaskCreate (void);
void AppObjCreate (void);

//==================================================================================
//                                   ������ƿ�����
//==================================================================================
OS_TCB       TaskStartTCB;           /*  ��ʼ����    */
OS_TCB       TaskGasProcTCB;         /*  ����⴦������    */
OS_TCB       TaskGreyProcTCB;        /*  �̹�⴦������    */
OS_TCB       TaskCmlSendTCB;         /*  �����е�������    */
OS_TCB       TaskCmlRecvTCB;         /*  �����е�������    */
OS_TCB       TaskUsbHostTCB;         /*  Usb������ͨѶ����    */
OS_TCB       TaskStdBusLaserTCB;     /*  STDBUS���������    */
OS_TCB       TaskStdBusMasterTCB;    /*  STDBUS��������    */
OS_TCB       TaskStdBusSlaveTCB;     /*  STDBUS�ӻ�����    */
OS_TCB       TaskLaserTCB;           /*  ���������    */
OS_TCB       TaskDisBoardTCB;        /*  ��ʾ������    */
OS_TCB       TaskMeasSpeedTCB;       /*  ��������    */
//==================================================================================
//                                   �����ջ����
//==================================================================================
CPU_STK      TaskStartStk   [TASK_START_STK_SIZE];                  /*  ��ʼ����    */
CPU_STK      TaskGasProcStk [TASK_GASPROC_STK_SIZE];                /*  ����⴦������    */
CPU_STK      TaskGreyProcStk[TASK_GREYPROC_STK_SIZE];               /*  �̹�⴦������    */
CPU_STK      TaskCmlSendStk [TASK_CML_SEND_STK_SIZE];               /*  �����е�������    */
CPU_STK      TaskCmlRecvStk [TASK_CML_RECV_STK_SIZE];               /*  �����е�������    */
CPU_STK      TaskUsbHostStk [TASK_USB_HOST_STK_SIZE];               /*  ����������    */
CPU_STK      TaskStdBusLaserStk  [TASK_STDBUSLASER_STK_SIZE];       /*  STDBUS���������    */
CPU_STK      TaskStdBusMasterStk  [TASK_STDBUSMASTER_STK_SIZE];     /*  STDBUS��������    */
CPU_STK      TaskStdBusSlaveStk  [TASK_STDBUSSLAVE_STK_SIZE];       /*  STDBUS�ӻ�����    */
CPU_STK      TaskLaserStk[TASK_LASER_STK_SIZE];                     /*  ���������    */
CPU_STK      TaskDisBoardStk[TASK_DISBOARD_STK_SIZE];               /*  ��ʾ������    */
CPU_STK      TaskMeasSpeedStk[TASK_MEASSPEED_STK_SIZE];             /*  ��������    */
//==================================================================================
//                                   ��������
//==================================================================================
#if (OS_CFG_Q_EN > 0u)
OS_Q         QCmlRecv;
OS_Q         QCmlTrans;
OS_Q         QSpeRecv;
OS_Q         QSpeTrans;
#endif

//==================================================================================
//                                   �ź���
//==================================================================================
#if (OS_CFG_SEM_EN > 0u)
OS_SEM       Sem_Rs485;
OS_SEM       Sem_Laser;
#endif

extern  void standalone(void);
//==================================================================================================
//| �������� | Task_Start
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��ʼ����
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��
//|----------|--------------------------------------------------------------------------------------
//| ���ز��� | ��
//|----------|--------------------------------------------------------------------------------------
//| ������� | wjb
//==================================================================================================
void Task_Start (void *p_arg)
{
    OS_ERR  os_err;


   (void)p_arg;

    //standalone();

    Bsp_Init();                                                 /* Initialize BSP functions                             */

    InitParaFromEeprom(INIT_NORMAL);                            //INIT_NORMAL   INIT_DEFAULT


#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&os_err);                            /* ����ͳ������                                         */
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();                                /* �����жϹر�ʱ��                                     */
#endif

    AppObjCreate();                                             /* Create Applicaiton kernel objects                    */

    AppTaskCreate();                                            /* Create Application tasks                             */

    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */

        OSTimeDlyHMSM(0u, 0u, 1u, 0u,
                      OS_OPT_TIME_HMSM_STRICT,
                      &os_err);
        TRACE_DBG(">>DBG:       ��ʼ����\r\n");
        BSP_Led1Toggle();
    }
}

//==================================================================================================
//| �������� | AppTaskCreate
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��������
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��
//|----------|--------------------------------------------------------------------------------------
//| ���ز��� | ��
//|----------|--------------------------------------------------------------------------------------
//| ������� | wjb
//==================================================================================================
void AppTaskCreate (void)
{
    OS_ERR  os_err;

    /* �����ȼ��ӵ͵��߿�ʼ���� */
#if 0
    /* �����е��� */
    OSTaskCreate((OS_TCB       *)&TaskCmlSendTCB,                               /* ����������ƿ� */
                 (CPU_CHAR     *)"Debug Task",                                  /* �������� */
                 (OS_TASK_PTR   )Task_CmlSend,                                  /* ������ */
                 (void         *)0u,                                            /* ������� */
                 (OS_PRIO       )TASK_CML_SEND_PRIO,                            /* �������ȼ� */
                 (CPU_STK      *)&TaskCmlSendStk[0u],                           /* ������ص�ַ */
                 (CPU_STK_SIZE  )TASK_CML_SEND_STK_SIZE / 10u,                  /* ����ջ������ */
                 (CPU_STK_SIZE  )TASK_CML_SEND_STK_SIZE,                        /* �����ջ��С */
                 (OS_MSG_QTY    )100u,                                          /* �ڲ���Ϣ���е������Ϣ��Ŀ */
                 (OS_TICK       )0u,                                            /* ʱ��Ƭ��ѯ��ʱ��Ƭ�� */
                 (void         *)0u,                                            /* �û�����洢�� */
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 (OS_ERR       *)&os_err);                                      /* ��Ŵ���ֵ */
#endif

#if 0
    /* �����е��� */
    OSTaskCreate((OS_TCB       *)&AppTaskCmlRecvTCB,                           /* ����������ƿ� */
                 (CPU_CHAR     *)"App Task CML Recv",                          /* �������� */
                 (OS_TASK_PTR   )AppTaskCmlRecv,                               /* ������ */
                 (void         *)0u,                                            /* ������� */
                 (OS_PRIO       )APP_CFG_TASK_CML_RECV_PRIO,                   /* �������ȼ� */
                 (CPU_STK      *)&AppTaskCmlRecvStk[0u],                       /* ������ص�ַ */
                 (CPU_STK_SIZE  )APP_CFG_TASK_CML_RECV_STK_SIZE / 10u,         /* ����ջ������ */
                 (CPU_STK_SIZE  )APP_CFG_TASK_CML_RECV_STK_SIZE,               /* �����ջ��С */
                 (OS_MSG_QTY    )0u,                                            /* �ڲ���Ϣ���е������Ϣ��Ŀ */
                 (OS_TICK       )0u,                                            /* ʱ��Ƭ��ѯ��ʱ��Ƭ�� */
                 (void         *)0u,                                            /* �û�����洢�� */
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 (OS_ERR       *)&os_err);                                      /* ��Ŵ���ֵ */
#endif


#if 1
    /* STDBUS PORT �߳� */
    OSTaskCreate((OS_TCB       *)&TaskStdBusLaserTCB,                          /* ����������ƿ� */
                 (CPU_CHAR     *)"StdBus Task",                                 /* �������� */
                 (OS_TASK_PTR   )Task_StdBusLaser,                             /* ������ */
                 (void         *)0u,                                            /* ������� */
                 (OS_PRIO       )TASK_STDBUSLASER_PRIO,                        /* �������ȼ� */
                 (CPU_STK      *)&TaskStdBusLaserStk[0u],                            /* ������ص�ַ */
                 (CPU_STK_SIZE  )TASK_STDBUSLASER_STK_SIZE / 10u,              /* ����ջ������ */
                 (CPU_STK_SIZE  )TASK_STDBUSLASER_STK_SIZE,                    /* �����ջ��С */
                 (OS_MSG_QTY    )1u,                                          /* �ڲ���Ϣ���е������Ϣ��Ŀ */
                 (OS_TICK       )0u,                                            /* ʱ��Ƭ��ѯ��ʱ��Ƭ�� */
                 (void         *)0u,                                            /* �û�����洢�� */
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 (OS_ERR       *)&os_err);                                      /* ��Ŵ���ֵ */

    OSTaskCreate((OS_TCB       *)&TaskStdBusMasterTCB,                          /* ����������ƿ� */
                 (CPU_CHAR     *)"StdBus Task",                                 /* �������� */
                 (OS_TASK_PTR   )Task_StdBusMaster,                             /* ������ */
                 (void         *)0u,                                            /* ������� */
                 (OS_PRIO       )TASK_STDBUSMASTER_PRIO,                        /* �������ȼ� */
                 (CPU_STK      *)&TaskStdBusMasterStk[0u],                            /* ������ص�ַ */
                 (CPU_STK_SIZE  )TASK_STDBUSMASTER_STK_SIZE / 10u,              /* ����ջ������ */
                 (CPU_STK_SIZE  )TASK_STDBUSMASTER_STK_SIZE,                    /* �����ջ��С */
                 (OS_MSG_QTY    )100u,                                          /* �ڲ���Ϣ���е������Ϣ��Ŀ */
                 (OS_TICK       )0u,                                            /* ʱ��Ƭ��ѯ��ʱ��Ƭ�� */
                 (void         *)0u,                                            /* �û�����洢�� */
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 (OS_ERR       *)&os_err);                                      /* ��Ŵ���ֵ */

     OSTaskCreate((OS_TCB       *)&TaskStdBusSlaveTCB,                          /* ����������ƿ� */
                  (CPU_CHAR     *)"StdBus Task",                                 /* �������� */
                  (OS_TASK_PTR   )Task_StdBusSlave,                             /* ������ */
                  (void         *)0u,                                            /* ������� */
                  (OS_PRIO       )TASK_STDBUSSLAVE_PRIO,                        /* �������ȼ� */
                  (CPU_STK      *)&TaskStdBusSlaveStk[0u],                            /* ������ص�ַ */
                  (CPU_STK_SIZE  )TASK_STDBUSSLAVE_STK_SIZE / 10u,              /* ����ջ������ */
                  (CPU_STK_SIZE  )TASK_STDBUSSLAVE_STK_SIZE,                    /* �����ջ��С */
                  (OS_MSG_QTY    )100u,                                          /* �ڲ���Ϣ���е������Ϣ��Ŀ */
                  (OS_TICK       )0u,                                            /* ʱ��Ƭ��ѯ��ʱ��Ƭ�� */
                  (void         *)0u,                                            /* �û�����洢�� */
                  (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                  (OS_ERR       *)&os_err);                                      /* ��Ŵ���ֵ */
#endif

#if 1
    /* �������� */
    OSTaskCreate((OS_TCB       *)&TaskMeasSpeedTCB,                                /* ����������ƿ� */
                 (CPU_CHAR     *)"MeasSpeed Task",                                 /* �������� */
                 (OS_TASK_PTR   )Task_MeasSpeed,                                   /* ������ */
                 (void         *)0u,                                            /* ������� */
                 (OS_PRIO       )TASK_MEASSPEED_PRIO,                              /* �������ȼ� */
                 (CPU_STK      *)&TaskMeasSpeedStk[0u],                            /* ������ص�ַ */
                 (CPU_STK_SIZE  )TASK_MEASSPEED_STK_SIZE / 10u,                    /* ����ջ������ */
                 (CPU_STK_SIZE  )TASK_MEASSPEED_STK_SIZE,                          /* �����ջ��С */
                 (OS_MSG_QTY    )10u,                                           /* �ڲ���Ϣ���е������Ϣ��Ŀ */
                 (OS_TICK       )0u,                                            /* ʱ��Ƭ��ѯ��ʱ��Ƭ�� */
                 (void         *)0u,                                            /* �û�����洢�� */
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 (OS_ERR       *)&os_err);                                      /* ��Ŵ���ֵ */
#endif
#if 1
     /* ��ѯ��������� */
     OSTaskCreate((OS_TCB       *)&TaskLaserTCB,                                /* ����������ƿ� */
                  (CPU_CHAR     *)"Laser Task",                                 /* �������� */
                  (OS_TASK_PTR   )Task_Laser,                                   /* ������ */
                  (void         *)0u,                                            /* ������� */
                  (OS_PRIO       )TASK_LASER_PRIO,                              /* �������ȼ� */
                  (CPU_STK      *)&TaskMeasSpeedStk[0u],                            /* ������ص�ַ */
                  (CPU_STK_SIZE  )TASK_LASER_STK_SIZE / 10u,                    /* ����ջ������ */
                  (CPU_STK_SIZE  )TASK_LASER_STK_SIZE,                          /* �����ջ��С */
                  (OS_MSG_QTY    )10u,                                           /* �ڲ���Ϣ���е������Ϣ��Ŀ */
                  (OS_TICK       )0u,                                            /* ʱ��Ƭ��ѯ��ʱ��Ƭ�� */
                  (void         *)0u,                                            /* �û�����洢�� */
                  (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                  (OS_ERR       *)&os_err);                                      /* ��Ŵ���ֵ */

#endif
#if 1
    /* ��ʾ������ */
    OSTaskCreate((OS_TCB       *)&TaskDisBoardTCB,                                /* ����������ƿ� */
                 (CPU_CHAR     *)"DisBorad Task",                                 /* �������� */
                 (OS_TASK_PTR   )Task_DisBoard,                                   /* ������ */
                 (void         *)0u,                                            /* ������� */
                 (OS_PRIO       )TASK_DISBOARD_PRIO,                              /* �������ȼ� */
                 (CPU_STK      *)&TaskDisBoardStk[0u],                            /* ������ص�ַ */
                 (CPU_STK_SIZE  )TASK_DISBOARD_STK_SIZE / 10u,                    /* ����ջ������ */
                 (CPU_STK_SIZE  )TASK_DISBOARD_STK_SIZE,                          /* �����ջ��С */
                 (OS_MSG_QTY    )0u,                                           /* �ڲ���Ϣ���е������Ϣ��Ŀ */
                 (OS_TICK       )0u,                                            /* ʱ��Ƭ��ѯ��ʱ��Ƭ�� */
                 (void         *)0u,                                            /* �û�����洢�� */
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 (OS_ERR       *)&os_err);                                      /* ��Ŵ���ֵ */
#endif


#if 0
    /* ���⴦������� */
    OSTaskCreate((OS_TCB       *)&TaskGasProcTCB,                               /* ����������ƿ� */
                 (CPU_CHAR     *)"Gas Process",                                 /* �������� */
                 (OS_TASK_PTR   )Task_GasProc,                                  /* ������ */
                 (void         *)0u,                                            /* ������� */
                 (OS_PRIO       )TASK_GASPROC_PRIO,                             /* �������ȼ� */
                 (CPU_STK      *)&TaskGasProcStk[0u],                           /* ������ص�ַ */
                 (CPU_STK_SIZE  )TASK_GASPROC_STK_SIZE / 10u,                   /* ����ջ������ */
                 (CPU_STK_SIZE  )TASK_GASPROC_STK_SIZE,                         /* �����ջ��С */
                 (OS_MSG_QTY    )10u,                                           /* �ڲ���Ϣ���е������Ϣ��Ŀ */
                 (OS_TICK       )0u,                                            /* ʱ��Ƭ��ѯ��ʱ��Ƭ�� */
                 (void         *)0u,                                            /* �û�����洢�� */
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 (OS_ERR       *)&os_err);                                      /* ��Ŵ���ֵ */
#endif
#if 1
    /* �Ҷȴ��� */
    OSTaskCreate((OS_TCB       *)&TaskGreyProcTCB,                              /* ����������ƿ� */
                 (CPU_CHAR     *)"Grey Process",                                /* �������� */
                 (OS_TASK_PTR   )Task_GreyProc,                                 /* ������ */
                 (void         *)0u,                                            /* ������� */
                 (OS_PRIO       )TASK_GREYPROC_PRIO,                            /* �������ȼ� */
                 (CPU_STK      *)&TaskGreyProcStk[0u],                          /* ������ص�ַ */
                 (CPU_STK_SIZE  )TASK_GREYPROC_STK_SIZE / 10u,                  /* ����ջ������ */
                 (CPU_STK_SIZE  )TASK_GREYPROC_STK_SIZE,                        /* �����ջ��С */
                 (OS_MSG_QTY    )10u,                                           /* �ڲ���Ϣ���е������Ϣ��Ŀ */
                 (OS_TICK       )0u,                                            /* ʱ��Ƭ��ѯ��ʱ��Ƭ�� */
                 (void         *)0u,                                            /* �û�����洢�� */
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 (OS_ERR       *)&os_err);                                      /* ��Ŵ���ֵ */

#endif
    /* ������ͨ�� */
    OSTaskCreate((OS_TCB       *)&TaskUsbHostTCB,                               /* ����������ƿ� */
                 (CPU_CHAR     *)"USB HOST",                                    /* �������� */
                 (OS_TASK_PTR   )Task_UsbHost,                                  /* ������ */
                 (void         *)0u,                                            /* ������� */
                 (OS_PRIO       )TASK_USB_HOST_PRIO,                            /* �������ȼ� */
                 (CPU_STK      *)&TaskUsbHostStk[0u],                           /* ������ص�ַ */
                 (CPU_STK_SIZE  )TASK_USB_HOST_STK_SIZE / 10u,                  /* ����ջ������ */
                 (CPU_STK_SIZE  )TASK_USB_HOST_STK_SIZE,                        /* �����ջ��С */
                 (OS_MSG_QTY    )100u,                                           /* �ڲ���Ϣ���е������Ϣ��Ŀ */
                 (OS_TICK       )0u,                                            /* ʱ��Ƭ��ѯ��ʱ��Ƭ�� */
                 (void         *)0u,                                            /* �û�����洢�� */
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 (OS_ERR       *)&os_err);                                      /* ��Ŵ���ֵ */
}

//==================================================================================================
//| �������� | AppObjCreate
//|----------|--------------------------------------------------------------------------------------
//| �������� | �����ں˶���
//|----------|--------------------------------------------------------------------------------------
//| ������� | ��
//|----------|--------------------------------------------------------------------------------------
//| ���ز��� | ��
//|----------|--------------------------------------------------------------------------------------
//| ������� | wjb
//==================================================================================================
void AppObjCreate (void)
{
    OS_ERR  os_err;
#if (OS_CFG_SEM_EN > 0u)
    OSSemCreate(&Sem_Rs485,
            "RS485 Port Lock",
             1u,
             &os_err);

    OSSemCreate(&Sem_Laser,
             "Laser Port Lock",
              1u,
              &os_err);
#endif

#if (OS_CFG_Q_EN > 0u)
    OSQCreate(&QCmlRecv,
              "Queue Command Line Recv",
               10,
              &os_err);

    OSQCreate(&QCmlTrans,
              "Queue Command Line Trans",
               10,
              &os_err);

    OSQCreate(&QSpeRecv,
              "Queue Spectrometer Recv",
               5,
              &os_err);

    OSQCreate(&QSpeTrans,
              "Queue Spectrometer Trans",
               5,
              &os_err);
#endif
}
