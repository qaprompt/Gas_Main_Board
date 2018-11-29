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
#include "Bsp.h"


/* �˿�״̬ */
typedef enum {
    e_StdbusIdle = 0;
    e_StdbusRecv;
    e_StdbusRecved;
    e_StdbusSend;
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
}StdBusFram_t;

/* �˿ڽṹ�� */
typedef struct {
    StdbusState_e   e_State;

    void *          pv_Handle;
    void*           pv_DevHandle;
    INT8U           uch_Addr;

    INT8U           uch_LastByte;
    INT8U*          puc_Buff;
    INT16U          uin_BuffSize;                   //��������С
    INT16U          uin_BuffLenth;                  //��Ч���ݵĳ���

    StdBusFram_t    pst_Fram;
}StdbusPort_t;

/* �豸��Ϣ */
typedef struct {
    StdbusPort_t**  ppst_PortList;
    INT8U           uch_PortListLen;
    INT8U           uch_UsePort;
    INT8U           uch_Addr;
}StdbusDev_t;

#define DEF_STDBUS_PORTLIST_MAX    10
StdbusPort_t*   ast_PortList[DEF_STDBUS_PORTLIST_MAX];

StdbusDev_t StdbusDev = {
    ast_PortList,                   //pst_PortList
    DEF_STDBUS_PORTLIST_MAX,        //uch_PortListLen
    0,                              //uch_UsePort
    0,                              //uch_Address
};


//==================================================================================
//| �������� | Mod_StdbusInit
//|----------|----------------------------------------------------------------------
//| �������� | ��ʼ������
//|----------|----------------------------------------------------------------------
//| ������� | ��
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
BOOL Mod_StdbusInit(Stdbus_t* pst_Handle,INT8U uch_Address)
{
    pst_Handle->uch_Addr = uch_Address;
}

//==================================================================================
//| �������� | Mod_StdbusRegPort
//|----------|----------------------------------------------------------------------
//| �������� | ע��һ���˿�
//|----------|----------------------------------------------------------------------
//| ������� | ��
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
BOOL Mod_StdbusRegPort(Stdbus_t* pst_Handle, StdbusPort_t* pst_Port)
{
    if(pst_Port == NULL || pst_Port->pv_Handle == NULL || pst_Port->puc_Buff == NULL)
        return FALSE;

    if( uch_UsePort >= uch_PortListLen)
        return FALSE;

    pst_Handle->ppst_PortList[uch_UsePort++] = pst_Port;
    pst_Port->pv_DevHandle = pst_Handle;
    return TRUE;
}

//==================================================================================
//| �������� | StdbusPoll
//|----------|----------------------------------------------------------------------
//| �������� | ������ѯ���� �ڲ���������
//|----------|----------------------------------------------------------------------
//| ������� | ��
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
void Mod_StdbusPoll(void)
{
    StdbusPort_t * pst_Port = (StdBus_t *)PendMsg();

    if(pst_Port != NULL)
    {
        INT16U  uin_crc16;

        STDBUS_DBG(">>STDBUS DBG:   �������\r\n");
        GetCrc16Bit(pst_Port->auc_Buff + 1,pst_Port->uin_BuffLenth - 2,&uin_crc16);
        if (uin_crc16 == 0)
        {
            STDBUS_DBG(">>STDBUS DBG:   CRCУ��ͨ��\r\n");
            //CRCУ��ͨ��
            Mod_StdbusDealPack(pst_Port);
        }
        else
        {
            STDBUS_DBG(">>STDBUS DBG:   CRCУ�鲻ͨ��\r\n");
            Mod_StdbusRscPack(pst_Port);                  //�ͷű��˿ڵ�����
        }
    }
}

//==================================================================================
//| �������� | Mod_StdbusRscPack
//|----------|----------------------------------------------------------------------
//| �������� | �ͷ�һ���˿ڵ�����
//|----------|----------------------------------------------------------------------
//| ������� | ��
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
void Mod_StdbusRscPack(StdbusPort_t * pst_Port )
{
    pst_Port->pst_Fram->uch_Resv[0] = 0;
    pst_Port->pst_Fram->uch_Resv[1] = 0;
    pst_Port->pst_Fram->uch_Resv[2] = 0;
    pst_Port->pst_Fram->uch_Resv[3] = 0;

    pst_Port->pst_Fram->uch_AddrLen = 0;
    pst_Port->pst_Fram->uch_AddrIndex = 0;
    pst_Port->pst_Fram->puc_AddrList = 0;

    pst_Port->pst_Fram->uch_Cmd = 0;
    pst_Port->pst_Fram->uch_SubCmd = 0;
    pst_Port->pst_Fram->uin_PayLoadLenth =  0;
    pst_Port->pst_Fram->puc_PayLoad = 0;

    pst_Port->e_State = e_StdbusIdle;
    pst_Port->uch_LastByte = 0;
    pst_Port->uin_BuffLenth = 0;
}

//==================================================================================
//| �������� | Mod_StdbusPortRecvOneByte
//|----------|----------------------------------------------------------------------
//| �������� | �˿ڴ���һ���ֽڵ�����
//|----------|----------------------------------------------------------------------
//| ������� | ��
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
BOOL Mod_StdbusPortRecvOneByte(StdbusPort_t* pst_Handle,INT8U uch_Byte)
{
    if(pst_Handle == NULL || pst_Handle->pv_Handle == NULL || pst_Handle->puc_Buff == NULL)
    {
        return FALSE;
    }

    if (pst_Handle->e_State == e_StdbusIdle)
    {
        if (uch_Byte == 0x7b)
        {
            Mod_StdbusRscPack(pst_Handle);                  //�ͷű��˿ڵ�����
            pst_Handle->e_State = e_StdbusRecv;
            pst_Handle->auc_Buff[pst_Handle->uin_BuffLenth++] = uch_Byte;
        }
    }
    else if (pst_Handle->e_State == e_StdbusRecv)
    {
        if(pst_Handle->uin_BuffLenth <= pst_Handle->uin_BuffSize)
        {
            if (uch_Byte == 0x7d)                               //�ж��Ƿ���ܵ�֡β
            {
                pst_Handle->e_State = e_StdbusRecv;
                pst_Handle->auc_Buff[pst_Fram->uin_BuffLenth++] = uch_Byte;
                PostMsg((void*)pst_Fram);
            }
            else if (uch_Byte == 0x7b)                          //�ٴν��յ�֡ͷ
            {
                Mod_StdbusRscPack(pst_Handle);                  //�ͷű��˿ڵ�����
                pst_Handle->uch_State = e_StdbusRecved;
                pst_Handle->auc_Buff[pst_Fram->uin_BuffLenth++] = uch_Byte;
            }
            else                                                //�������
            {
                if(pst_Handle->uch_LastByte == 0x7c)
                    pst_Handle->auc_Buff[pst_Fram->uin_BuffLenth-1] ^= uch_Byte;
                else
                    pst_Handle->auc_Buff[pst_Fram->uin_BuffLenth++] = uch_Byte;
                pst_Handle->uch_LastByte =uch_Byte;
            }
        }
    }
}

//==================================================================================
//| �������� | Mod_StdbusDealPack
//|----------|----------------------------------------------------------------------
//| �������� | ����һ������
//|----------|----------------------------------------------------------------------
//| ������� | Stdbus���ݽṹ��
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
void Mod_StdbusDealPack(StdbusPort_t * pst_Port)
{
    INT16U i = 1;

    pst_Port->pst_Fram->uch_Resv[0]     = pst_Fram->auc_Buff[i++];
    pst_Port->pst_Fram->uch_Resv[1]     = pst_Fram->auc_Buff[i++];
    pst_Port->pst_Fram->uch_Resv[2]     = pst_Fram->auc_Buff[i++];
    pst_Port->pst_Fram->uch_Resv[3]     = pst_Fram->auc_Buff[i++];

    pst_Port->pst_Fram->uch_AddrLen     = pst_Fram->auc_Buff[i++];
    pst_Port->pst_Fram->uch_AddrIndex   = pst_Fram->auc_Buff[i++];
    pst_Port->pst_Fram->puc_AddrList    = &pst_Fram->auc_Buff[i];

    i += pst_Fram->uch_AddrLen;

    pst_Port->pst_Fram->uch_Cmd         = pst_Fram->auc_Buff[i++];
    pst_Port->pst_Fram->uch_SubCmd      = pst_Fram->auc_Buff[i++];
    pst_Port->pst_Fram->uin_PayLoadLenth    =  (uint16_t)(pst_Fram->auc_Buff[i++]<<8);
    pst_Port->pst_Fram->uin_PayLoadLenth    +=  pst_Fram->auc_Buff[i++];
    pst_Port->pst_Fram->puc_PayLoad         = &pst_Fram->auc_Buff[i];

    //�ж��Ƿ�����ĩ�ڵ�
    if(pst_Port->pst_Fram->puc_AddrList[pst_Port->pst_Fram->uch_AddrIndex] !=
        ((StdbusDev_t*)pst_Port->pv_DevHandle)->uch_Addr)
    {
        //ת���������˿�
        STDBUS_DBG(">>STDBUS DBG:   ������ĩ�ڵ�ת������һ���ڵ�\r\n");
        Mod_StdbusSend_Other(pst_Fram);
    }
    else
    {

    }
    //�����豸���ʱ��豸 ��App��ȥ����
    STDBUS_DBG(">>STDBUS DBG:   �����豸���ʱ��豸 ��App��ȥ����\r\n");
    if(0 != Deal_CmdPack(pst_Fram))
    {
        int i = 0;
        //��ת��ַ�б� ԭ·����
        for(i = 0 ; i < pst_Fram->uch_LinkLenth/2; i++)
        {
            uint8_t uch_temp = pst_Fram->puc_AddrList[pst_Fram->uch_LinkLenth -1 -i];
            pst_Fram->puc_AddrList[pst_Fram->uch_LinkLenth -1 -i] = pst_Fram->puc_AddrList[i];    //len = 4 0<>3 1<>2 // len = 5  0<>4 1<>3
            pst_Fram->puc_AddrList[i] = uch_temp;
        }
        pst_Fram->uch_Location = 1;
        pst_Fram->uch_SubCmd ^= 0xff;
        Make_ComPack(pst_Fram);
    }
    else
    {
        //����Ҫ�ظ�
        Rsc_ComPack(pst_Fram);              //�ͷű��˿ڵ�����
    }

}

//==================================================================================
//| �������� | Mod_StdbusSend_Other
//|----------|----------------------------------------------------------------------
//| �������� | ����һ������
//|----------|----------------------------------------------------------------------
//| ������� | StdbusPort_t ���ݽṹ��
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
void Mod_StdbusSend_Other(StdbusPort_t * pst_Port)
{

}
