
#ifndef __MOD_LASER_H__
#define __MOD_LASER_H__


#include "Mod_Include.h"

typedef struct {
    INT32U   ul_Count;               /* ���Լ��� */
    FP32    f_Trans;                /* ͸���� */
    FP64    lf_ConcentrationCO2;    /* CO2Ũ�� */
    FP64    lf_ConcentrationCO;     /* COŨ�� */

    StdbusDev_t* pst_Handle;
}LaserBoard_t;


extern LaserBoard_t st_Laser;

void Mod_LaserPoll(void);


#endif
