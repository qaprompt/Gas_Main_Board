#include "App_Include.h"

#define DEF_LASER_DBG_EN    FALSE

#if (DEF_LASER_DBG_EN == TRUE)
    #define LASER_DBG(...)             do {                                \
                                            OS_ERR os_err;                  \
                                            OSSchedLock(&os_err);           \
                                            printf(__VA_ARGS__);            \
                                            OSSchedUnlock(&os_err);         \
                                        }while(0)
#else
    #define LASER_DBG(...)             
#endif

BOOL Mod_MeasLaserDealFram(StdbusFram_t* pst_Fram);

StdbusDev_t st_LaserBoard = {
    "�����",                               /* �豸���� */
    {0,0x20},                               /*��ַ�б�*/
    2,                                      /*��ַ�б���*/
    NULL,                                   /*�˿ھ��*/
    Mod_MeasLaserDealFram,                  /*������*/
};

LaserBoard_t st_Laser = {
    0,                                      /* ״̬ */
    0,                                      /* ���Լ��� */
    0,                                      /* ͸���� */
    0,                                      /* CO2��ֵ */
    0,                                      /* CO2Ũ�� */
    0,                                      /* CO��ֵ */
    0,                                      /* COŨ�� */
    &st_LaserBoard,
};


BOOL Mod_LaserGetStatus(LaserBoard_t* pst_Laser);

static void InitSem(void)
{
    OS_ERR os_err;
    //OSTaskSemSet(&TaskLaserTCB,0U,&os_err);
    OSSemSet(&Sem_LaserRecv,0,&os_err);
}

static void PostSem(void)
{
    OS_ERR os_err;
    OSSemPost(&Sem_LaserRecv,OS_OPT_POST_1,&os_err);                     /* ����Ӧ�� */
}

static BOOL PendSem(INT32U ul_Time)
{
    OS_ERR os_err;
    
    OSSemPend(&Sem_LaserRecv, ul_Time,OS_OPT_PEND_BLOCKING,NULL,&os_err);  /* �ȴ�Ӧ�� */
    
    if (os_err != OS_ERR_NONE)
        return FALSE;
    else
        return TRUE;
}

//==================================================================================
//| �������� | Mod_LaserInit
//|----------|----------------------------------------------------------------------
//| �������� | �������Ҫ�������� ��ͣ���Ͳ�ѯŨ�ȵ�����
//|----------|----------------------------------------------------------------------
//| ������� | ��Ϣ�ľ��
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
void Mod_LaserInit(void)
{
    InitSem();
}

//==================================================================================
//| �������� | Mod_LaserPoll
//|----------|----------------------------------------------------------------------
//| �������� | �������Ҫ�������� ��ͣ���Ͳ�ѯŨ�ȵ�����
//|----------|----------------------------------------------------------------------
//| ������� | ��Ϣ�ľ��
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
void Mod_LaserPoll(void)
{
    OS_ERR  os_err;
  
    OSTimeDlyHMSM(0u, 0u, 1u, 0u,
                OS_OPT_TIME_HMSM_STRICT,
                &os_err);
    Mod_LaserGetStatus(&st_Laser);
}

BOOL Mod_MeasLaserDealFram(StdbusFram_t* pst_Fram)
{
    if (pst_Fram == NULL)
        return FALSE;
    switch(pst_Fram->uch_Cmd)
    {
    case 0x80:
        //��ȡ״̬
        if(pst_Fram->uch_SubCmd == e_StdbusReadAck)
        {
            //������
            // 0-4���Լ��� 4-8 ͸���� 8-16 CO2Ũ�� 9-13 CO1Ũ��
            if(pst_Fram->uin_PayLoadLenth == 25)
            {
                st_Laser.uch_State = pst_Fram->puc_PayLoad[0];
                st_Laser.ul_Count = Bsp_CnvArrToINT32U(&pst_Fram->puc_PayLoad[1],FALSE);
                st_Laser.f_Trans  = Bsp_CnvArrToFP32(&pst_Fram->puc_PayLoad[5],FALSE);
                st_Laser.lf_PeakCO2             = (FP64)Bsp_CnvArrToFP32(&pst_Fram->puc_PayLoad[9],FALSE);
                st_Laser.lf_ConcentrationCO2    = (FP64)Bsp_CnvArrToFP32(&pst_Fram->puc_PayLoad[13],FALSE);
                st_Laser.lf_PeakCO              = (FP64)Bsp_CnvArrToFP32(&pst_Fram->puc_PayLoad[17],FALSE);
                st_Laser.lf_ConcentrationCO     = (FP64)Bsp_CnvArrToFP32(&pst_Fram->puc_PayLoad[21],FALSE);
                
                LASER_DBG(">>LASER_DBG: ��ȡ�������״̬\r\n");
                
                PostSem();
            }
  
        }
        else if(pst_Fram->uch_SubCmd == e_StdbusWriteAck)
        {

        }
        break;
    case 0xA4:
        //��ȡCO2������
        if(pst_Fram->uch_SubCmd == e_StdbusReadAck)
        {
            //������
            // 0-1���Ե���� 2-x Ũ������
            INT16U  len =  Bsp_CnvArrToINT16U(&pst_Fram->puc_PayLoad[0],FALSE);
          
            if( ((pst_Fram->uin_PayLoadLenth - 2) / 4) == len)
            {
                for(int i = 0; i < len ; i++)
                    Mod_LaserCO2Notification(Bsp_CnvArrToFP32(&pst_Fram->puc_PayLoad[i*4+2],FALSE));
            }
            LASER_DBG(">>LASER_DBG: ��ȡ��CO2������\r\n");
            PostSem();
        }
        else if(pst_Fram->uch_SubCmd == e_StdbusWriteAck)
        {

        }
        break;
    case 0xA5:
        //��ȡCO������
        if(pst_Fram->uch_SubCmd == e_StdbusReadAck)
        {
            //������
            // 0-1���Ե���� 2-x Ũ������
            INT16U  len =  Bsp_CnvArrToINT16U(&pst_Fram->puc_PayLoad[0],FALSE);
          
            if( ((pst_Fram->uin_PayLoadLenth - 2) / 4) == len)
            {
                for(int i = 0; i < len ; i++)
                    Mod_LaserCONotification(Bsp_CnvArrToFP32(&pst_Fram->puc_PayLoad[i*4+2],FALSE));

            }
            LASER_DBG(">>LASER_DBG: ��ȡ��CO������\r\n");
            PostSem();
        }
        else if(pst_Fram->uch_SubCmd == e_StdbusWriteAck)
        {

        }
        break;    
        
    case 0xA6:
        if(pst_Fram->uch_SubCmd == e_StdbusReadAck)
        {
            //������
            // 0-1���Ե���� 2-x Ũ������
            INT16U  len =  Bsp_CnvArrToINT16U(&pst_Fram->puc_PayLoad[0],FALSE);
          
            if( pst_Fram->uin_PayLoadLenth == 8)
            {
                FP64 f;
                f = Bsp_CnvArrToFP32(&pst_Fram->puc_PayLoad[0],FALSE);
                st_Measure.lf_CO2 = f * 1000000;
                f = Bsp_CnvArrToFP32(&pst_Fram->puc_PayLoad[4],FALSE);
                st_Measure.lf_CO = f * 1000000;
            }
            
            PostSem();
        }
        else if(pst_Fram->uch_SubCmd == e_StdbusWriteAck)
        {

        }
        break; 
        
    }
    return FALSE;
}

BOOL Mod_LaserGetStatus(LaserBoard_t* pst_Laser)
{
    INT16U  i = 0;
    if (pst_Laser == NULL)
        return FALSE;
    
    while(i < 3)
    {
        Mod_StdbusReadCmd(&st_LaserBoard,0x80,NULL,0);
        if(PendSem(100) == TRUE)
        {
            LASER_DBG(">>LASER_DBG: ��ȡ�����״̬�ɹ�\r\n");
            return TRUE;
        }
    }
    LASER_DBG(">>LASER_DBG: ��ȡ�����״̬ʧ��\r\n");
    return FALSE;
}

BOOL Mod_LaserRequestGasAvg(LaserBoard_t* pst_Laser)
{
    INT16U  i = 0;
    if (pst_Laser == NULL)
        return FALSE;
    
    while(i < 3)
    {
        Mod_StdbusReadCmd(&st_LaserBoard,0xA6,NULL,0);
        if(PendSem(100) == TRUE)
        {
            LASER_DBG(">>LASER_DBG: ��ȡCO2COŨ��ƽ��ֵ�ɹ�\r\n");
            return TRUE;
        }
    }
    LASER_DBG(">>LASER_DBG: ��ȡCO2COŨ��ƽ��ֵʧ��\r\n");
    return FALSE;
}

BOOL Mod_LaserRequestCO2SampleDot(LaserBoard_t* pst_Laser)
{
    INT16U  i = 0;
  
    if (pst_Laser == NULL)
        return FALSE;
    
    while(i < 3)
    {
        Mod_StdbusReadCmd(&st_LaserBoard,0xA4,NULL,0);
        if(PendSem(100) == TRUE)
        {
            LASER_DBG(">>LASER_DBG: ��ȡCO2������ɹ�\r\n");
            return TRUE;
        }
    }
    LASER_DBG(">>LASER_DBG: ��ȡCO2������ʧ��\r\n");
    return FALSE;
}

BOOL Mod_LaserRequestCOSampleDot(LaserBoard_t* pst_Laser)
{
    INT16U i = 0;
    
    if (pst_Laser == NULL)
        return FALSE;
    
    while(i < 3)
    {
        Mod_StdbusReadCmd(&st_LaserBoard,0xA5,NULL,0);
        if(PendSem(100) == TRUE)
        {
            LASER_DBG(">>LASER_DBG: ��ȡCO������ɹ�\r\n");
            return TRUE;
        }
    }
    LASER_DBG(">>LASER_DBG: ��ȡCO������ʧ��\r\n");
    return FALSE;
}

__weak void Mod_LaserReply(LaserBoard_t* pst_Laser)
{

}

__weak void Mod_LaserCO2Notification(FP32 f_Concentration)
{

}

__weak void Mod_LaserCONotification(FP32 f_Concentration)
{
  
}


