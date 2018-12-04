#ifndef __MOD_MEASURE_H__
#define __MOD_MEASURE_H__

typedef struct {
    
    INT8U   auch_CarNum[16];        /* ���� */
    FP32    f_Speed;                /* �ٶ� */
    FP32    f_Acc;                  /* ���ٶ� */
    FP64    lf_CO2;                 /* CO2Ũ�� */
    FP64    lf_CO;                  /* CO1Ũ�� */
    FP64    lf_NO;                  /* NOŨ�� */
    FP64    lf_HC;                  /* HCŨ�� */
    FP64    lf_Grey;                /* �̶� */
}Measure_t ;

extern  Measure_t st_Measure;

void Mod_MeasureInit(Measure_t* pst_Meas);
void Mod_MeasurePoll(Measure_t* pst_Meas);

#endif
