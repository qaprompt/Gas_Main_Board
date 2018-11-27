//==================================================================================================
//| �ļ����� | Mod_GreyAnalysis.c
//|--------- |--------------------------------------------------------------------------------------
//| �ļ����� | �̹�Ҷȴ���
//|--------- |--------------------------------------------------------------------------------------
//| ��Ȩ���� | 
//|----------|--------------------------------------------------------------------------------------
//|  �汾    |  ʱ��       |  ����     | ����
//|--------- |-------------|-----------|------------------------------------------------------------
//|  V1.0    | 2018.11.02  |  wjb      | ���� ����֤�Ƿ�����DMA������ȡLT1867
//==================================================================================================
#include "mod_greyanalysis.h"

#define DEF_GREYCHANNEL_DEFAULT    0,0,0,0
/*
#define DEF_GREYCALIBPOINT_DEFAULT  0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
typedef struct __GreenCalibPoint {
    FP32 f_Volt;
    FP32 f_Grey;
}GreyCalibPoint;
*/


GreyChannel_t ast_GreyChannle[10] = {
    0,DEF_GREYCHANNEL_DEFAULT,
    1,DEF_GREYCHANNEL_DEFAULT,
    2,DEF_GREYCHANNEL_DEFAULT,
    3,DEF_GREYCHANNEL_DEFAULT,
    4,DEF_GREYCHANNEL_DEFAULT,
    5,DEF_GREYCHANNEL_DEFAULT,
    6,DEF_GREYCHANNEL_DEFAULT,
    7,DEF_GREYCHANNEL_DEFAULT,
    8,DEF_GREYCHANNEL_DEFAULT,
    9,DEF_GREYCHANNEL_DEFAULT,
};

GreyAnalysis_t st_GreyMoudle = {
    e_GreyIdle,             //״̬
    10,                     //ͨ������
    ast_GreyChannle,        //10������ͨ��
    0.0,                    //͸����    
    0.0,                    //�Ҷ�
};



void Mod_GreySample(GreyChannel_t* pst_Grye)
{
    INT16U i = 0;
    switch(pst_Grye->uch_Num)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
        i = Bsp_LTC1867SampleAvg(&st_LTC1867B,pst_Grye->uch_Num,50);
        break;
    case 8:
    case 9:
        i = Bsp_LTC1867SampleAvg(&st_LTC1867A,pst_Grye->uch_Num-8,50);
        break;
    }

    pst_Grye->f_Volt = (FP32)Bsp_LTC1867HexToVolt(i);                       //���µ�ǰ��ѹ
    pst_Grye->f_Trans = pst_Grye->f_Volt / pst_Grye->f_AbsTransVolt * 100;  //���µ�ǰ��·��͸����
    
}

void Mod_GreyProce(GreyAnalysis_t* pst_Grye)
{
    INT8U   i;
    /* ����10��ͨ����AD��ѹֵ */
    for( i = 0; i < pst_Grye->uch_ChannelNum ; i++)
    {
        Mod_GreySample(&pst_Grye->pst_Channel[i]);
    }
    
    for( i = 0; i < pst_Grye->uch_ChannelNum ; i++)
    {
        if(pst_Grye->pst_Channel[i].f_Trans >= pst_Grye->f_TransThreshold)      //͸���ʴ���10%ʱ��
        {
            if(pst_Grye->e_Status == e_GreyIdle)
            {
                //һ���˲� ����ʱ�ĵ�ѹ�Ǳ���
                pst_Grye->pst_Channel[i].f_BkVolt = pst_Grye->pst_Channel[i].f_Volt * 0.5 + \
                                                    pst_Grye->pst_Channel[i].f_BkVolt * 0.5;
            }
            else
            {
                //����Ҷ�
                //Frecv = Fsend * e^-kL
                //-1/L * log(e)(Fr/Fs) = -2.303/L * log(e)(1- N/100)
                //log(e)(Fr/Fs) = 2.303 * log(e)(1- N/100)
                //
                FP32 FrPerFs =  pst_Grye->pst_Channel[i].f_Volt / pst_Grye->pst_Channel[i].f_BkVolt;
     
                FP64 t = log(FrPerFs)/2.303;        //t = log(e)(1- N/100)
                
                FP64 n = 0.0;
                
                t = exp(t);                         //t = 1 - N/100
                
                n = (1-t)*100;
                
                pst_Grye->pst_Channel[i].f_Grey = n;
            }
        }
    }  

}
