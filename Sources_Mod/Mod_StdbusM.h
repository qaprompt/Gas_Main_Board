//==================================================================================================
//| �ļ����� | Mod_StdbusM.c
//|--------- |--------------------------------------------------------------------------------------
//| �ļ����� | StdbusЭ����������
//|          | �ȴ������� ����������Ӷ˿�
//|--------- |--------------------------------------------------------------------------------------
//| ��Ȩ���� |
//|----------|--------------------------------------------------------------------------------------
//|  �汾    |  ʱ��       |  ����     | ����
//|--------- |-------------|-----------|------------------------------------------------------------
//|  V1.0    | 2018.11.29  |  wjb      |
//==================================================================================================
#ifndef __MOD_STDBUSM_H__
#define __MOD_STDBUSM_H__

#include "Bsp.h"

#define     DEF_STDBUS_HOST_ADDR            0x10        /* ������ַ */
#define     DEF_STDBUS_PORTLIST_MAX         10          /* ��������������Ķ˿� */
#define     DEF_STDBUS_DEV_ADDRLIST_MAX     4           /* �豸��ַ��󳤶� */


/* STDBUS ���� */
typedef enum {
    e_StdbusReadCmd = 0x55,
    e_StdbusReadAck = 0xaa,
    e_StdbusWriteCmd = 0x66,
    e_StdbusWriteAck = 0x99,
}StdbusSubCmd;

/* �˿�״̬ */
typedef enum {
    e_StdbusIdle = 0,
    e_StdbusRecv,
    e_StdbusRecved,
    e_StdbusSend,
}StdbusState_e;

/* STD����֡ �ṹ�� */
typedef struct {
    uint8_t     uch_Resv[4];        /*����*/
    uint8_t     uch_AddrLen;        /*��ַ�б���*/
    uint8_t     uch_AddrIndex;      /*��ǰλ��*/
    uint8_t*    puc_AddrList;       /*��ַ�б�*/
    uint8_t     uch_Cmd;            /*��������*/
    uint8_t     uch_SubCmd;         /*�������� 0x55 0x66 0xaa 0x99*/
    uint16_t    uin_PayLoadLenth;   /*�����غɳ���*/
    uint8_t*    puc_PayLoad;        /*�����غ�*/
}StdbusFram_t;

/* STDBUS�豸(�ӻ�)�ṹ�� */
typedef struct {
    INT8U           puc_AddrList[DEF_STDBUS_DEV_ADDRLIST_MAX + 1];  /*��ַ�б�*/
    INT8U           uch_AddrLen;                            /*��ַ�б���*/
    void*           pv_PortHandle;                          /*�˿ھ��*/
    BOOL            (*cb_DealFram)(StdbusFram_t* pst_Fram); /*������*/
}StdbusDev_t;


/* �˿ڽṹ�� */
typedef struct {
    StdbusState_e   e_State;                            /* ״̬ */

    void *          pv_Handle;                          /* ���ھ�� */
    void*           pv_HostHandle;                      /* ������� */
    StdbusDev_t**   ppst_DevList;                      /* �豸�б� */
    INT8U           uch_DevListLen;                     /* �豸�б��� */
    INT8U           uch_DevUse;                        /* ��ʹ�õ��豸���� */

    INT8U           uch_LastByte;                       /* ���һ���ֽ� */
    INT8U*          puc_Buff;                           /* ���ݻ����� */
    INT16U          uin_BuffSize;                       /* ��������С */
    INT16U          uin_BuffLenth;                      /* ��Ч���ݵĳ��� */
    INT16U          uin_BuffIndex;                      /* ������������ */
    StdbusFram_t    pst_Fram;
}StdbusPort_t;

/* �豸��Ϣ */
typedef struct {
    StdbusPort_t**  ppst_PortList;
    INT8U           uch_PortListLen;
    INT8U           uch_UsePort;
    INT8U           uch_Addr;
}StdbusHost_t;


extern StdbusHost_t st_StdbusHost;

__weak BOOL App_StdbusMasterDealFram(StdbusFram_t* pst_Fram);

BOOL Mod_StdbusInit(StdbusHost_t* pst_Host,INT8U uch_Address);

BOOL Mod_StdbusRegPort(StdbusHost_t* pst_Host, StdbusPort_t* pst_Port);

void Mod_StdbusRscPack(StdbusPort_t* pst_Port );

void Mod_StdbusPortSendOneByte(StdbusPort_t * pst_Port);

BOOL Mod_StdbusPortRecvOneByte(StdbusPort_t* pst_Port,INT8U uch_Byte);

void Mod_StdbusPoll(void);

void Mod_StdbusWriteCmd(StdbusDev_t* pst_Dev,INT8U uch_Cmd,INT8U* puc_Payload, INT16U puc_PayloadLen);

void Mod_StdbusReadCmd(StdbusDev_t* pst_Dev,INT8U uch_Cmd,INT8U* puc_Payload, INT16U puc_PayloadLen);


#endif
