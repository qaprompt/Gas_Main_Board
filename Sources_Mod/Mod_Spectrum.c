//==================================================================================================
//| �ļ����� | Mod_Sperctrum.c
//|----------|--------------------------------------------------------------------------------------
//| �ļ����� | ���״��� 226.049nm 214.8nm 204.411nm
//|----------|--------------------------------------------------------------------------------------
//| ��Ȩ���� |
//|----------|--------------------------------------------------------------------------------------
//|  �汾    |  ʱ��       |  ����     | ����
//|----------|-------------|-----------|------------------------------------------------------------
//|  V1.0    | 2018.12.05  |  wjb      | ����
//==================================================================================================
#include "App_Include.h"

#define     DEF_SPECTRUM_DBG_EN           FALSE

#if (DEF_SPECTRUM_DBG_EN == TRUE)
    #define SPECTRUM_DBG(...)            do {                                \
                                            OS_ERR os_err;                  \
                                            OSSchedLock(&os_err);           \
                                            printf(__VA_ARGS__);            \
                                            OSSchedUnlock(&os_err);         \
                                        }while(0)
#else
    #define SPECTRUM_DBG(...)
#endif

FP64 alf_AbsSpectrum[3840] = {0.0};
FP64 alf_BkgSpectrum[3840] = {0.0};
FP64 alf_DiffSpectrum[3840] = {0.0};
FP64 alf_Spectrum[3840] = {0.0};

GasInfo_t st_GasN0 = {
    "NO",                                       /* �������� */
    eGasNO,                                     /* ������ */

    {0},                                        /* ���������շ�λ�� */
    {0},                                        /* ʵ������շ���Ϣ */

    &st_CPList_GasNO,                           /* �궨���б� */
    0.0,                                        /* �궨Ũ�� */

    DEF_CALIB_NIHE_ORDER_MAX,                   /* ��Ͻ��� */
    {0.0},                                      /* ������� */
    0.0,                                        /* Ũ�� */
};

GasMeasure_t st_GasMeasure = {      
    eGasAbsMeasure,                 /* ����״̬ */
                               
    NULL,                           /* �������豸 */
    
    TRUE,                          /* �洢������� */ 
    
    NULL,                           /* �������� */
    alf_AbsSpectrum,                /* ���Թ��� ������� */
    alf_BkgSpectrum,                /* �������� ��ֲ��� */
    alf_DiffSpectrum,               /* ��ֹ��� */
    alf_Spectrum,                   /* ���ڵĹ��� */

    0,                              /* ���׳��� */
    0,                              /* ʹ�õĹ��׷�Χ��߽� */
    3648,                           /* ʹ�õĹ��׷�Χ�ұ߽� */
                                    
    0.0,                            /* ͸���� */
    0.0,                            /* ͸����ϵ�� */
    10.0,                           /* ͸��������ֵ */   
    1200,                           /* ͸������ߵ� */
    1300,                           /* ͸�����ұߵ� */
                                    

    0.5,                            /* ����һ���˲�ϵ�� */
    0,                              /* ����ֵ */
    10,                             /* ͸���ʱ궨����ֵ */
    10,                             /* �������ֵ */
    10,                             /* У׼����ֵ */
                                    
    &st_GasN0,                      /* ����1 */
    NULL,                           /* ����2 */
    
};

static void InitSem(void)
{
    OS_ERR os_err;
    OSTaskSemSet(&TaskGasProcTCB,0U,&os_err);
}

static void PostSem(void)
{
    OS_ERR os_err;
    OSTaskSemPost(&TaskGasProcTCB,OS_OPT_POST_NONE,&os_err);
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
    OSTaskQPost(&TaskGasProcTCB,(void*)pv_Msg,1,OS_OPT_POST_FIFO ,&os_err);
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

void Mod_GasMeasureInit(GasMeasure_t* pst_Meas)
{    
    //570 650 730
    st_GasN0.st_PeakRef.ul_PeakLeftDot = 640;
    st_GasN0.st_PeakRef.ul_PeakCenterDot = 650;
    st_GasN0.st_PeakRef.ul_PeakRightDot = 660;
    
    st_GasN0.st_PeakRef.ul_LeftBackgroundLeftDot = 610;
    st_GasN0.st_PeakRef.ul_LeftBackgroundRightDot = 630;
    
    st_GasN0.st_PeakRef.ul_RightBackgroundLeftDot = 670;
    st_GasN0.st_PeakRef.ul_RightBackgroundRightDot = 690;

    if(pst_Meas->pst_Gas1 != NULL)
        Mod_CalibPointListInit(pst_Meas->pst_Gas1->pst_CalibPointList);
    if(pst_Meas->pst_Gas2 != NULL)
        Mod_CalibPointListInit(pst_Meas->pst_Gas2->pst_CalibPointList);
    
    
    InitSem();
}


FP32 Mod_GasMeasureCalBkgSpectrumTrans(GasMeasure_t* pst_Meas)
{
    FP64  lf_Sum1 = 0;
    FP64  lf_Sum2 = 0;
    FP32  f_Trans = 0;
    INT32U  i;
    for(i = pst_Meas->ul_TransLeftDot; i< pst_Meas->ul_TransRightDot; i++)
    {
        lf_Sum1 += pst_Meas->plf_AbsSpectrum[i];
        lf_Sum2 += pst_Meas->plf_BkgSpectrum[i];
    }

    f_Trans = lf_Sum2 / lf_Sum1;
    f_Trans = (f_Trans>1)? 1:f_Trans;
    return f_Trans;
}

FP32 Mod_GasMeasureCalSpectrumTrans(GasMeasure_t* pst_Meas)
{
    FP64  lf_Sum1 = 0;
    FP64  lf_Sum2 = 0;
    FP32  f_Trans = 0;
    INT32U  i;
    for(i = pst_Meas->ul_TransLeftDot; i< pst_Meas->ul_TransRightDot; i++)
    {
        lf_Sum1 += pst_Meas->plf_AbsSpectrum[i];
        lf_Sum2 += pst_Meas->plf_Spectrum[i];
    }

    f_Trans = lf_Sum2 / lf_Sum1;
    f_Trans = (f_Trans>1)? 1:f_Trans;
    return f_Trans;
}

void Mod_GasMeasureUpdataTrans(GasMeasure_t* pst_Meas)
{
    FP64  lf_Sum1 = 0;
    FP32  f_Trans = 0;
    INT32U  i;
    for(i = pst_Meas->ul_TransLeftDot; i< pst_Meas->ul_TransRightDot; i++)
    {
        lf_Sum1 += pst_Meas->plf_Spectrum[i];
    }
    pst_Meas->f_Trans = pst_Meas->f_TransK * lf_Sum1;
    pst_Meas->f_Trans = (pst_Meas->f_Trans>100.0)? 100.0:pst_Meas->f_Trans;
}



FP64 Mod_GasMeasureGetPeakHight(FP64* plf_Spectrum, GasInfo_t* pst_Gas)
{
    FP64    lf_Peak = 0;
    FP64    lf_BkgL = 0;
    FP64    lf_BkgR =0;
    INT32S   i,j;
    Peak_t* pst_Peak = &pst_Gas->st_PeakMeasure;

    /* ֱ�Ӹ��� �������������շ�߶� */
    memcpy(&pst_Gas->st_PeakMeasure, &pst_Gas->st_PeakRef, sizeof(Peak_t));
    SPECTRUM_DBG("NO���շ�ԭʼλ�ã�%d\r\n",pst_Peak->ul_PeakCenterDot,j);
#if 1
    /* �������ķ�Χ�ڵ���ߵ����շ�*/
    for(i = pst_Peak->ul_PeakLeftDot; i <=pst_Peak->ul_PeakRightDot; i++)
    {
        if(i == pst_Peak->ul_PeakLeftDot)
        {
            lf_Peak = plf_Spectrum[i];
            j = i;
        }
        else if(plf_Spectrum[i] > lf_Peak)   //lf_Peak = lf_Max
        {
            lf_Peak = plf_Spectrum[i];
            j = i;
        }
    }
    SPECTRUM_DBG("��� %d,�ұ� %d ����������%d",pst_Peak->ul_PeakLeftDot,pst_Peak->ul_PeakRightDot,j);
    j -= pst_Peak->ul_PeakCenterDot;

    pst_Peak->ul_PeakCenterDot  += j;
    pst_Peak->ul_PeakLeftDot    += j;
    pst_Peak->ul_PeakRightDot   += j;
    pst_Peak->ul_LeftBackgroundLeftDot  += j;
    pst_Peak->ul_LeftBackgroundRightDot += j;
    pst_Peak->ul_RightBackgroundLeftDot  += j;
    pst_Peak->ul_RightBackgroundRightDot += j;
#endif
    SPECTRUM_DBG("NO���շ�����λ�ã�%d ƫ������%d\r\n",pst_Peak->ul_PeakCenterDot,j);

    for(i = pst_Peak->ul_PeakLeftDot; i <=pst_Peak->ul_PeakRightDot; i++)    //�ۼƷ�߶�
        lf_Peak += plf_Spectrum[i];
    lf_Peak /= (pst_Peak->ul_PeakRightDot - pst_Peak->ul_PeakLeftDot + 1);   //���ƽ��

    for(i = pst_Peak->ul_LeftBackgroundLeftDot; i <= pst_Peak->ul_LeftBackgroundRightDot; i++)      //�ۼ��󱳾�
        lf_BkgL += plf_Spectrum[i];
    lf_BkgL /= (pst_Peak->ul_LeftBackgroundRightDot - pst_Peak->ul_LeftBackgroundLeftDot + 1);     //���ƽ��

    for(i = pst_Peak->ul_RightBackgroundLeftDot; i <= pst_Peak->ul_RightBackgroundRightDot; i++)    //�ۼ��ұ���
        lf_BkgR += plf_Spectrum[i];
    lf_BkgR /= (pst_Peak->ul_RightBackgroundRightDot - pst_Peak->ul_RightBackgroundLeftDot + 1);   //���ƽ��

    return  (lf_Peak - ((lf_BkgL+lf_BkgR)/2));
}

void Mod_GasMeasurePoll(GasMeasure_t* pst_Meas)
{
    INT32U  i;
    void *pv_Msg = PendMeg();        
    FP32 k;
    FP32 k1;
    
    if(pv_Msg == NULL)
        return;
    pst_Meas->pst_Dev = pv_Msg;
    USB4000_HandleTypeDef* USB4000_Handle = (USB4000_HandleTypeDef *) pst_Meas->pst_Dev;
    pst_Meas->ul_SpectrumLen = USB4000_Handle->uin_Pixels;

    /* �������׵���ǰ���� */
    for(i = 0; i < pst_Meas->ul_SpectrumLen; i++)
    {
        pst_Meas->plf_Spectrum[i] = USB4000_Handle->plf_ProcessSpectrum[i];
    }
#if 1  
    switch (pst_Meas->e_State)
    {
    case eGasCalibTrans: 
        /* ���Է�ֵУ׼ ͸���ʱ궨 */
        pst_Meas->f_Trans = 100.0f;
        
        /* һ���˲����±������� */
        for(i = 0; i < pst_Meas->ul_SpectrumLen; i++)
        {
            pst_Meas->plf_BkgSpectrum[i] = pst_Meas->plf_Spectrum[i] * (1 - pst_Meas->f_FilterCoeff) +
                                           pst_Meas->plf_BkgSpectrum[i] * pst_Meas->f_FilterCoeff;
        }
        
        /* �ۼ�͸���ʲ����ε�ֵ */
        for(i = pst_Meas->ul_TransLeftDot; i< pst_Meas->ul_TransRightDot; i++)
        {
            pst_Meas->f_TransK += pst_Meas->plf_BkgSpectrum[i];
        }

        if( ++pst_Meas->ul_Cnt >= pst_Meas->ul_CalibTransCnt )
        {
            pst_Meas->ul_Cnt = 0;

            pst_Meas->f_TransK /= pst_Meas->ul_CalibTransCnt;           //ȡ���ƽ��
            pst_Meas->f_TransK = 100.0 / pst_Meas->f_TransK;            //����͸����ϵ��
            
            
            /* �洢���ݵ�EEPROM */
            SaveToEeprom((INT32U)&pst_Meas->f_TransK);                  //�洢͸����ϵ��
            Mod_GasMeasureGotoAbsMeasure(pst_Meas);
        }
        break;
    case eGasAdjZero:
        /* ���� */
        Mod_GasMeasureUpdataTrans(pst_Meas);        //����͸����
        
        /*  һ���˲����±������� */
        for(i = 0; i < pst_Meas->ul_SpectrumLen; i++)
        {
            pst_Meas->plf_BkgSpectrum[i] = pst_Meas->plf_Spectrum[i] * (1 - pst_Meas->f_FilterCoeff) +
                                           pst_Meas->plf_BkgSpectrum[i] * pst_Meas->f_FilterCoeff;
        }

        if( ++pst_Meas->ul_Cnt >= pst_Meas->ul_AdjZeroCnt )
        {
            pst_Meas->ul_Cnt = 0;
            
            /* ���������״�����Թ��ײ�д��E2PROM */
            for(i = 0; i < pst_Meas->ul_SpectrumLen; i++)
            {
                pst_Meas->plf_AbsSpectrum[i] = pst_Meas->plf_BkgSpectrum[i];
            }

            CalibPoint_t st_CalibPoint = {TRUE,0.0,0.0};
            Mod_CalibPointListEditOnePoint(pst_Meas->pst_Gas1->pst_CalibPointList,0,&st_CalibPoint);
            
            if(pst_Meas->b_SaveAbsSpecetrum == TRUE)
            {
                OS_ERR os_err;
                OSTaskSuspend(&TaskUsbHostTCB,&os_err);     //������ײɼ�
                SaveToEepromExt((INT32U)pst_Meas->plf_AbsSpectrum,pst_Meas->ul_SpectrumLen);        //�洢������
                OSTaskResume(&TaskUsbHostTCB,&os_err);      //�ָ����ײɼ�
            }
            Mod_GasMeasureGotoAbsMeasure(pst_Meas);     
        }
        break;

    case eGasCalibGas1:
    case eGasCalibGas2:
    case eGasCalibAll:
        /* �궨 ��¼�궨��Ũ�� */
        Mod_GasMeasureUpdataTrans(pst_Meas);        //����͸����
        
        for(i = 0; i < pst_Meas->ul_SpectrumLen; i++)
        {
            pst_Meas->plf_BkgSpectrum[i] = pst_Meas->plf_Spectrum[i] * (1 - pst_Meas->f_FilterCoeff) +
                                           pst_Meas->plf_BkgSpectrum[i] * pst_Meas->f_FilterCoeff;  //���±�������
        }

        k = Mod_GasMeasureCalBkgSpectrumTrans(pst_Meas);    //����͸����

        for(i = pst_Meas->ul_UseLeftDot; i < pst_Meas->ul_UseRightDot; i++)
        {
            pst_Meas->plf_DiffSpectrum[i] = pst_Meas->plf_AbsSpectrum[i] - pst_Meas->plf_BkgSpectrum[i] / k;    //��ȥ���Ա��� ��ò�ֹ���
        }

        switch (pst_Meas->e_State)
        {
            case eGasCalibGas1:
                if(pst_Meas->pst_Gas1 == NULL)
                {
                    Mod_GasMeasureGotoAbsMeasure(pst_Meas);
                    break;
                }

                pst_Meas->pst_Gas1->lf_PeakHight += Mod_GasMeasureGetPeakHight(pst_Meas->plf_DiffSpectrum, pst_Meas->pst_Gas1);
                if( ++pst_Meas->ul_Cnt >= pst_Meas->ul_CalibCnt )
                {
                    pst_Meas->ul_Cnt = 0;
                    pst_Meas->pst_Gas1->lf_PeakHight /= pst_Meas->ul_CalibCnt;

                    CalibPoint_t st_CalibPoint;
                    st_CalibPoint.b_Use = TRUE;
                    st_CalibPoint.f_X   = pst_Meas->pst_Gas1->lf_PeakHight;
                    st_CalibPoint.f_Y   = pst_Meas->pst_Gas1->lf_Concentration;
                    /* �洢���ݵ�EEPROM */
                    //Mod_CalibPointListEditOnePoint(pst_Meas->pst_Gas1->pst_CalibPointList,1,&st_CalibPoint);
                    Mod_CalibPointListAddOnePoint(pst_Meas->pst_Gas1->pst_CalibPointList,&st_CalibPoint);
                    Mod_GasMeasureGotoAbsMeasure(pst_Meas);
                }
                break;
            case eGasCalibGas2:
                if(pst_Meas->pst_Gas2 == NULL)
                {
                    Mod_GasMeasureGotoAbsMeasure(pst_Meas);
                    break;
                }

                pst_Meas->pst_Gas2->lf_PeakHight += Mod_GasMeasureGetPeakHight(pst_Meas->plf_DiffSpectrum, pst_Meas->pst_Gas2);
                if( ++pst_Meas->ul_Cnt >= pst_Meas->ul_CalibCnt )
                {
                    pst_Meas->ul_Cnt = 0;
                    pst_Meas->pst_Gas2->lf_PeakHight /= pst_Meas->ul_CalibCnt;

                    CalibPoint_t st_CalibPoint;
                    st_CalibPoint.b_Use = TRUE;
                    st_CalibPoint.f_X   = pst_Meas->pst_Gas2->lf_PeakHight;
                    st_CalibPoint.f_Y   = pst_Meas->pst_Gas2->lf_Concentration;
                    /* �洢���ݵ�EEPROM */
                    Mod_CalibPointListEditOnePoint(pst_Meas->pst_Gas2->pst_CalibPointList,1,&st_CalibPoint);
                    Mod_GasMeasureGotoAbsMeasure(pst_Meas);
                }
                break;
            case eGasCalibAll:
                if(pst_Meas->pst_Gas1 == NULL || pst_Meas->pst_Gas2 == NULL)
                {
                    Mod_GasMeasureGotoAbsMeasure(pst_Meas);
                }

                pst_Meas->pst_Gas1->lf_PeakHight += Mod_GasMeasureGetPeakHight(pst_Meas->plf_DiffSpectrum, pst_Meas->pst_Gas1);
                pst_Meas->pst_Gas2->lf_PeakHight += Mod_GasMeasureGetPeakHight(pst_Meas->plf_DiffSpectrum, pst_Meas->pst_Gas2);

                if( ++pst_Meas->ul_Cnt >= pst_Meas->ul_CalibCnt )
                {
                    CalibPoint_t st_CalibPoint;

                    pst_Meas->ul_Cnt = 0;

                    pst_Meas->pst_Gas1->lf_PeakHight /= pst_Meas->ul_CalibCnt;
                    st_CalibPoint.b_Use = TRUE;
                    st_CalibPoint.f_X   = pst_Meas->pst_Gas1->lf_PeakHight;
                    st_CalibPoint.f_Y   = pst_Meas->pst_Gas1->lf_Concentration;
                    /* �洢���ݵ�EEPROM */
                    Mod_CalibPointListEditOnePoint(pst_Meas->pst_Gas1->pst_CalibPointList,1,&st_CalibPoint);

                    pst_Meas->pst_Gas2->lf_PeakHight /= pst_Meas->ul_CalibCnt;
                    st_CalibPoint.b_Use = TRUE;
                    st_CalibPoint.f_X   = pst_Meas->pst_Gas2->lf_PeakHight;
                    st_CalibPoint.f_Y   = pst_Meas->pst_Gas2->lf_Concentration;
                    /* �洢���ݵ�EEPROM */
                    Mod_CalibPointListEditOnePoint(pst_Meas->pst_Gas2->pst_CalibPointList,1,&st_CalibPoint);

                    Mod_GasMeasureGotoAbsMeasure(pst_Meas);
                }
                break;
            default:
                break;
        }
        break;
    case eGasWait:
        Mod_GasMeasureUpdataTrans(pst_Meas);                //����͸����
      
        k = Mod_GasMeasureCalSpectrumTrans(pst_Meas);       //�����ͨ��
        
        for(i = pst_Meas->ul_UseLeftDot; i < pst_Meas->ul_UseRightDot; i++)
        {
            pst_Meas->plf_DiffSpectrum[i] = pst_Meas->plf_AbsSpectrum[i] - pst_Meas->plf_Spectrum[i] / k;    //��ȥ���Ա��� ��ò�ֹ���
        }
        
        break;
        
    case eGasAbsMeasure:
        /* ���Բ��� */
        Mod_GasMeasureUpdataTrans(pst_Meas);        //����͸����
      
        k = Mod_GasMeasureCalSpectrumTrans(pst_Meas);       //�����ͨ��

        for(i = pst_Meas->ul_UseLeftDot; i < pst_Meas->ul_UseRightDot; i++)
        {
            pst_Meas->plf_DiffSpectrum[i] = pst_Meas->plf_AbsSpectrum[i] - pst_Meas->plf_Spectrum[i] / k;    //��ȥ���Ա��� ��ò�ֹ���
        }

        if(pst_Meas->pst_Gas1 != NULL)
        {
            FP64 f;
            pst_Meas->pst_Gas1->lf_PeakHight = Mod_GasMeasureGetPeakHight(pst_Meas->plf_DiffSpectrum, pst_Meas->pst_Gas1);
            f = s_fx(pst_Meas->pst_Gas1->af_NiheCoeff,pst_Meas->pst_Gas1->uch_NiheOrder,(FP32)pst_Meas->pst_Gas1->lf_PeakHight);
            f = (f < 0) ? 0:f;
            pst_Meas->pst_Gas1->lf_Concentration = f;
        }
        if(pst_Meas->pst_Gas2 != NULL)
        {
            FP64 f;
            pst_Meas->pst_Gas2->lf_PeakHight = Mod_GasMeasureGetPeakHight(pst_Meas->plf_DiffSpectrum, pst_Meas->pst_Gas2);
            f = s_fx(pst_Meas->pst_Gas2->af_NiheCoeff,pst_Meas->pst_Gas2->uch_NiheOrder,(FP32)pst_Meas->pst_Gas2->lf_PeakHight);
            f = (f < 0) ? 0:f;
            pst_Meas->pst_Gas2->lf_Concentration = f;
        }

        /* ���±������� */
        if(pst_Meas->f_Trans >= pst_Meas->f_TransThreshold)
        {
            for(i = 0; i < pst_Meas->ul_SpectrumLen; i++)
            {
                pst_Meas->plf_BkgSpectrum[i] = pst_Meas->plf_Spectrum[i] * (1 - pst_Meas->f_FilterCoeff) +
                                               pst_Meas->plf_BkgSpectrum[i] * pst_Meas->f_FilterCoeff;  //���±�������
            }
        }
        break;
    case eGasDiffMeasure:
        k1 = Mod_GasMeasureCalBkgSpectrumTrans(pst_Meas);       //���㱳�����׹�ͨ��
        k = Mod_GasMeasureCalSpectrumTrans(pst_Meas);           //���㵱ǰ���׹�ͨ��

        
        for(i = pst_Meas->ul_UseLeftDot; i < pst_Meas->ul_UseRightDot; i++)
        {
            pst_Meas->plf_DiffSpectrum[i] = pst_Meas->plf_BkgSpectrum[i] / k1 - pst_Meas->plf_Spectrum[i] / k;    //ȫ������ɵ�����׼���
        }

        if(pst_Meas->pst_Gas1 != NULL)
        {
            pst_Meas->pst_Gas1->lf_PeakHight = Mod_GasMeasureGetPeakHight(pst_Meas->plf_DiffSpectrum, pst_Meas->pst_Gas1);
            pst_Meas->pst_Gas1->lf_Concentration = s_fx(pst_Meas->pst_Gas1->af_NiheCoeff,pst_Meas->pst_Gas1->uch_NiheOrder,
                                                       (FP32)pst_Meas->pst_Gas1->lf_PeakHight);
            Mod_MeasureGasNOReply(pst_Meas->pst_Gas1->lf_Concentration);

        }
        if(pst_Meas->pst_Gas2 != NULL)
        {
            pst_Meas->pst_Gas2->lf_PeakHight = Mod_GasMeasureGetPeakHight(pst_Meas->plf_DiffSpectrum, pst_Meas->pst_Gas2);
            pst_Meas->pst_Gas2->lf_Concentration = s_fx(pst_Meas->pst_Gas2->af_NiheCoeff,pst_Meas->pst_Gas2->uch_NiheOrder,
                                                       (FP32)pst_Meas->pst_Gas2->lf_PeakHight);
            Mod_MeasureGasHCReply(pst_Meas->pst_Gas2->lf_Concentration);
        }

        break;
    default:
        break;
    }
#endif
}



BOOL Mod_GasMeasureGotoAdjZero(GasMeasure_t* pst_Meas)
{
    if(pst_Meas == NULL)
        return FALSE;
    pst_Meas->e_State = eGasAdjZero;
    pst_Meas->ul_Cnt = 0;
    return TRUE;
}


BOOL Mod_GasMeasureGotoCalibTrans(GasMeasure_t* pst_Meas)
{
    if(pst_Meas == NULL)
        return FALSE;
    pst_Meas->e_State = eGasCalibTrans;
    pst_Meas->ul_Cnt = 0;
    pst_Meas->f_TransK = 0;
    return TRUE;
}

BOOL Mod_GasMeasureGotoCalib(GasMeasure_t* pst_Meas,GasMeasureState_e e_State,FP64 lf_GasCon1,FP64 lf_GasCon2)
{
    if(pst_Meas == NULL)
        return FALSE;
    
    switch (e_State)
    {
    case eGasCalibGas1:
        if( pst_Meas->pst_Gas1 == NULL )
            return FALSE;
        pst_Meas->pst_Gas1->lf_Concentration = lf_GasCon1;
        pst_Meas->pst_Gas1->lf_PeakHight = 0.0;
        pst_Meas->ul_Cnt = 0;
        pst_Meas->e_State = eGasCalibGas1; 
        return TRUE;
    case eGasCalibGas2:
        if( pst_Meas->pst_Gas2 == NULL )
            return FALSE;
        pst_Meas->pst_Gas2->lf_Concentration = lf_GasCon2;
        pst_Meas->pst_Gas2->lf_PeakHight = 0.0;
        pst_Meas->ul_Cnt = 0;
        pst_Meas->e_State = eGasCalibGas2; 
        return TRUE;
    case eGasCalibAll:
        if( pst_Meas->pst_Gas1 == NULL || pst_Meas->pst_Gas2 == NULL )
            return FALSE;
        pst_Meas->pst_Gas1->lf_Concentration = lf_GasCon1;
        pst_Meas->pst_Gas1->lf_PeakHight = 0.0;
        pst_Meas->pst_Gas2->lf_Concentration = lf_GasCon2;
        pst_Meas->pst_Gas2->lf_PeakHight = 0.0;
        pst_Meas->ul_Cnt = 0;
        pst_Meas->e_State = eGasCalibAll; 
        return TRUE;
    default:
        return FALSE;
    }
}

BOOL Mod_GasMeasureGotoDiffMeasure(GasMeasure_t* pst_Meas)
{
    if(pst_Meas == NULL)
        return FALSE;
    pst_Meas->e_State = eGasDiffMeasure;
    return TRUE;
}

BOOL Mod_GasMeasureGotoAbsMeasure(GasMeasure_t* pst_Meas)
{
    if(pst_Meas == NULL)
        return FALSE;
    pst_Meas->e_State = eGasAbsMeasure;
    return TRUE;
}

BOOL Mod_GasMeasureGotoWait(GasMeasure_t* pst_Meas)
{
    if(pst_Meas == NULL)
        return FALSE;
    pst_Meas->e_State = eGasWait;
    return TRUE;
}


BOOL Mod_GasMarkWorkLine(GasMeasure_t* pst_Meas,GasMeasureState_e e_Ops)
{
    
    switch(e_Ops)
    {
    case eGasCalibGas1:
        if(st_GasMeasure.pst_Gas1 == NULL)
            return FALSE;
        st_GasMeasure.pst_Gas1->af_NiheCoeff[0] = 0;
        st_GasMeasure.pst_Gas1->af_NiheCoeff[1] = 0;
        st_GasMeasure.pst_Gas1->af_NiheCoeff[2] = 0;
        st_GasMeasure.pst_Gas1->af_NiheCoeff[3] = 0;
        st_GasMeasure.pst_Gas1->af_NiheCoeff[4] = 0;
        st_GasMeasure.pst_Gas1->af_NiheCoeff[5] = 0;      
        Mod_CalibPointListNihe(pst_Meas->pst_Gas1->pst_CalibPointList,
                               pst_Meas->pst_Gas1->uch_NiheOrder,
                               pst_Meas->pst_Gas1->af_NiheCoeff);
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas1->af_NiheCoeff[0]));
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas1->af_NiheCoeff[1]));
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas1->af_NiheCoeff[2]));
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas1->af_NiheCoeff[3]));
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas1->af_NiheCoeff[4]));
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas1->af_NiheCoeff[5]));
        TRACE_DBG(">>DBG:   ����1 ���ϵ��0:%f ���ϵ��1:%f ���ϵ��2:%f\r\n",
                    st_GasMeasure.pst_Gas1->af_NiheCoeff[0],
                    st_GasMeasure.pst_Gas1->af_NiheCoeff[1],
                    st_GasMeasure.pst_Gas1->af_NiheCoeff[2]);
        break;
    case eGasCalibGas2:
        if(st_GasMeasure.pst_Gas2 == NULL)
            return FALSE;
        st_GasMeasure.pst_Gas2->af_NiheCoeff[0] = 0;
        st_GasMeasure.pst_Gas2->af_NiheCoeff[1] = 0;
        st_GasMeasure.pst_Gas2->af_NiheCoeff[2] = 0;
        st_GasMeasure.pst_Gas2->af_NiheCoeff[3] = 0;
        st_GasMeasure.pst_Gas2->af_NiheCoeff[4] = 0;
        st_GasMeasure.pst_Gas2->af_NiheCoeff[5] = 0;
        Mod_CalibPointListNihe(pst_Meas->pst_Gas2->pst_CalibPointList,
                               pst_Meas->pst_Gas2->uch_NiheOrder,
                               pst_Meas->pst_Gas2->af_NiheCoeff);
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas2->af_NiheCoeff[0]));
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas2->af_NiheCoeff[1]));
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas2->af_NiheCoeff[2]));
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas2->af_NiheCoeff[3]));
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas2->af_NiheCoeff[4]));
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas2->af_NiheCoeff[5]));
        TRACE_DBG(">>DBG:   ����2 ���ϵ��0:%f ���ϵ��1:%f ���ϵ��2:%f\r\n",
                    st_GasMeasure.pst_Gas2->af_NiheCoeff[0],
                    st_GasMeasure.pst_Gas2->af_NiheCoeff[1],
                    st_GasMeasure.pst_Gas2->af_NiheCoeff[2]);
        break;                                      
    case eGasCalibAll:
        if(st_GasMeasure.pst_Gas1 == NULL)
            return FALSE;
        if(st_GasMeasure.pst_Gas2 == NULL)
            return FALSE;
        Mod_CalibPointListNihe(pst_Meas->pst_Gas1->pst_CalibPointList,
                               pst_Meas->pst_Gas1->uch_NiheOrder,
                               pst_Meas->pst_Gas1->af_NiheCoeff);
        Mod_CalibPointListNihe(pst_Meas->pst_Gas2->pst_CalibPointList,
                               pst_Meas->pst_Gas2->uch_NiheOrder,
                               pst_Meas->pst_Gas2->af_NiheCoeff);
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas1->af_NiheCoeff[0]));
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas1->af_NiheCoeff[1]));
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas1->af_NiheCoeff[2]));
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas2->af_NiheCoeff[0]));
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas2->af_NiheCoeff[1]));
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas2->af_NiheCoeff[2]));
        TRACE_DBG(">>DBG:   ����1 ���ϵ��0:%f ���ϵ��1:%f ���ϵ��2:%f\r\n",
                    st_GasMeasure.pst_Gas1->af_NiheCoeff[0],
                    st_GasMeasure.pst_Gas1->af_NiheCoeff[1],
                    st_GasMeasure.pst_Gas1->af_NiheCoeff[2]);
        TRACE_DBG(">>DBG:   ����2 ���ϵ��0:%f ���ϵ��1:%f ���ϵ��2:%f\r\n",
                    st_GasMeasure.pst_Gas2->af_NiheCoeff[0],
                    st_GasMeasure.pst_Gas2->af_NiheCoeff[1],
                    st_GasMeasure.pst_Gas2->af_NiheCoeff[2]);
        break;
    default:
        return FALSE;
        
    }
    return TRUE;
}

void USB4000_EvnetHandle(USB4000_HandleTypeDef *USB4000_Handle)
{
    /* ֪ͨ�ϲ� ����ģ��*/
    OS_ERR  os_err;
    OSTaskQPost (&TaskGasProcTCB,
                 (void*)USB4000_Handle,
                 sizeof(USB4000_Handle),
                 OS_OPT_POST_FIFO,
                 &os_err);
}

__weak void Mod_MeasureGasHCReply(FP64 lf_Concentration)
{

}

__weak void Mod_MeasureGasNOReply(FP64 lf_Concentration)
{

}
