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
    uint8_t*    puc_AddrList;       /*������*/
    uint8_t     uch_Cmd;            /*��������*/
    uint8_t     uch_SubCmd;         /*�������� 0x55 0x66 0xaa 0x99*/
    uint16_t    uin_PayLoadLenth;   /*�����غɳ���*/
    uint8_t*    puc_PayLoad;        /*�����غ�*/
}StdbusFram_t;

/* �˿ڽṹ�� */
typedef struct {
    StdbusState_e   e_State;                            /* ״̬ */                        

    void *          pv_Handle;                          /* ���ھ�� */        
    void*           pv_HostHandle;                       /* ������� */
    INT8U           uch_Addr;                           /* �豸��ַ */        

    INT8U           uch_LastByte;                       /* ���һ���ֽ� */
    INT8U*          puc_Buff;                           /* ���ݻ����� */        
    INT16U          uin_BuffSize;                       /* ��������С */
    INT16U          uin_BuffLenth;                      /* ��Ч���ݵĳ��� */
    StdbusFram_t    pst_Fram;
    BOOL            (*cb_DealFram)(StdbusFram_t* pst_Fram);  //������
}StdbusDev_t;

/* �豸��Ϣ */
typedef struct {
    StdbusDev_t**  ppst_DevList;
    INT8U           uch_PortListLen;
    INT8U           uch_UsePort;
    INT8U           uch_Addr;
}StdbusHost_t;



#define DEF_STDBUS_PORTLIST_MAX    10

extern StdbusHost_t st_StdbusHost;



BOOL Mod_StdbusInit(StdbusHost_t* pst_Host,INT8U uch_Address);

BOOL Mod_StdbusRegDev(StdbusHost_t* pst_Host, StdbusDev_t* pst_Dev);

void Mod_StdbusRscPack(StdbusDev_t* pst_Dev );

BOOL Mod_StdbusPortRecvOneByte(StdbusDev_t* pst_Dev,INT8U uch_Byte);

void Mod_StdbusPoll(void);



#endif