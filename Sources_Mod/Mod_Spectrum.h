#ifndef __MOD_SPECTRUM_H__
#define __MOD_SPECTRUM_H__

#define     DEF_CALIB_NIHE_ORDER_MAX    9

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
    eGasWait,               /* �ȴ����� */
    eGasCalibTrans,         /* �궨͸���� */   
    eGasCalibCorrectionGas1,/* �ֳ����� */
    eGasCalibCorrectionGas2,/* �ֳ����� */
    eGasCalibCorrectionGasAll,/* �ֳ����� */
}GasMeasureState_e;

typedef struct {
    INT32U  ul_PeakCenterDot;                   /* �������ĵ� */
    INT32U  ul_PeakLeftDot;                     /* ������߽�� */
    INT32U  ul_PeakRightDot;                    /* �����ұ߽�� */
    INT32U  ul_LeftBackgroundLeftDot;           /* �󱳾���߽�� */
    INT32U  ul_LeftBackgroundRightDot;          /* �󱳾��ұ߽�� */
    INT32U  ul_RightBackgroundLeftDot;          /* �ұ�����߽�� */
    INT32U  ul_RightBackgroundRightDot;         /* �ұ����ұ߽�� */
}Peak_t;

typedef struct {
    INT8U*      puch_Name;                      /* �������� */
    GasType_e   e_GasType;                      /* ������ */

    Peak_t      st_PeakRef;                     /* ���������շ�λ�� */
    Peak_t      st_PeakMeasure;                 /* ʵ������շ���Ϣ */

    CalibPointList_t* pst_CalibPointList;       /* �궨���б� */

    INT8U       uch_NiheOrder;                  /* ��Ͻ��� */
    FP32        af_NiheCoeff[DEF_CALIB_NIHE_ORDER_MAX+1];                /* ������� */
    FP32        f_Correction;                   /* �������� */

    FP64        lf_PeakHight;                   /* ���շ�߶� */
    FP64        lf_Concentration;               /* Ũ�� */
}GasInfo_t;

typedef struct {
    GasMeasureState_e e_State;                  /* ����״̬ */

    void*        pst_Dev;                       /* �������豸 */

    BOOL         b_SaveZeroSpecetrum;            /* �洢������� */
    
    FP32*        pf_WaveLenth;                  /* �������� */
    FP32*        plf_Spectrum;                  /* ���ڵĹ��� */
    FP32*        pf_ProcSpectrum;               /* ������� */
    FP32*        pf_ZeroSpectrum;               /* ���Թ��� ������� */
    FP32*        plf_BkgSpectrum;               /* �������� ��ֲ��� */
    FP32*        plf_DiffSpectrum;              /* ��ֹ��� */


    INT32U       ul_SpectrumLen;                /* ���׳��� */
    INT32U       ul_UseLeftDot;                 /* ʹ�õĹ��׷�Χ��߽� */
    INT32U       ul_UseRightDot;                /* ʹ�õĹ��׷�Χ�ұ߽� */

    FP32         f_Trans;                       /* ͸���� */
    FP32         f_TransK;                      /* ͸����ϵ�� */
    FP32         f_TransThreshold;              /* ͸��������ֵ */    
    INT32U       ul_TransLeftDot;               /* ͸������ߵ� */
    INT32U       ul_TransRightDot;              /* ͸�����ұߵ� */

    FP32         f_FilterCoeff;                 /* ����һ���˲�ϵ�� */
    INT32U       ul_Cnt;                        /* ����ֵ */
    INT32U       ul_ScanAvg;                    /* ƽ������ֵ */
    
    GasInfo_t*   pst_Gas1;                      /* ����1 */
    GasInfo_t*   pst_Gas2;                      /* ����2 */
}GasMeasure_t;

extern FP32 af_ZeroSpectrum[3648];
extern GasInfo_t st_GasNO;
extern GasInfo_t st_GasHC;
extern GasMeasure_t st_GasMeasure;

BOOL Mod_GasMeasureDoAdjZero(GasMeasure_t* pst_Meas,INT16U uin_Cont);

BOOL Mod_GasMeasureDoCalib(GasMeasure_t* pst_Meas,GasMeasureState_e e_State,INT16U uin_Cont,FP32 lf_GasCon1,FP32 lf_GasCon2);

BOOL Mod_GasMeasureDoCalibCorrection(GasMeasure_t* pst_Meas,GasMeasureState_e e_State,INT16U uin_Cont,FP32 lf_GasCon1,FP32 lf_GasCon2);

BOOL Mod_GasMeasureDoDiffMeasure(GasMeasure_t* pst_Meas);

BOOL Mod_GasMeasureDoAbsMeasure(GasMeasure_t* pst_Meas);

BOOL Mod_GasMeasureDoCalibTrans(GasMeasure_t* pst_Meas);

BOOL Mod_GasMeasureDoWait(GasMeasure_t* pst_Meas);

void Mod_GasMeasureInit(GasMeasure_t* pst_Meas);

void Mod_GasMeasurePoll(GasMeasure_t* pst_Meas);

BOOL Mod_GasMarkWorkLine(GasMeasure_t* pst_Meas,GasMeasureState_e e_Ops);

__weak void Mod_MeasureGasHCReply(FP64 lf_Concentration);

__weak void Mod_MeasureGasNOReply(FP64 lf_Concentration);


#endif