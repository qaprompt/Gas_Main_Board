#include "Mod_Include.h"

#define DEF_STDBUS_MASTER_PORT  &COM2
#define DEF_STDBUS_MASTER_ADDR  0x30

static INT8U auc_Buff[1024] = {0};

StdbusDev_t st_StdbusMaster ={
    e_StdbusIdle,                       /* ״̬ */           
    DEF_STDBUS_MASTER_PORT,             /* ���ھ�� */ 
    NULL,                               /* ������� */      
    DEF_STDBUS_MASTER_ADDR,             /* �豸��ַ */ 
                                              
    0,                                  /* ���һ���ֽ� */
    auc_Buff,                           /* ���ݻ����� */
    1024,                               /* ��������С */
    0,                                  /* ��Ч���ݵĳ��� */
                                        
    {
        {0,0,0,0},                          /*����*/
        0,                                  /*��ַ�б���*/
        0,                                  /*��ǰλ��*/
        0,                                  /*������*/
        0,                                  /*��������*/
        0,                                  /*�������� 0x55 0x66 0xaa 0x99*/
        0,                                  /*�����غɳ���*/
        0,                                  /*�����غ�*/
    },

    NULL,                                   /*������*/
};

__weak BOOL App_StdbusMasterDealFram(StdbusFram_t* pst_Fram);

static void Mod_SendComplete(void * pv_dev)
{
    //Mod_StdbusPortSendOneByte(&st_StdbusMaster);
}

static void Mod_RecvReady(void * pv_dev, INT8U* puch_Buff, INT16U uin_Len)
{
    INT16U   i;
    for(i = 0; i < uin_Len; i++)
        Mod_StdbusPortRecvOneByte(&st_StdbusMaster,puch_Buff[i]);
}

static void Mod_ErrHandle(void * pv_dev)
{
    Mod_StdbusRscPack(&st_StdbusMaster);                  //�ͷű��˿ڵ�����
    Bsp_UartClose(pv_dev);
    Bsp_UartClose(pv_dev);
}

void Mod_StdbusMasterInit(void)
{
    Mod_StdbusInit(&st_StdbusHost,DEF_STDBUS_MASTER_ADDR);
    Mod_StdbusRegDev(&st_StdbusHost,&st_StdbusMaster);
    st_StdbusMaster.cb_DealFram = App_StdbusMasterDealFram;
    ((Dev_SerialPort*)st_StdbusMaster.pv_Handle)->cb_SendComplete = Mod_SendComplete;
    ((Dev_SerialPort*)st_StdbusMaster.pv_Handle)->cb_RecvReady =  Mod_RecvReady;
    ((Dev_SerialPort*)st_StdbusMaster.pv_Handle)->cb_ErrHandle = Mod_ErrHandle;
}

__weak BOOL App_StdbusMasterDealFram(StdbusFram_t* pst_Fram)  //������
{
    return FALSE;
}

