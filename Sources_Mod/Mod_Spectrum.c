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
                                          

FP32 af_ZeroSpectrum[3648] = {0.0};
FP32 af_BkgSpectrum[3648] = {0.0};
FP32 af_DiffSpectrum[3648] = {0.0};
FP32 af_Spectrum[3648] = {0.0};
FP32 af_PrcoSpectrum[3648] = {0.0};

GasInfo_t st_GasNO = {
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

GasInfo_t st_GasHC = {
    "HC",                                       /* �������� */
    eGasHC,                                     /* ������ */

    {0},                                        /* ���������շ�λ�� */
    {0},                                        /* ʵ������շ���Ϣ */

    &st_CPList_GasHC,                           /* �궨���б� */
    0.0,                                        /* �궨Ũ�� */

    DEF_CALIB_NIHE_ORDER_MAX,                   /* ��Ͻ��� */
    {0.0},                                      /* ������� */
    0.0,                                        /* Ũ�� */
};

GasMeasure_t st_GasMeasure = {      
    eGasAbsMeasure,                 /* ����״̬ */
                               
    NULL,                           /* �������豸 */
    
    TRUE,                           /* �洢������� */ 
    
    NULL,                           /* �������� */
    
    af_Spectrum,                    /* ���ڵĹ��� */    
    af_PrcoSpectrum,                /* ������� */
    af_ZeroSpectrum,                /* ������� */
    af_BkgSpectrum,                 /* �������� ��ֲ��� */
    af_DiffSpectrum,                /* ��ֹ��� */
    


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
    10,                             /* ƽ������ֵ */
                                    
    &st_GasNO,                      /* ����1 */
    &st_GasHC,                      /* ����2 */
    
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

//==================================================================================
//| �������� | Mod_GasMeasureInit
//|----------|----------------------------------------------------------------------
//| �������� | ��ʼ��
//|----------|----------------------------------------------------------------------
//| ������� | ��
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
void Mod_GasMeasureInit(GasMeasure_t* pst_Meas)
{    
    INT16U i;
    //570 650 730
    st_GasNO.st_PeakRef.ul_PeakLeftDot = 640;
    st_GasNO.st_PeakRef.ul_PeakCenterDot = 650;
    st_GasNO.st_PeakRef.ul_PeakRightDot = 660;
           
    st_GasNO.st_PeakRef.ul_LeftBackgroundLeftDot = 610;
    st_GasNO.st_PeakRef.ul_LeftBackgroundRightDot = 630;
           
    st_GasNO.st_PeakRef.ul_RightBackgroundLeftDot = 670;
    st_GasNO.st_PeakRef.ul_RightBackgroundRightDot = 690;
    
    
    st_GasHC.st_PeakRef.ul_PeakLeftDot = 799;
    st_GasHC.st_PeakRef.ul_PeakCenterDot = 919;
    st_GasHC.st_PeakRef.ul_PeakRightDot = 939;

    st_GasHC.st_PeakRef.ul_LeftBackgroundLeftDot = 185;
    st_GasHC.st_PeakRef.ul_LeftBackgroundRightDot = 224;

    st_GasHC.st_PeakRef.ul_RightBackgroundLeftDot = 2096;
    st_GasHC.st_PeakRef.ul_RightBackgroundRightDot = 2143;

    if(pst_Meas->pst_Gas1 != NULL)
        Mod_CalibPointListInit(pst_Meas->pst_Gas1->pst_CalibPointList);
    if(pst_Meas->pst_Gas2 != NULL)
        Mod_CalibPointListInit(pst_Meas->pst_Gas2->pst_CalibPointList);
    
    for(i = 0; i < DEF_CALIB_NIHE_ORDER_MAX+1; i++)
    {
        SPECTRUM_DBG("���ϵ��[%d] = %e\r\n",i,pst_Meas->pst_Gas1->af_NiheCoeff[i]); 
    }
    
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
        lf_Sum1 += pst_Meas->pf_ZeroSpectrum[i];
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
        lf_Sum1 += pst_Meas->pf_ZeroSpectrum[i];
        lf_Sum2 += pst_Meas->plf_Spectrum[i];
    }

    f_Trans = lf_Sum2 / lf_Sum1;
    f_Trans = (f_Trans>1)? 1:f_Trans;
    return f_Trans;
}

FP64 Mod_GasMeasureUpdataTrans(GasMeasure_t* pst_Meas)
{
    FP64  lf_Sum = 0;
    INT32U  i;
    for(i = pst_Meas->ul_TransLeftDot; i < pst_Meas->ul_TransRightDot; i++)
    {
        lf_Sum += pst_Meas->plf_Spectrum[i];
    }
    lf_Sum /= (pst_Meas->ul_TransRightDot - pst_Meas->ul_TransLeftDot);
    pst_Meas->f_Trans = pst_Meas->f_TransK * lf_Sum;
    pst_Meas->f_Trans = (pst_Meas->f_Trans>100.0)? 100.0:pst_Meas->f_Trans;
    return lf_Sum;
}



FP64 Mod_GasMeasureGetPeakHight(FP32* plf_Spectrum, GasInfo_t* pst_Gas)
{
    FP64    lf_Peak = 0;
    FP64    lf_BkgL = 0;
    FP64    lf_BkgR =0;
    INT32S   i,j;
    Peak_t* pst_Peak = &pst_Gas->st_PeakMeasure;

    /* ֱ�Ӹ��� �������������շ�߶� */
    memcpy(&pst_Gas->st_PeakMeasure, &pst_Gas->st_PeakRef, sizeof(Peak_t));
    SPECTRUM_DBG("NO���շ�ԭʼλ�ã�%d\r\n",pst_Peak->ul_PeakCenterDot);
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

void Mod_GasMeasureAdjZero(GasMeasure_t* pst_Meas)
{
    INT16U  i;
    
    /* ���� */
    
    /* һ���˲����±������� */
    for(i = 0; i < pst_Meas->ul_SpectrumLen; i++)
    {
        pst_Meas->plf_BkgSpectrum[i] = pst_Meas->pf_ProcSpectrum[i] * (1 - pst_Meas->f_FilterCoeff) +
                                       pst_Meas->plf_BkgSpectrum[i] * pst_Meas->f_FilterCoeff;
    }

    if( ++pst_Meas->ul_Cnt >= pst_Meas->ul_ScanAvg )
    {
        pst_Meas->ul_Cnt = 0;
        
        /* ���������״�����Թ��ײ�д��E2PROM */
        for(i = 0; i < pst_Meas->ul_SpectrumLen; i++)
        {
            pst_Meas->pf_ZeroSpectrum[i] = pst_Meas->plf_BkgSpectrum[i];
        }

        CalibPoint_t st_CalibPoint = {TRUE,0.0,0.0};
        Mod_CalibPointListEditOnePoint(pst_Meas->pst_Gas1->pst_CalibPointList,0,&st_CalibPoint);    //������
        Mod_CalibPointListEditOnePoint(pst_Meas->pst_Gas2->pst_CalibPointList,0,&st_CalibPoint);    //������
        if(pst_Meas->b_SaveZeroSpecetrum == TRUE)
        {
            OS_ERR os_err;
            OSTaskSuspend(&TaskUsbHostTCB,&os_err);     //������ײɼ�
            SaveToEepromExt((INT32U)pst_Meas->pf_ZeroSpectrum,pst_Meas->ul_SpectrumLen);            //�洢������
            OSTaskResume(&TaskUsbHostTCB,&os_err);      //�ָ����ײɼ�
        }
        Mod_GasMeasureDoAbsMeasure(pst_Meas);     
    }
}

void Mod_GasMeasureCalibGas1(GasMeasure_t* pst_Meas)
{
    INT16U  i;
    if(pst_Meas->pst_Gas1 == NULL)
    {
        Mod_GasMeasureDoAbsMeasure(pst_Meas);
        return;
    }

    for(i = 0; i < pst_Meas->ul_SpectrumLen; i++)
    {
        pst_Meas->plf_BkgSpectrum[i] = pst_Meas->pf_ProcSpectrum[i] * (1 - pst_Meas->f_FilterCoeff) +
                                       pst_Meas->plf_BkgSpectrum[i] * pst_Meas->f_FilterCoeff;  //���±�������
    }

    for(i = pst_Meas->ul_UseLeftDot; i < pst_Meas->ul_UseRightDot; i++)
    {
        pst_Meas->plf_DiffSpectrum[i] = pst_Meas->pf_ZeroSpectrum[i] - pst_Meas->plf_BkgSpectrum[i];    //��ȥ���Ա��� ��ò�ֹ���
    }

    pst_Meas->pst_Gas1->lf_PeakHight += Mod_GasMeasureGetPeakHight(pst_Meas->plf_DiffSpectrum, pst_Meas->pst_Gas1);
    if( ++pst_Meas->ul_Cnt >= pst_Meas->ul_ScanAvg )
    {
        pst_Meas->ul_Cnt = 0;
        pst_Meas->pst_Gas1->lf_PeakHight /= pst_Meas->ul_ScanAvg;

        CalibPoint_t st_CalibPoint;
        st_CalibPoint.b_Use = TRUE;
        st_CalibPoint.f_X   = pst_Meas->pst_Gas1->lf_PeakHight;
        st_CalibPoint.f_Y   = pst_Meas->pst_Gas1->lf_Concentration;
        /* �洢���ݵ�EEPROM */
        //Mod_CalibPointListEditOnePoint(pst_Meas->pst_Gas1->pst_CalibPointList,1,&st_CalibPoint);
        Mod_CalibPointListAddOnePoint(pst_Meas->pst_Gas1->pst_CalibPointList,&st_CalibPoint);
        Mod_GasMeasureDoAbsMeasure(pst_Meas);
    }
}

void Mod_GasMeasureCalibGas2(GasMeasure_t* pst_Meas)
{
    INT16U  i;
    if(pst_Meas->pst_Gas2 == NULL)
    {
        Mod_GasMeasureDoAbsMeasure(pst_Meas);
        return;
    }
    
    for(i = 0; i < pst_Meas->ul_SpectrumLen; i++)
    {
        pst_Meas->plf_BkgSpectrum[i] = pst_Meas->pf_ProcSpectrum[i] * (1 - pst_Meas->f_FilterCoeff) +
                                       pst_Meas->plf_BkgSpectrum[i] * pst_Meas->f_FilterCoeff;  //���±�������
    }

    for(i = pst_Meas->ul_UseLeftDot; i < pst_Meas->ul_UseRightDot; i++)
    {
        pst_Meas->plf_DiffSpectrum[i] = pst_Meas->pf_ZeroSpectrum[i] - pst_Meas->plf_BkgSpectrum[i];    //��ȥ���Ա��� ��ò�ֹ���
    }

    pst_Meas->pst_Gas2->lf_PeakHight += Mod_GasMeasureGetPeakHight(pst_Meas->plf_DiffSpectrum, pst_Meas->pst_Gas2);
    if( ++pst_Meas->ul_Cnt >= pst_Meas->ul_ScanAvg )
    {
        pst_Meas->ul_Cnt = 0;
        pst_Meas->pst_Gas2->lf_PeakHight /= pst_Meas->ul_ScanAvg;

        CalibPoint_t st_CalibPoint;
        st_CalibPoint.b_Use = TRUE;
        st_CalibPoint.f_X   = pst_Meas->pst_Gas2->lf_PeakHight;
        st_CalibPoint.f_Y   = pst_Meas->pst_Gas2->lf_Concentration;
        /* �洢���ݵ�EEPROM */
        Mod_CalibPointListAddOnePoint(pst_Meas->pst_Gas2->pst_CalibPointList,&st_CalibPoint);
        Mod_GasMeasureDoAbsMeasure(pst_Meas);
    }
}

void Mod_GasMeasureCalibGasAll(GasMeasure_t* pst_Meas)
{
    INT16U  i;
    if(pst_Meas->pst_Gas1 == NULL || pst_Meas->pst_Gas2 == NULL)
    {
        Mod_GasMeasureDoAbsMeasure(pst_Meas);
    }
    
    for(i = 0; i < pst_Meas->ul_SpectrumLen; i++)
    {
        pst_Meas->plf_BkgSpectrum[i] = pst_Meas->pf_ProcSpectrum[i] * (1 - pst_Meas->f_FilterCoeff) +
                                       pst_Meas->plf_BkgSpectrum[i] * pst_Meas->f_FilterCoeff;  //���±�������
    }

    for(i = pst_Meas->ul_UseLeftDot; i < pst_Meas->ul_UseRightDot; i++)
    {
        pst_Meas->plf_DiffSpectrum[i] = pst_Meas->pf_ZeroSpectrum[i] - pst_Meas->plf_BkgSpectrum[i];    //��ȥ���Ա��� ��ò�ֹ���
    }

    pst_Meas->pst_Gas1->lf_PeakHight += Mod_GasMeasureGetPeakHight(pst_Meas->plf_DiffSpectrum, pst_Meas->pst_Gas1);
    pst_Meas->pst_Gas2->lf_PeakHight += Mod_GasMeasureGetPeakHight(pst_Meas->plf_DiffSpectrum, pst_Meas->pst_Gas2);

    if( ++pst_Meas->ul_Cnt >= pst_Meas->ul_ScanAvg )
    {
        CalibPoint_t st_CalibPoint;

        pst_Meas->ul_Cnt = 0;

        pst_Meas->pst_Gas1->lf_PeakHight /= pst_Meas->ul_ScanAvg;
        st_CalibPoint.b_Use = TRUE;
        st_CalibPoint.f_X   = pst_Meas->pst_Gas1->lf_PeakHight;
        st_CalibPoint.f_Y   = pst_Meas->pst_Gas1->lf_Concentration;
        /* �洢���ݵ�EEPROM */
        Mod_CalibPointListEditOnePoint(pst_Meas->pst_Gas1->pst_CalibPointList,1,&st_CalibPoint);

        pst_Meas->pst_Gas2->lf_PeakHight /= pst_Meas->ul_ScanAvg;
        st_CalibPoint.b_Use = TRUE;
        st_CalibPoint.f_X   = pst_Meas->pst_Gas2->lf_PeakHight;
        st_CalibPoint.f_Y   = pst_Meas->pst_Gas2->lf_Concentration;
        /* �洢���ݵ�EEPROM */
        Mod_CalibPointListEditOnePoint(pst_Meas->pst_Gas2->pst_CalibPointList,1,&st_CalibPoint);

        Mod_GasMeasureDoAbsMeasure(pst_Meas);
    }
}

void Mod_GasMeasureCorrectionGas1(GasMeasure_t* pst_Meas)
{
    INT16U  i;
    if(pst_Meas->pst_Gas1 == NULL)
    {
        Mod_GasMeasureDoAbsMeasure(pst_Meas);
        return;
    }

    for(i = 0; i < pst_Meas->ul_SpectrumLen; i++)
    {
        pst_Meas->plf_BkgSpectrum[i] = pst_Meas->pf_ProcSpectrum[i] * (1 - pst_Meas->f_FilterCoeff) +
                                       pst_Meas->plf_BkgSpectrum[i] * pst_Meas->f_FilterCoeff;  //���±�������
    }

    for(i = pst_Meas->ul_UseLeftDot; i < pst_Meas->ul_UseRightDot; i++)
    {
        pst_Meas->plf_DiffSpectrum[i] = pst_Meas->pf_ZeroSpectrum[i] - pst_Meas->plf_BkgSpectrum[i];    //��ȥ���Ա��� ��ò�ֹ���
    }

    pst_Meas->pst_Gas1->lf_PeakHight += Mod_GasMeasureGetPeakHight(pst_Meas->plf_DiffSpectrum, pst_Meas->pst_Gas1);
    if( ++pst_Meas->ul_Cnt >= pst_Meas->ul_ScanAvg )
    {
        pst_Meas->ul_Cnt = 0;
        pst_Meas->pst_Gas1->lf_PeakHight /= pst_Meas->ul_ScanAvg;

        FP64 f;
        pst_Meas->pst_Gas1->lf_PeakHight = Mod_GasMeasureGetPeakHight(pst_Meas->plf_DiffSpectrum, pst_Meas->pst_Gas1);
        pst_Meas->pst_Gas1->lf_PeakHight = (pst_Meas->pst_Gas1->lf_PeakHight < 0) ? 0:pst_Meas->pst_Gas1->lf_PeakHight;
        f = s_fx(pst_Meas->pst_Gas1->af_NiheCoeff,pst_Meas->pst_Gas1->uch_NiheOrder,(FP32)pst_Meas->pst_Gas1->lf_PeakHight);
        pst_Meas->pst_Gas1->f_Correction = pst_Meas->pst_Gas1->lf_Concentration / f;
        
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas1->f_Correction));
        
        Mod_GasMeasureDoAbsMeasure(pst_Meas);
    }
}

void Mod_GasMeasureCorrectionGas2(GasMeasure_t* pst_Meas)
{
    INT16U  i;
    if(pst_Meas->pst_Gas1 == NULL)
    {
        Mod_GasMeasureDoAbsMeasure(pst_Meas);
        return;
    }

    for(i = 0; i < pst_Meas->ul_SpectrumLen; i++)
    {
        pst_Meas->plf_BkgSpectrum[i] = pst_Meas->pf_ProcSpectrum[i] * (1 - pst_Meas->f_FilterCoeff) +
                                       pst_Meas->plf_BkgSpectrum[i] * pst_Meas->f_FilterCoeff;  //���±�������
    }

    for(i = pst_Meas->ul_UseLeftDot; i < pst_Meas->ul_UseRightDot; i++)
    {
        pst_Meas->plf_DiffSpectrum[i] = pst_Meas->pf_ZeroSpectrum[i] - pst_Meas->plf_BkgSpectrum[i];    //��ȥ���Ա��� ��ò�ֹ���
    }

    pst_Meas->pst_Gas2->lf_PeakHight += Mod_GasMeasureGetPeakHight(pst_Meas->plf_DiffSpectrum, pst_Meas->pst_Gas2);
    if( ++pst_Meas->ul_Cnt >= pst_Meas->ul_ScanAvg )
    {
        pst_Meas->ul_Cnt = 0;
        pst_Meas->pst_Gas2->lf_PeakHight /= pst_Meas->ul_ScanAvg;

        FP64 f;
        pst_Meas->pst_Gas2->lf_PeakHight = Mod_GasMeasureGetPeakHight(pst_Meas->plf_DiffSpectrum, pst_Meas->pst_Gas2);
        pst_Meas->pst_Gas2->lf_PeakHight = (pst_Meas->pst_Gas2->lf_PeakHight < 0) ? 0:pst_Meas->pst_Gas2->lf_PeakHight;
        f = s_fx(pst_Meas->pst_Gas2->af_NiheCoeff,pst_Meas->pst_Gas2->uch_NiheOrder,(FP32)pst_Meas->pst_Gas2->lf_PeakHight);
        pst_Meas->pst_Gas2 ->f_Correction = pst_Meas->pst_Gas2->lf_Concentration / f;
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas2->f_Correction));
        
        Mod_GasMeasureDoAbsMeasure(pst_Meas);
    }
}

void Mod_GasMeasureCorrectionGasAll(GasMeasure_t* pst_Meas)
{
    INT16U  i;
    if(pst_Meas->pst_Gas1 == NULL)
    {
        Mod_GasMeasureDoAbsMeasure(pst_Meas);
        return;
    }

    for(i = 0; i < pst_Meas->ul_SpectrumLen; i++)
    {
        pst_Meas->plf_BkgSpectrum[i] = pst_Meas->pf_ProcSpectrum[i] * (1 - pst_Meas->f_FilterCoeff) +
                                       pst_Meas->plf_BkgSpectrum[i] * pst_Meas->f_FilterCoeff;  //���±�������
    }

    for(i = pst_Meas->ul_UseLeftDot; i < pst_Meas->ul_UseRightDot; i++)
    {
        pst_Meas->plf_DiffSpectrum[i] = pst_Meas->pf_ZeroSpectrum[i] - pst_Meas->plf_BkgSpectrum[i];    //��ȥ���Ա��� ��ò�ֹ���
    }
    
    pst_Meas->pst_Gas1->lf_PeakHight += Mod_GasMeasureGetPeakHight(pst_Meas->plf_DiffSpectrum, pst_Meas->pst_Gas1);
    pst_Meas->pst_Gas2->lf_PeakHight += Mod_GasMeasureGetPeakHight(pst_Meas->plf_DiffSpectrum, pst_Meas->pst_Gas2);
    if( ++pst_Meas->ul_Cnt >= pst_Meas->ul_ScanAvg )
    {
        pst_Meas->ul_Cnt = 0;
        pst_Meas->pst_Gas1->lf_PeakHight /= pst_Meas->ul_ScanAvg;
        pst_Meas->pst_Gas2->lf_PeakHight /= pst_Meas->ul_ScanAvg;

        FP64 f;
        
        pst_Meas->pst_Gas1->lf_PeakHight = Mod_GasMeasureGetPeakHight(pst_Meas->plf_DiffSpectrum, pst_Meas->pst_Gas1);
        pst_Meas->pst_Gas1->lf_PeakHight = (pst_Meas->pst_Gas1->lf_PeakHight < 0) ? 0:pst_Meas->pst_Gas1->lf_PeakHight;
        f = s_fx(pst_Meas->pst_Gas1->af_NiheCoeff,pst_Meas->pst_Gas1->uch_NiheOrder,(FP32)pst_Meas->pst_Gas1->lf_PeakHight);
        pst_Meas->pst_Gas1->f_Correction = pst_Meas->pst_Gas1->lf_Concentration / f;
        
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas1->f_Correction));

        pst_Meas->pst_Gas2->lf_PeakHight = Mod_GasMeasureGetPeakHight(pst_Meas->plf_DiffSpectrum, pst_Meas->pst_Gas2);
        pst_Meas->pst_Gas2->lf_PeakHight = (pst_Meas->pst_Gas2->lf_PeakHight < 0) ? 0:pst_Meas->pst_Gas2->lf_PeakHight;
        f = s_fx(pst_Meas->pst_Gas2->af_NiheCoeff,pst_Meas->pst_Gas2->uch_NiheOrder,(FP32)pst_Meas->pst_Gas2->lf_PeakHight);
        pst_Meas->pst_Gas2 ->f_Correction = pst_Meas->pst_Gas2->lf_Concentration / f;
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas2->f_Correction));
        
        Mod_GasMeasureDoAbsMeasure(pst_Meas);
    }
}

void Mod_GasMeasureAbsMeasure(GasMeasure_t* pst_Meas)
{
    INT16U i;
  
    if(pst_Meas == NULL)
    {
        return;
    }

    /* ���Բ��� */
    for(i = pst_Meas->ul_UseLeftDot; i < pst_Meas->ul_UseRightDot; i++)
    {
        pst_Meas->plf_DiffSpectrum[i] = pst_Meas->pf_ZeroSpectrum[i] - pst_Meas->pf_ProcSpectrum[i];    //��ȥ���Ա��� ��ò�ֹ���
    }
    
    if(pst_Meas->pst_Gas1 != NULL)
    {
        FP64 f;
        pst_Meas->pst_Gas1->lf_PeakHight = Mod_GasMeasureGetPeakHight(pst_Meas->plf_DiffSpectrum, pst_Meas->pst_Gas1);
        pst_Meas->pst_Gas1->lf_PeakHight = (pst_Meas->pst_Gas1->lf_PeakHight < 0) ? 0:pst_Meas->pst_Gas1->lf_PeakHight;
        f = s_fx(pst_Meas->pst_Gas1->af_NiheCoeff,pst_Meas->pst_Gas1->uch_NiheOrder,(FP32)pst_Meas->pst_Gas1->lf_PeakHight);
        f = (f < 0) ? 0:f;
        pst_Meas->pst_Gas1->lf_Concentration = f * pst_Meas->pst_Gas1->f_Correction;
    }
    if(pst_Meas->pst_Gas2 != NULL)
    {
        FP64 f;
        pst_Meas->pst_Gas2->lf_PeakHight = Mod_GasMeasureGetPeakHight(pst_Meas->plf_DiffSpectrum, pst_Meas->pst_Gas2);
        pst_Meas->pst_Gas2->lf_PeakHight = (pst_Meas->pst_Gas2->lf_PeakHight < 0) ? 0:pst_Meas->pst_Gas2->lf_PeakHight;
        f = s_fx(pst_Meas->pst_Gas2->af_NiheCoeff,pst_Meas->pst_Gas2->uch_NiheOrder,(FP32)pst_Meas->pst_Gas2->lf_PeakHight);
        f = (f < 0) ? 0:f;
        pst_Meas->pst_Gas2->lf_Concentration = f * pst_Meas->pst_Gas2->f_Correction;
    }

    /* ���±������� */
    if(pst_Meas->f_Trans >= pst_Meas->f_TransThreshold)
    {
        for(i = 0; i < pst_Meas->ul_SpectrumLen; i++)
        {
            pst_Meas->plf_BkgSpectrum[i] = pst_Meas->pf_ProcSpectrum[i] * (1 - pst_Meas->f_FilterCoeff) +
                                           pst_Meas->plf_BkgSpectrum[i] * pst_Meas->f_FilterCoeff;  //���±�������
        }
    }
}

void Mod_GasMeasureDiffBackground(GasMeasure_t* pst_Meas)
{
    INT16U  i;
    
    if(pst_Meas == NULL)
        return;

    /* ���±������� */
    if(pst_Meas->f_Trans >= pst_Meas->f_TransThreshold)
    {
        for(i = 0; i < pst_Meas->ul_SpectrumLen; i++)
        {
            pst_Meas->plf_BkgSpectrum[i] = pst_Meas->pf_ProcSpectrum[i] * (1 - pst_Meas->f_FilterCoeff) +
                                           pst_Meas->plf_BkgSpectrum[i] * pst_Meas->f_FilterCoeff;  //���±�������
        }
    }
}

void Mod_GasMeasureDiffMeasure(GasMeasure_t* pst_Meas)
{
    INT16U  i;
    if(pst_Meas == NULL)
    {
        return;
    }

    for(i = pst_Meas->ul_UseLeftDot; i < pst_Meas->ul_UseRightDot; i++)
    {
        pst_Meas->plf_DiffSpectrum[i] = pst_Meas->plf_BkgSpectrum[i] - pst_Meas->pf_ProcSpectrum[i];    //
    }

    if(pst_Meas->pst_Gas1 != NULL)
    {
        FP64 f;
        pst_Meas->pst_Gas1->lf_PeakHight = Mod_GasMeasureGetPeakHight(pst_Meas->plf_DiffSpectrum, pst_Meas->pst_Gas1);
        pst_Meas->pst_Gas1->lf_PeakHight = (pst_Meas->pst_Gas1->lf_PeakHight < 0) ? 0:pst_Meas->pst_Gas1->lf_PeakHight;
        f = s_fx(pst_Meas->pst_Gas1->af_NiheCoeff,pst_Meas->pst_Gas1->uch_NiheOrder,(FP32)pst_Meas->pst_Gas1->lf_PeakHight);
        f = (f < 0) ? 0:f;
        pst_Meas->pst_Gas1->lf_Concentration = f * pst_Meas->pst_Gas1->f_Correction;
        
        Mod_MeasureGasNOReply(pst_Meas->pst_Gas1->lf_Concentration);
    }
    
    if(pst_Meas->pst_Gas2 != NULL)
    {
        FP64 f;
        pst_Meas->pst_Gas2->lf_PeakHight = Mod_GasMeasureGetPeakHight(pst_Meas->plf_DiffSpectrum, pst_Meas->pst_Gas2);
        pst_Meas->pst_Gas2->lf_PeakHight = (pst_Meas->pst_Gas2->lf_PeakHight < 0) ? 0:pst_Meas->pst_Gas2->lf_PeakHight;
        f = s_fx(pst_Meas->pst_Gas2->af_NiheCoeff,pst_Meas->pst_Gas2->uch_NiheOrder,(FP32)pst_Meas->pst_Gas2->lf_PeakHight);
        f = (f < 0) ? 0:f;
        pst_Meas->pst_Gas2->lf_Concentration = f * pst_Meas->pst_Gas2->f_Correction;
        Mod_MeasureGasHCReply(pst_Meas->pst_Gas2->lf_Concentration);
    }

}


//==================================================================================
//| �������� | Mod_GasMeasurePoll
//|----------|----------------------------------------------------------------------
//| �������� | ��ʼ��
//|----------|----------------------------------------------------------------------
//| ������� | ��
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
void Mod_GasMeasurePoll(GasMeasure_t* pst_Meas)
{
    INT32U  i;
    FP32 k;
    FP32 k1;
    void *pv_Msg = PendMeg();        

    if(pv_Msg == NULL)
        return;
    pst_Meas->pst_Dev = pv_Msg;
    USB4000_HandleTypeDef* USB4000_Handle = (USB4000_HandleTypeDef *) pst_Meas->pst_Dev;
    pst_Meas->ul_SpectrumLen = USB4000_Handle->uin_Pixels;

    /* �������׵���ǰ���ײ���������ʱ�� */
    for(i = 0; i < pst_Meas->ul_SpectrumLen; i++)
    {
        pst_Meas->plf_Spectrum[i] = USB4000_Handle->plf_ProcessSpectrum[i];
        pst_Meas->pf_ProcSpectrum[i] = USB4000_Handle->plf_ProcessSpectrum[i]/(USB4000_Handle->ul_IntegralTime/1000);
    }
    
    k = Mod_GasMeasureUpdataTrans(pst_Meas);
    
    /* ����͸���� */
    for(i = 0; i < pst_Meas->ul_SpectrumLen; i++)
    {
        pst_Meas->pf_ProcSpectrum[i] /= k;
    }
    
    switch (pst_Meas->e_State)
    {
    case eGasAdjZero:
        Mod_GasMeasureAdjZero(pst_Meas);
        break;
    case eGasCalibGas1:
        Mod_GasMeasureCalibGas1(pst_Meas);
        break;
    case eGasCalibGas2:
        Mod_GasMeasureCalibGas2(pst_Meas);
        break;
    case eGasCalibGasAll:
        Mod_GasMeasureCalibGasAll(pst_Meas);
        break;
    case eGasCalibCorrectionGas1:
        Mod_GasMeasureCorrectionGas1(pst_Meas);
        break;
    case eGasCalibCorrectionGas2:
        Mod_GasMeasureCorrectionGas2(pst_Meas);
        break; 
    case eGasCalibCorrectionGasAll:
        Mod_GasMeasureCorrectionGasAll(pst_Meas);
        break;
    case eGasAbsMeasure:
        Mod_GasMeasureAbsMeasure(pst_Meas);
        break;
    case eGasDiffBackground:
        Mod_GasMeasureDiffBackground(pst_Meas);
        break;
    case eGasDiffMeasure:
        Mod_GasMeasureDiffMeasure(pst_Meas);
        break;
    default:
        break;
    }
}

BOOL Mod_GasMeasureDoAdjZero(GasMeasure_t* pst_Meas,INT16U uin_Cont)
{
    if(pst_Meas == NULL)
        return FALSE;
    pst_Meas->e_State = eGasAdjZero;
    pst_Meas->ul_Cnt = 0;
    pst_Meas->ul_ScanAvg = uin_Cont;
    return TRUE;
}

BOOL Mod_GasMeasureDoCalib(GasMeasure_t* pst_Meas,GasMeasureState_e e_State,INT16U uin_Cont,FP32 lf_GasCon1,FP32 lf_GasCon2)
{
    if(pst_Meas == NULL)
        return FALSE;
    
    pst_Meas->ul_Cnt = 0;
    pst_Meas->ul_ScanAvg = uin_Cont;
    
    switch (e_State)
    {
    case eGasCalibGas1:
        if( pst_Meas->pst_Gas1 == NULL )
            return FALSE;
        pst_Meas->pst_Gas1->lf_Concentration = lf_GasCon1;
        pst_Meas->pst_Gas1->lf_PeakHight = 0.0;
        pst_Meas->e_State = eGasCalibGas1; 
        return TRUE;
    case eGasCalibGas2:
        if( pst_Meas->pst_Gas2 == NULL )
            return FALSE;
        pst_Meas->pst_Gas2->lf_Concentration = lf_GasCon2;
        pst_Meas->pst_Gas2->lf_PeakHight = 0.0;
        pst_Meas->e_State = eGasCalibGas2; 
        return TRUE;
    case eGasCalibGasAll:
        if( pst_Meas->pst_Gas1 == NULL || pst_Meas->pst_Gas2 == NULL )
            return FALSE;
        pst_Meas->pst_Gas1->lf_Concentration = lf_GasCon1;
        pst_Meas->pst_Gas1->lf_PeakHight = 0.0;
        pst_Meas->pst_Gas2->lf_Concentration = lf_GasCon2;
        pst_Meas->pst_Gas2->lf_PeakHight = 0.0;
        pst_Meas->e_State = eGasCalibGasAll; 
        return TRUE;
    default:
        return FALSE;
    }
}

BOOL Mod_GasMeasureDoCalibCorrection(GasMeasure_t* pst_Meas,GasMeasureState_e e_State,INT16U uin_Cont,FP32 lf_GasCon1,FP32 lf_GasCon2)
{
    if(pst_Meas == NULL)
        return FALSE;
    
    pst_Meas->ul_Cnt = 0;
    pst_Meas->ul_ScanAvg = uin_Cont;
    
    switch (e_State)
    {
    case eGasCalibCorrectionGas1:
        if( pst_Meas->pst_Gas1 == NULL )
            return FALSE;
        pst_Meas->pst_Gas1->lf_Concentration = lf_GasCon1;
        pst_Meas->pst_Gas1->lf_PeakHight = 0.0;

        pst_Meas->e_State = eGasCalibCorrectionGas1; 
        return TRUE;
    case eGasCalibCorrectionGas2:
        if( pst_Meas->pst_Gas2 == NULL )
            return FALSE;
        pst_Meas->pst_Gas2->lf_Concentration = lf_GasCon2;
        pst_Meas->pst_Gas2->lf_PeakHight = 0.0;
        pst_Meas->e_State = eGasCalibCorrectionGas2; 
        return TRUE;
    case eGasCalibCorrectionGasAll:
        if( pst_Meas->pst_Gas1 == NULL || pst_Meas->pst_Gas2 == NULL )
            return FALSE;
        pst_Meas->pst_Gas1->lf_Concentration = lf_GasCon1;
        pst_Meas->pst_Gas1->lf_PeakHight = 0.0;
        pst_Meas->pst_Gas2->lf_Concentration = lf_GasCon2;
        pst_Meas->pst_Gas2->lf_PeakHight = 0.0;
        pst_Meas->e_State = eGasCalibCorrectionGasAll; 
        return TRUE;
    default:
        return FALSE;
    }
}

BOOL Mod_GasMeasureDoDiffBackground(GasMeasure_t* pst_Meas)
{
    if(pst_Meas == NULL)
        return FALSE;
    pst_Meas->e_State = eGasDiffBackground;
    return TRUE;
}

BOOL Mod_GasMeasureDoDiffMeasure(GasMeasure_t* pst_Meas)
{
    if(pst_Meas == NULL)
        return FALSE;
    pst_Meas->e_State = eGasDiffMeasure;
    return TRUE;
}

BOOL Mod_GasMeasureDoAbsMeasure(GasMeasure_t* pst_Meas)
{
    if(pst_Meas == NULL)
        return FALSE;
    pst_Meas->e_State = eGasAbsMeasure;
    return TRUE;
}
/*
BOOL Mod_GasMeasureDoWait(GasMeasure_t* pst_Meas)
{
    if(pst_Meas == NULL)
        return FALSE;
    pst_Meas->e_State = eGasWait;
    return TRUE;
}*/

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
        st_GasMeasure.pst_Gas1->af_NiheCoeff[6] = 0;
        st_GasMeasure.pst_Gas1->af_NiheCoeff[7] = 0;
        st_GasMeasure.pst_Gas1->af_NiheCoeff[8] = 0;  
        st_GasMeasure.pst_Gas1->af_NiheCoeff[9] = 0;  
        Mod_CalibPointListNihe(pst_Meas->pst_Gas1->pst_CalibPointList,
                               pst_Meas->pst_Gas1->uch_NiheOrder,
                               pst_Meas->pst_Gas1->af_NiheCoeff);

        st_GasMeasure.pst_Gas1->f_Correction = 1.0;
        
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas1->af_NiheCoeff[0]));
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas1->af_NiheCoeff[1]));
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas1->af_NiheCoeff[2]));
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas1->af_NiheCoeff[3]));
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas1->af_NiheCoeff[4]));
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas1->af_NiheCoeff[5])); 
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas1->af_NiheCoeff[6]));
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas1->af_NiheCoeff[7]));
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas1->af_NiheCoeff[8]));
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas1->af_NiheCoeff[9])); 
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas1->f_Correction));

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
        st_GasMeasure.pst_Gas2->af_NiheCoeff[6] = 0;
        st_GasMeasure.pst_Gas2->af_NiheCoeff[7] = 0;
        st_GasMeasure.pst_Gas2->af_NiheCoeff[8] = 0;  
        st_GasMeasure.pst_Gas2->af_NiheCoeff[9] = 0;  
        Mod_CalibPointListNihe(pst_Meas->pst_Gas2->pst_CalibPointList,
                               pst_Meas->pst_Gas2->uch_NiheOrder,
                               pst_Meas->pst_Gas2->af_NiheCoeff);
        
        st_GasMeasure.pst_Gas2->f_Correction = 1.0;
        
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas2->af_NiheCoeff[0]));
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas2->af_NiheCoeff[1]));
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas2->af_NiheCoeff[2]));
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas2->af_NiheCoeff[3]));
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas2->af_NiheCoeff[4]));
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas2->af_NiheCoeff[5])); 
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas2->af_NiheCoeff[6]));
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas2->af_NiheCoeff[7]));
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas2->af_NiheCoeff[8]));
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas2->af_NiheCoeff[9])); 
        SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas2->f_Correction));

        break; 
/*
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
*/
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
