#include "Mod_Include.h"


#define DEF_STDBUS_SLAVE_PORT  &COM4
#define DEF_STDBUS_SLAVE_BUFF_LEN   100
#define DEF_STDBUS_SLAVE_DEV_LEN    10


static INT8U auc_Buff[DEF_STDBUS_SLAVE_BUFF_LEN] = {0};
static StdbusDev_t* ast_DevList[DEF_STDBUS_SLAVE_DEV_LEN] = {0};

StdbusPort_t st_StdbusSlave ={
    e_StdbusIdle,                           /* ״̬ */
    &Sem_Rs485,                             /* �˿�ռ����  OS�������һ���ź��� */
    &TaskStdBusSlaveTCB,                    /* �˿���Ϣ OS�������һ����Ϣ����*/
    DEF_STDBUS_SLAVE_PORT,                  /* ���ھ�� */
    NULL,                                   /* ������� */
    ast_DevList,                            /* �豸�б� */
    DEF_STDBUS_SLAVE_DEV_LEN,               /* �豸�б��� */
    0,                                      /* ��ʹ�õ��豸���� */

    0,                                      /* ���һ���ֽ� */
    auc_Buff,                               /* ���ݻ����� */
    DEF_STDBUS_SLAVE_BUFF_LEN,              /* ��������С */
    0,                                      /* ��Ч���ݵĳ��� */
    0,                                      /* ������������ */

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
};




static void Mod_SendComplete(void * pv_dev)
{
    Mod_StdbusPortSendOneByte(&st_StdbusSlave);
}

static void Mod_RecvReady(void * pv_dev, INT8U* puch_Buff, INT16U uin_Len)
{
    INT16U   i;
    for(i = 0; i < uin_Len; i++)
        Mod_StdbusPortRecvOneByte(&st_StdbusSlave,puch_Buff[i]);
}

static void Mod_ErrHandle(void * pv_dev)
{
    Mod_StdbusRscPack(&st_StdbusSlave);                  //�ͷű��˿ڵ�����
    Bsp_UartClose(pv_dev);
    Bsp_UartClose(pv_dev);
}

void Mod_StdbusSlaveInit(void)
{
    Mod_StdbusRegPort(&st_StdbusHost,&st_StdbusSlave);
    Mod_StdbusRegDev(&st_StdbusSlave,st_MeasSpeed.pst_Handle);
    Mod_StdbusRegDev(&st_StdbusSlave,st_DisBoard.pst_Handle);
    ((Dev_SerialPort*)st_StdbusSlave.pv_Handle)->cb_SendComplete = Mod_SendComplete;
    ((Dev_SerialPort*)st_StdbusSlave.pv_Handle)->cb_RecvReady =  Mod_RecvReady;
    ((Dev_SerialPort*)st_StdbusSlave.pv_Handle)->cb_ErrHandle = Mod_ErrHandle;
}

void Mod_StdbusSlavePoll(void)
{
    Mod_StdbusPortPoll(&st_StdbusSlave);
}
