#ifndef __MOD_MEASURE_H__
#define __MOD_MEASURE_H__


#define DEF_SAMPLE_DOT_MAX     600

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
    FP64    lf_CO2;                 /* CO2Ũ�� */
    FP64    lf_CO;                  /* CO1Ũ�� */
    FP64    lf_NO;                  /* NOŨ�� */
    FP64    lf_HC;                  /* HCŨ�� */
    FP64    lf_Grey;                /* �̶� */
}MeasureResult_t;


typedef struct {

    INT32U  ul_DeadTime;            /* ����ʱ�� */
    INT32U  ul_MesureTime;          /* ����ʱ�� */
    
    SampleDots_t st_SampleHC;       /* HC������ */
    SampleDots_t st_SampleNO;       /* NO������ */
    SampleDots_t st_SampleCO;       /* CO������ */
    SampleDots_t st_SampleCO2;      /* CO2������ */
    
    SampleDots_t st_SampleGrey[10]; /* �̶Ȳ����� */
    
    MeasureResult_t st_Result;      /* ���Խ�� */
    
}Measure_t ;

extern  Measure_t st_Measure;

void Mod_MeasureInit(Measure_t* pst_Meas);
void Mod_MeasurePoll(Measure_t* pst_Meas);

#endif
