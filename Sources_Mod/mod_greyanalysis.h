//==================================================================================================
//| �ļ����� | Mod_GreyAnalysis.h
//|--------- |--------------------------------------------------------------------------------------
//| �ļ����� | �̹�Ҷȴ���
//|--------- |--------------------------------------------------------------------------------------
//| ��Ȩ���� |
//|----------|--------------------------------------------------------------------------------------
//|  �汾    |  ʱ��       |  ����     | ����
//|--------- |-------------|-----------|------------------------------------------------------------
//|  V1.0    | 2018.11.02  |  wjb      | ���� ����֤�Ƿ�����DMA������ȡLT1867
//==================================================================================================
#ifndef __MOD_GREYANALYSIS_H__
#define __MOD_GREYANALYSIS_H__

#include "bsp.h"

#define     DEF_GREY_SAMPLEDOT_MAX      10

typedef enum   __GreyAnalysisStaus {
    e_GreyIdle = 0,e_GreyMeas,e_GreyCalib,e_GreyWait,
}GreyAnalysisStaus_t;

typedef struct __GreyChannel {
    INT8U                   uch_Num;            //ͨ�����
    void*                   pv_Manage;          //����ṹ��
    FP32                    f_Volt;             //��ǰ��ѹ
    FP32                    f_BkVolt;           //������ѹ
    FP32                    f_AbsTransVolt;     //����͸���ʵ�ѹ
    FP32                    f_Trans;            //͸����
    FP32                    f_Grey;             //�Ҷ�
    INT8U                   uch_SampleLen;      //������鳤��
    FP32                    af_SampleBuff[DEF_GREY_SAMPLEDOT_MAX];      //�������
}GreyChannel_t;

typedef struct __GreyAnalysis
{
    GreyAnalysisStaus_t     e_Status;           //״̬
    INT8U                   uch_ChannelNum;     //ͨ������
    GreyChannel_t*          pst_Channel;        //10��ͨ��
    FP32                    f_TransThreshold;   //��������ֵ
    FP32                    f_Trans;            //������
    FP32                    f_Grey;             //�Ҷ�
    INT16U                  uin_CalibCnt;       //�궨����
    INT16U                  uin_CalibTimeCnt;   //�궨ʱ��
}GreyAnalysis_t;

extern GreyChannel_t ast_GreyChannle[10];
extern GreyAnalysis_t st_Grey;

void Mod_GreyPoll(GreyAnalysis_t* pst_Grye);

void Mod_GreyGotoCalib(GreyAnalysis_t* pst_Grye);

void Mod_GreyGotoMeas(GreyAnalysis_t* pst_Grye);

void Mod_GreyGotoIdle(GreyAnalysis_t* pst_Grye);

void Mod_GreyGotoWait(GreyAnalysis_t* pst_Grye);

#endif
