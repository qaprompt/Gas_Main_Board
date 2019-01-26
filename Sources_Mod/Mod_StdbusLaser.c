//==================================================================================================
//| �ļ����� | Mod_StdbusLaser.c
//|--------- |--------------------------------------------------------------------------------------
//| �ļ����� | �����ͨѶ���
//|--------- |--------------------------------------------------------------------------------------
//| ��Ȩ���� |
//|----------|--------------------------------------------------------------------------------------
//|  �汾    |  ʱ��       |  ����     | ����
//|--------- |-------------|-----------|------------------------------------------------------------
//|  V1.0    | 2018.12.03  |  wjb      | ����
//==================================================================================================
#include "App_Include.h"


#define DEF_STDBUS_LASER_PORT  &COM2
#define DEF_STDBUS_LASER_BUFF_LEN   1024
#define DEF_STDBUS_LASER_DEV_LEN    1


static INT8U auc_Buff[DEF_STDBUS_LASER_BUFF_LEN] = {0};
static StdbusDev_t* ast_DevList[DEF_STDBUS_LASER_DEV_LEN] = {0};

StdbusPort_t st_StdbusLaser ={
    "�����˿�",                             /* �˿����� */
    e_StdbusIdle,                           /* ״̬ */
    &Sem_Laser,                          /* �˿�ռ����  OS�������һ���ź��� */
    &TaskStdBusLaserTCB,                    /* �˿���Ϣ OS�������һ����Ϣ����*/
    DEF_STDBUS_LASER_PORT,                  /* ���ھ�� */
    NULL,                                   /* ������� */
    ast_DevList,                            /* �豸�б� */
    DEF_STDBUS_LASER_DEV_LEN,               /* �豸�б��� */
    0,                                      /* ��ʹ�õ��豸���� */

    0,                                      /* ���һ���ֽ� */
    auc_Buff,                               /* ���ݻ����� */
    DEF_STDBUS_LASER_BUFF_LEN,              /* ��������С */
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
    Mod_StdbusPortSendOneByte(&st_StdbusLaser);
}

static void Mod_RecvReady(void * pv_dev, INT8U* puch_Buff, INT16U uin_Len)
{
    INT16U   i;
    for(i = 0; i < uin_Len; i++)
        Mod_StdbusPortRecvOneByte(&st_StdbusLaser,puch_Buff[i]);
}

static void Mod_ErrHandle(void * pv_dev)
{
    Mod_StdbusRscPack(&st_StdbusLaser);                  //�ͷű��˿ڵ�����
    Bsp_UartClose(pv_dev);
    Bsp_UartOpen(pv_dev);
}

void Mod_StdbusLaserInit(void)
{
    Mod_StdbusRegPort(&st_StdbusHost,&st_StdbusLaser);
    Mod_StdbusRegDev(&st_StdbusLaser,st_Laser.pst_Handle);
    ((Dev_SerialPort*)st_StdbusLaser.pv_Handle)->cb_SendComplete = Mod_SendComplete;
    ((Dev_SerialPort*)st_StdbusLaser.pv_Handle)->cb_RecvReady =  Mod_RecvReady;
    ((Dev_SerialPort*)st_StdbusLaser.pv_Handle)->cb_ErrHandle = Mod_ErrHandle;
}

void Mod_StdbusLaserPoll(void)
{
    Mod_StdbusPortPoll(&st_StdbusLaser);
}
