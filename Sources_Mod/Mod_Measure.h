#ifndef __MOD_MEASURE_H__
#define __MOD_MEASURE_H__


#define DEF_SAMPLE_DOT_MAX     50

typedef struct {
    FP32    af_Buff[DEF_SAMPLE_DOT_MAX];
    INT32U  ul_Len;
    INT32U  ul_Size;
}SampleDots_t;

typedef struct {
    INT32U  ul_ID;                  /* ������� */
    INT8U   auch_CarNum[10];        /* ���� */
    FP32    f_Speed;                /* �ٶ� */
    FP32    f_Acc;                  /* ���ٶ� */
    FP64    lf_Vsp;                 /* �ȹ���  */
    FP64    lf_CO2;                 /* CO2Ũ�� */
    FP64    lf_CO;                  /* CO1Ũ�� */
    FP64    lf_NO;                  /* NOŨ�� */
    FP64    lf_HC;                  /* HCŨ�� */
    FP64    lf_Grey;                /* �̶� */
}MeasureResult_t;


typedef enum {
    e_MeasureIdle = 0,
    e_MeasureWait,
    e_MeasureDead,
    e_MeasureSample,
    e_MeasureCal,
    e_MeasureTimeOut,
    
    e_MeasureStaticSample = 0x80,
    e_MeasureStaticCal,
    
}MeasureState_e;


typedef enum {
    e_MeasIdle = 0,                 /* ����*/
    e_MeasStaticCalib,              /* ��̬�궨 */
    e_MeasStaticMeasure,            /* ��̬���� */
    e_MeasDynamicMeasure,           /* ȼ�շ��̲��� */    
}MeaureMode_e;

typedef struct {
    MeasureState_e  e_State;        /* ����״̬ */
    MeaureMode_e    e_Mode;         /* ����ģʽ */
    INT32U  ul_DeadTime;            /* ����ʱ�� */
    INT32U  ul_MesureTime;          /* ����ʱ�� */
    INT16U	uin_InvalidDots;		/* ��Ч�� */
    INT16U	uin_ActiveDots;			/* ��Ч�� */
    
    SampleDots_t st_SampleCO2;      /* CO2������ */    
    SampleDots_t st_SampleCO;       /* CO������ */
    SampleDots_t st_SampleNO;       /* NO������ */    
    SampleDots_t st_SampleHC;       /* HC������ */
    SampleDots_t st_SampleGrey[10]; /* �̶Ȳ����� */
    
    INT16U  uin_CalibCnt;           /* �궨���� */
    
    CalibPoint_t st_CalibCO2;       /* CO2У׼�� */
    CalibPoint_t st_CalibCO;        /* COУ׼�� */
    CalibPoint_t st_CalibNO;        /* NOУ׼�� */
    CalibPoint_t st_CalibHC;        /* HCУ׼�� */
    
    FP64    lf_CO2;                 /* ƽ��CO2 */
    FP64    lf_CO;                  /* ƽ��CO */
    FP64    lf_NO;                  /* ƽ��NO */
    FP64    lf_HC;                  /* ƽ��HC */
    FP64    lf_Grey;                /* ƽ���̶�ֵ */
    
    MeasureResult_t st_Result;      /* ���Խ�� */
    
}Measure_t ;

extern  Measure_t st_Measure;

void Mod_MeasureInit(Measure_t* pst_Meas);
void Mod_MeasurePoll(Measure_t* pst_Meas);

void Mod_MeasureDoStaticMeas(Measure_t* pst_Meas);
void Mod_MeasureDoStaticCalib(Measure_t* pst_Meas,FP32 f_CO2,FP32 f_CO,FP32 f_NO,FP32 f_HC);
void Mod_MeasureDoDynamicMeas(Measure_t* pst_Meas);

#endif
