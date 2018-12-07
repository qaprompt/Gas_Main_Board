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

#define     DEF_CALIB_NIHE_ORDER_MAX    3

typedef enum {
    eGasN2 = 0,
    eGasCO2,
    eGasCO,
    eGasNO,
    eGasHC,
}GasType_e;

typedef enum {
    eGasAdjZero = 0,        /* ���� */
    eGasCalibGas1,          /* �궨 ����1 */
    eGasCalibGas2,          /* �궨 ����2 */
    eGasCalibAll,           /* �궨 �������� */
    eGasAbsMeasure,         /* ����Ũ�Ȳ��� */
    eGasDiffMeasure,        /* ���Ũ�Ȳ��� */
}GasMeasureState_e;

typedef {
    INT32U  ul_PeakCenterDot;                   /* �������ĵ� */
    INT32U  ul_PeakLeftDot;                     /* ������߽�� */
    INT32U  ul_PeakRightDot;                    /* �����ұ߽�� */
    INT32U  ul_LeftBackgroundLeftDot;           /* �󱳾���߽�� */
    INT32U  ul_LeftBackgroundRightDot;          /* �󱳾��ұ߽�� */
    INT32U  ul_RightBackgroundLeftDot;          /* �ұ�����߽�� */
    INT32U  ul_RightBackgroundRightDot;         /* �ұ����ұ߽�� */
}Peak_t;

typedef {
    INT8U*      puch_Name;                      /* �������� */
    GasType_e   e_GasType;                      /* ������ */

    Peak_t      st_PeakRef;                     /* ���������շ�λ�� */
    Peak_t      st_PeakMeasure;                 /* ʵ������շ���Ϣ */

    CalibPointList_t* pst_CalibPointList;       /* �궨���б� */

    INT8U       uch_NiheOrder;                  /* ��Ͻ��� */
    FP32        af_NiheCoeff[DEF_CALIB_NIHE_ORDER_MAX];                /* ������� */

    FP64        lf_PeakHight;                   /* ���շ�߶� */
    FP64        lf_Concentration;               /* Ũ�� */
}GasInfo_t;

typedef {
    GasMeasureState_e e_State;                  /* ����״̬ */

    void*        pst_Dev;                       /* �������豸 */

    FP32*        pf_WaveLenth;                  /* �������� */
    FP64*        plf_AbsSpectrum;               /* ���Թ��� ������� */
    FP64*        plf_BkgSpectrum;               /* �������� ��ֲ��� */
    FP64*        plf_DiffSpectrum��             /* ��ֹ��� */
    FP64*        plf_NowSpectrum;               /* ���ڵĹ��� */

    INT32U       ul_SpectrumLen;                /* ���׳��� */
    INT32U       ul_UseLeftDot;                 /* ʹ�õĹ��׷�Χ��߽� */
    INT32U       ul_UseRightDot;                /* ʹ�õĹ��׷�Χ�ұ߽� */

    FP32         f_Trans;                       /* ͸���� */
    INT32U       ul_TransLeftDot;               /* ͸������ߵ� */
    INT32U       ul_TransRightDot;              /* ͸�����ұߵ� */

    FP32         f_FilterCoeff;                 /* ����һ���˲�ϵ�� */
    INT32U       ul_Cnt;                        /* ����ֵ */
    INT32U       ul_AdjZeroCnt;                 /* �������ֵ */
    INT32U       ul_CalibCnt;                   /* У׼����ֵ */

    GasInfo_t*   pst_Gas1;                      /* ����1 */
    GasInfo_t*   pst_Gas2;                      /* ����2 */
}GasMeasure_t;

static  FP64 alf_AbsSpectrum[3840] = {0.0};
static  FP64 alf_BkgSpectrum[3840] = {0.0};
static  FP64 alf_DiffSpectrum[3840] = {0.0};
static  FP64 alf_Spectrum[3840] = {0.0};

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
    NULL,                           /* �������豸 */

    NULL,                           /* �������� */
    &alf_AbsSpectrum,               /* ���Թ��� ������� */
    &alf_BkgSpectrum,               /* �������� ��ֲ��� */
    &alf_DiffSpectrum,              /* ��ֹ��� */
    &alf_Spectrum,                  /* ���ڵĹ��� */

    100,                            /* ʹ�õĹ��׷�Χ��߽� */
    1500,                           /* ʹ�õĹ��׷�Χ�ұ߽� */

    0.0,                            /* ͸���� */
    1000,                           /* ͸������ߵ� */
    1300,                           /* ͸�����ұߵ� */

    0.5,                            /* ����һ���˲�ϵ�� */

    &st_GasN0,                      /* ����1 */
    NULL,                           /* ����2 */
}

static void InitSem(void)
{
    OS_ERR os_err;
    OSTaskSemSet(&TaskGasMeasTCB,0U,&os_err);
}

static void PostSem(void)
{
    OS_ERR os_err;
    OSTaskSemPost(&TaskGasMeasTCB,OS_OPT_POST_NONE,&os_err);
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
    OSTaskQPost(&TaskGasMeasTCB,(void*)pv_Msg,1,OS_OPT_POST_FIFO ,&os_err);
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
    st_GasN0.st_RefPeak.ul_PeakCenterDot = (226.049);
    st_GasN0.st_RefPeak.ul_PeakLeftDot = st_GasN0.st_RefPeak.ul_PeakCenterDot - 20;
    st_GasN0.st_RefPeak.ul_PeakRightDot = st_GasN0.st_RefPeak.ul_PeakCenterDot + 20;

    st_GasN0.st_RefPeak.ul_LeftBackgroundRightDot = st_GasN0.st_RefPeak.ul_PeakLeftDot - 1;
    st_GasN0.st_RefPeak.ul_LeftBackgroundLeftDot = st_GasN0.st_RefPeak.ul_LeftBackgroundRightDot - 40;

    st_GasN0.st_RefPeak.ul_RightBackgroundLeftDot = st_GasN0.st_RefPeak.ul_PeakRightDot + 1;
    st_GasN0.st_RefPeak.ul_LeftBackgroundLeftDot = st_GasN0.st_RefPeak.ul_RightBackgroundLeftDot + 40;

    if(pst_Meas->pst_Gas1 != NULL)
        Mod_CalibPointListInit(pst_Meas->pst_Gas1->pst_CalibPointList);
    if(pst_Meas->pst_Gas2 != NULL)
        Mod_CalibPointListInit(pst_Meas->pst_Gas2->pst_CalibPointList);
}

BOOL Mod_GasMeasureGotoAbsMeasure(GasMeasure_t* pst_Meas)
{
    if(pst_Meas == NULL)
        return FALSE;
    pst_Meas->e_State = eGasAbsMeasure;
}

FP32 Mod_GasMeasureCalBkgSpectrumTrans(GasMeasure_t* pst_Meas)
{
    FP64  lf_Sum1 = 0;
    FP64  lf_Sum2 = 0;
    FP32  f_Trans = 0;
    INT32U  i;
    for(i = pst_Meas->ul_TransLeftDot; i< pst_Meas->ul_TransRightDot; i++)
    {
        ul_Sum1 += pst_Meas->plf_AbsSpectrum[i];
        ul_Sum2 += pst_Meas->plf_BkgSpectrum[i];
    }

    f_Trans = pst_Meas->ul_Sum2 / pst_Meas->ul_Sum1;
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
        ul_Sum1 += pst_Meas->plf_AbsSpectrum[i];
        ul_Sum2 += pst_Meas->alf_Spectrum[i];
    }

    f_Trans = pst_Meas->ul_Sum2 / pst_Meas->ul_Sum1;
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
        ul_Sum1 += pst_Meas->plf_AbsSpectrum[i];
        ul_Sum2 += pst_Meas->plf_Spectrum[i];
    }

    f_Trans = pst_Meas->ul_Sum2 / pst_Meas->ul_Sum1;
    f_Trans = (f_Trans>1)? 1:f_Trans;
    return f_Trans;
}


FP64 Mod_GasMeasureGetPeakHight(FP64* plf_Spectrum, GasInfo_t* pst_Gas)
{
    FP64    lf_Peak = 0;
    FP64    lf_BkgL = 0;
    FP64    lf_BkgR =0;
    INT32   i,j;
    Peak_t* pst_Peak = &pst_Gas->st_PeakMeasure;

#if 0
    /* ֱ�Ӹ��� �������������շ�߶� */
    memcpy(&pst_Gas->st_PeakMeasure, &pst_Gas->st_Peakref, sizeof(Peak_t));
#else
    /* �������ķ�Χ�ڵ���ߵ����շ�*/
    for(i = pst_Peak->ul_PeakLeftDot; i <=pst_Peak->ul_PeakRightDot; i++)
    {
        if(plf_Spectrum[i] > lf_Peak)   //lf_Peak = lf_Max
        {
            lf_Peak = plf_Spectrum[i];
            j = i;
        }
    }
    i -= pst_Peak->ul_PeakCenterDot;

    pst_Peak->ul_PeakCenterDot  += i;
    pst_Peak->ul_PeakLeftDot    += i;
    pst_Peak->ul_PeakRightDot   += i;
    pst_Peak->ul_LeftBackgroundLeftDot  += i;
    pst_Peak->ul_LeftBackgroundRightDot += i;
    pst_Peak->ul_RightBackgroundLeftDot  += i;
    pst_Peak->ul_RightBackgroundRightDot += i;
#endif

    for(i = pst_Peak->ul_PeakLeftDot; i <=pst_Peak->ul_PeakRightDot; i++)    //�ۼƷ�߶�
        lf_Peak += plf_Spectrum[i];
    lf_Peak /= (pst_Peak->ul_PeakRightDot - pst_Peak->ul_PeakLeftDot + 1);   //���ƽ��

    for(i = pst_Peak->ul_LeftBackgroundLeftDot; i <= pst_Peak->ul_LeftBackgroundRightDot; i++)      //�ۼ��󱳾�
        lf_BkgL += plf_Spectrum[i];
    lf_BkgL /= (pst_Peak->ul_LeftBackgroundRightDot - pst_Peak->ul_LeftBackgroundLeftDot + 1);     //���ƽ��

    for(i = pst_Peak->ul_RightBackgroundLeftDot; i <= pst_Peak->ul_RightBackgroundRightDot; i++)    //�ۼ��ұ���
        lf_BkgR += plf_Spectrum[i];
    lf_BkgR /= (pst_Peak->ul_RightBackgroundRightDot - pst_Peak->ul_RightBackgroundLeftDot + 1);   //���ƽ��

    return = lf_Peak - ((lf_BkgL+lf_BkgR)/2);
}

void Mod_GasMeasurePoll(GasMeasure_t* pst_Meas)
{
    INT32U  i;
    void *pv_Msg = PendMeg();
    if(pv_Msg == NULL)
        return;

    USB4000_HandleTypeDef* USB4000_Handle =  (USB4000_HandleTypeDef *) pst_Meas->pst_Dev;
    pst_Meas->ul_SpectrumLen = USB4000_Handle->uin_Pixels;

    /* �������׵���ǰ���� */
    for(i = 0; i < pst_Meas->ul_SpectrumLen; i++)
    {
        pst_Meas->plf_Spectrum[i] = USB4000_Handle->plf_ProcessSpectrum[i];
    }

    switch (pst_Meas->e_State)
    {
    case eGasAdjZero:
        /* ���� һ���˲����±������� �����������״�����Թ��ײ�д��E2PROM */
        pst_Meas->f_Trans = 100.0f;

        for(i = 0; i < pst_Meas->ul_SpectrumLen; i++)
        {
            pst_Meas->plf_BkgSpectrum[i] = pst_Meas->plf_Spectrum[i] * (1 - pst_Meas->f_FilterCoeff) +
                                           pst_Meas->plf_BkgSpectrum[i] * pst_Meas->f_FilterCoeff;
        }

        if( ++pst_Meas->ul_Cnt >= pst_Meas->ul_AdjZeroCnt )
        {
            pst_Meas->ul_Cnt = 0;

            for(i = 0; i < pst_Meas->ul_SpectrumLen;; i++)
            {
                pst_Meas->plf_AbsSpectrum[i] = pst_Meas->plf_BkgSpectrum[i];
            }

            /* �洢���ݵ�EEPROM */
            CalibPoint_t st_CalibPoint = {TRUE,0.0,0.0};
            Mod_CalibPointListEditOnePoint((pst_Meas->pst_Gas1->pst_CalibPointList,0,&st_CalibPoint);

            SaveToEepromExt((INT32U)pst_Meas->plf_AbsSpectrum,pst_Meas->ul_SpectrumLen);        //�洢������

            Mod_GasMeasureGotoAbsMeasure(&pst_Meas);
        }
        break;

    case eGasCalibGas1:
    case eGasCalibGas2:
    case eGasCalibALL:
        /* �궨 ��¼�궨��Ũ�� */
        FP32 k;
        for(i = 0; i < pst_Meas->ul_SpectrumLen; i++)
        {
            pst_Meas->plf_BkgSpectrum[i] = pst_Meas->plf_Spectrum[i] * (1 - pst_Meas->f_FilterCoeff) +
                                           pst_Meas->plf_BkgSpectrum[i] * pst_Meas->f_FilterCoeff;  //���±�������
        }

        k = Mod_GasMeasureCalBkgSpectrumTrans();    //����͸����
        pst_Meas->f_Trans = k * 100.0f;             //����͸�������˲����׼���͸����

        for(i = ul_UseLeftDot; i < pst_Meas->ul_UseRightDot; i++)
        {
            pst_Meas->plf_DiffSpectrum[i] = pst_Meas->plf_AbsSpectrum[i] - pst_Meas->plf_BkgSpectrum[i] / k;    //��ȥ���Ա��� ��ò�ֹ���
        }

        switch (pst_Meas->e_State)
        {
            case eGasCalibGas1:
                if(pst_Meas->pst_Gas1 == NULL)
                {
                    Mod_GasMeasureGotoAbsMeasure(&pst_Meas);
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
                    Mod_CalibPointListEditOnePoint((pst_Meas->pst_Gas1->pst_CalibPointList,1,&st_CalibPoint);
                    Mod_GasMeasureGotoAbsMeasure(&pst_Meas);
                }
                break;
            case eGasCalibGas2:
                if(pst_Meas->pst_Gas2 == NULL)
                {
                    Mod_GasMeasureGotoAbsMeasure(&pst_Meas);
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
                    Mod_CalibPointListEditOnePoint((pst_Meas->pst_Gas2->pst_CalibPointList,1,&st_CalibPoint);
                    Mod_GasMeasureGotoAbsMeasure(&pst_Meas);
                }
                break;
            case eGasCalibALL:
                if(pst_Meas->pst_Gas1 == NULL || pst_Meas->pst_Gas2 == NULL)
                {
                    Mod_GasMeasureGotoAbsMeasure(&pst_Meas);
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
                    Mod_CalibPointListEditOnePoint((pst_Meas->pst_Gas1->pst_CalibPointList,1,&st_CalibPoint);

                    pst_Meas->pst_Gas2->lf_PeakHight /= pst_Meas->ul_CalibCnt;
                    st_CalibPoint.b_Use = TRUE;
                    st_CalibPoint.f_X   = pst_Meas->pst_Gas2->lf_PeakHight;
                    st_CalibPoint.f_Y   = pst_Meas->pst_Gas2->lf_Concentration;
                    /* �洢���ݵ�EEPROM */
                    Mod_CalibPointListEditOnePoint((pst_Meas->pst_Gas2->pst_CalibPointList,1,&st_CalibPoint);

                    Mod_GasMeasureGotoAbsMeasure(&pst_Meas);
                }
                break;
            default:
                break;
        }
        break;
    case eGasAbsMeasure:
        /* ���Բ��� */
        FP32 k;
        k = Mod_GasMeasureCalSpectrumTrans();       //����͸����
        pst_Meas->f_Trans = k * 100.0f;             //����͸�����õ�ǰ���׼���͸����

        for(i = ul_UseLeftDot; i < pst_Meas->ul_UseRightDot; i++)
        {
            pst_Meas->plf_DiffSpectrum[i] = pst_Meas->plf_AbsSpectrum[i] - pst_Meas->plf_Spectrum[i] / k;    //��ȥ���Ա��� ��ò�ֹ���
        }

        if(pst_Meas->pst_Gas1 != NULL)
        {
            pst_Meas->pst_Gas1->lf_PeakHight = Mod_GasMeasureGetPeakHight(pst_Meas->plf_DiffSpectrum, pst_Meas->pst_Gas1);
            pst_Meas->pst_Gas1->lf_Concentration = s_fx(pst_Meas->pst_Gas1->af_NiheCoeff,pst_Proc->uc_WorkLineOrder,
                                                       (FP32)pst_Meas->pst_Gas1->lf_PeakHight);
        }
        if(pst_Meas->pst_Gas2 != NULL)
        {
            pst_Meas->pst_Gas2->lf_PeakHight = Mod_GasMeasureGetPeakHight(pst_Meas->plf_DiffSpectrum, pst_Meas->pst_Gas2);
            pst_Meas->pst_Gas2->lf_Concentration = s_fx(pst_Meas->pst_Gas2->af_NiheCoeff,pst_Proc->uc_WorkLineOrder,
                                                       (FP32)pst_Meas->pst_Gas2->lf_PeakHight);
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
        k = Mod_GasMeasureCalSpectrumTrans();       //����͸����
        pst_Meas->f_Trans = k * 100.0f;             //����͸�����õ�ǰ���׼���͸����

        for(i = ul_UseLeftDot; i < pst_Meas->ul_UseRightDot; i++)
        {
            pst_Meas->plf_DiffSpectrum[i] = pst_Meas->plf_BkgSpectrum[i] - pst_Meas->plf_Spectrum[i] / k;    //��ȥ���Ա��� ��ò�ֹ���
        }

        if(pst_Meas->pst_Gas1 != NULL)
        {
            pst_Meas->pst_Gas1->lf_PeakHight = Mod_GasMeasureGetPeakHight(pst_Meas->plf_DiffSpectrum, pst_Meas->pst_Gas1);
            pst_Meas->pst_Gas1->lf_Concentration = s_fx(pst_Meas->pst_Gas1->af_NiheCoeff,pst_Proc->uc_WorkLineOrder,
                                                       (FP32)pst_Meas->pst_Gas1->lf_PeakHight);
        }
        if(pst_Meas->pst_Gas2 != NULL)
        {
            pst_Meas->pst_Gas2->lf_PeakHight = Mod_GasMeasureGetPeakHight(pst_Meas->plf_DiffSpectrum, pst_Meas->pst_Gas2);
            pst_Meas->pst_Gas2->lf_Concentration = s_fx(pst_Meas->pst_Gas2->af_NiheCoeff,pst_Proc->uc_WorkLineOrder,
                                                       (FP32)pst_Meas->pst_Gas2->lf_PeakHight);
        }

        break;
    default:
        break;
    }

}
