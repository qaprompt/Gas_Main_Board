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
#include "App_Include.h"

     
#define DEF_GREYCHANNEL_DEFAULT         &st_Grey,0,0,0,0,0,0,{0}


/*
#define DEF_GREYCALIBPOINT_DEFAULT  0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
typedef struct __GreenCalibPoint {
    FP32 f_Volt;
    FP32 f_Grey;
}GreyCalibPoint;
*/


#define     DEF_GREY_DBG_EN           TRUE

#if (DEF_GREY_DBG_EN == TRUE)
    #define GREY_DBG(...)            do {                                \
                                            OS_ERR os_err;                  \
                                            OSSchedLock(&os_err);           \
                                            printf(__VA_ARGS__);            \
                                            OSSchedUnlock(&os_err);         \
                                        }while(0)
#else
    #define GREY_DBG(...)
#endif

                                          
GreyChannel_t ast_GreyChannle[10] = {
    {0,DEF_GREYCHANNEL_DEFAULT},
    {1,DEF_GREYCHANNEL_DEFAULT},
    {2,DEF_GREYCHANNEL_DEFAULT},
    {3,DEF_GREYCHANNEL_DEFAULT},
    {4,DEF_GREYCHANNEL_DEFAULT},
    {5,DEF_GREYCHANNEL_DEFAULT},
    {6,DEF_GREYCHANNEL_DEFAULT},
    {7,DEF_GREYCHANNEL_DEFAULT},
    {8,DEF_GREYCHANNEL_DEFAULT},
    {9,DEF_GREYCHANNEL_DEFAULT},
};

GreyAnalysis_t st_Grey = {
    e_GreyIdle,             //״̬
    10,                     //ͨ������
    ast_GreyChannle,        //10��ͨ��
    10.0,                   //��������ֵ
    0.0,                    //������
    0.0,                    //�Ҷ�
    0,                      //�궨����
    10,                    //�궨ʱ��
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
        i = Bsp_LTC1867SampleAvg(&st_LTC1867B,pst_Grye->uch_Num,5);
        break;
    case 8:
    case 9:
        i = Bsp_LTC1867SampleAvg(&st_LTC1867A,pst_Grye->uch_Num-8,5);
        break;
    }

    pst_Grye->f_Volt = (FP32)Bsp_LTC1867HexToVolt(i);                       //���µ�ǰ��ѹ
    //GREY_DBG(">>GREY DBG:   ͨ��%d = %x ��ѹ = %fV\r\n",pst_Grye->uch_Num,i,pst_Grye->f_Volt);
}

void Mod_GreyCalculate(GreyChannel_t* pst_Channel)
{
    //����Ҷ�
    //Frecv = Fsend * e^-kL
    //-1/L * log(e)(Fr/Fs) = -2.303/L * log(e)(1- N/100)
    //log(e)(Fr/Fs) = 2.303 * log(e)(1- N/100)
    //

    FP32 FrPerFs =  pst_Channel->f_Volt / pst_Channel->f_BkVolt;    //  Fr/Fs
    FP64 t = log(FrPerFs)/2.303;        //t = log(e)(1- N/100)
    FP64 n = 0.0;
    t = exp(t);                         //t = 1 - N/100

    n = (1-t)*100;

    pst_Channel->f_Grey = n;
}

void Mod_GreyProc(GreyChannel_t* pst_Channel)
{
    GreyAnalysis_t* pst_Manage = pst_Channel->pv_Manage;

    switch (pst_Manage->e_Status)
    {
        case e_GreyIdle:
            /* ����ʱ��ͣ���±�����ѹ */
            FP32 f = pst_Channel->f_Volt / pst_Channel->f_AbsTransVolt * 100;  //���µ�ǰ��·��͸����
            pst_Channel->f_Trans = (f > 100) ? 100:f;
          
          
            if(pst_Channel->f_Trans >= pst_Manage->f_TransThreshold)      //͸���ʴ���10%ʱ�Ÿ��±���
            {
                //һ���˲� ����ʱ�ĵ�ѹ�Ǳ���
                pst_Channel->f_BkVolt = pst_Channel->f_Volt * 0.5 + \
                                        pst_Channel->f_BkVolt * 0.5;
            }
            break;
        case e_GreyMeas:
            /* ����ʱʹ�ò����ĵ�ѹ�ͱ�����ѹ������ */
            if(pst_Channel->f_Trans >= pst_Manage->f_TransThreshold)      //͸���ʴ���10%ʱ�ż���Ҷ�
            {
                Mod_GreyCalculate(pst_Channel);                          //����Ҷ�
            }
            break;
        case e_GreyCalib:
            /* ���������궨 �ѵ�ǰ�ĵ�ѹ�趨Ϊ100%�����ĵ�ѹ */
            pst_Channel->f_BkVolt = pst_Channel->f_Volt * 0.5 + \
                                   pst_Channel->f_BkVolt * 0.5;
            pst_Channel->f_AbsTransVolt = pst_Channel->f_BkVolt;
            break;
        default:
            break;
    }
}

void Mod_GreyPoll(GreyAnalysis_t* pst_Grye)
{
    INT8U   i;
    /* ����10��ͨ����AD��ѹֵ */
    for( i = 0; i < pst_Grye->uch_ChannelNum ; i++)
    {
        Mod_GreySample(&pst_Grye->pst_Channel[i]);
    }

    for( i = 0; i < pst_Grye->uch_ChannelNum ; i++)
    {
        Mod_GreyProc(&pst_Grye->pst_Channel[i]);
    }

    if(pst_Grye->e_Status == e_GreyCalib)
    {
        if(++pst_Grye->uin_CalibCnt >= pst_Grye->uin_CalibTimeCnt)
        {
            /* ���¾��Է�ֵ��ѹ ��д��EEPROM */
            for( i = 0; i < pst_Grye->uch_ChannelNum ; i++)
            {
                pst_Grye->pst_Channel[i].f_AbsTransVolt = pst_Grye->pst_Channel[i].f_BkVolt;
                SaveToEeprom((INT32U)&pst_Grye->pst_Channel[i].f_AbsTransVolt);
            }
            Mod_GreyGotoIdle(pst_Grye);
        }
    }
    else
    {
        /* �����ܵ�͸���� */
        FP32 f = 0;
        for( i = 0; i < pst_Grye->uch_ChannelNum ; i++)
        {
            f += pst_Grye->pst_Channel[i].f_Trans;
        }
        pst_Grye->f_Trans = f / pst_Grye->uch_ChannelNum;
    }
}

void Mod_GreyGotoCalib(GreyAnalysis_t* pst_Grye)
{
    INT8U i = 0;
    pst_Grye->uin_CalibCnt = 0;
    for(i = 0; i < 10; i++)     /* ��������10·��ѹ */
        pst_Grye->pst_Channel[i].f_BkVolt = pst_Grye->pst_Channel[i].f_Volt;
    pst_Grye->e_Status = e_GreyCalib;
}

void Mod_GreyGotoMeas(GreyAnalysis_t* pst_Grye)
{
    pst_Grye->e_Status = e_GreyMeas;
}

void Mod_GreyGotoIdle(GreyAnalysis_t* pst_Grye)
{
    pst_Grye->e_Status = e_GreyIdle;
}
