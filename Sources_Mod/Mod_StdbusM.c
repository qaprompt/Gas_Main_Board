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
#include "Mod_Include.h"

             
#define STDBUS_DBG(x)                   printf(x)

StdbusDev_t*   ast_PortList[DEF_STDBUS_PORTLIST_MAX];

StdbusHost_t st_StdbusHost = {
    ast_PortList,                   //pst_DevList
    DEF_STDBUS_PORTLIST_MAX,        //uch_PortListLen
    0,                              //uch_UsePort
    0,                              //uch_Address
};

//==================================================================================
//| �������� | PostMsg
//|----------|----------------------------------------------------------------------
//| �������� | ������Ϣ(����ֲ)
//|----------|----------------------------------------------------------------------
//| ������� | ��Ϣ�ľ��
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
void PostMsg(void* pst_Hardware)
{
    OS_ERR  os_err;
    OSTaskQPost(&TaskStdBusTCB,pst_Hardware,1,OS_OPT_POST_FIFO ,&os_err);
    if(os_err != OS_ERR_NONE)
    {

    }
}

//==================================================================================
//| �������� | PendMsg
//|----------|----------------------------------------------------------------------
//| �������� | �ȴ���Ϣ (����ֲ) �ڲ���������
//|----------|----------------------------------------------------------------------
//| ������� | ��
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��Ϣ�ľ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
void* PendMsg(void)
{
    OS_ERR  os_err;
    uint16_t ui_MsgSize = 0;
    void * pv_Msg;
    pv_Msg = OSTaskQPend(500,OS_OPT_PEND_BLOCKING,&ui_MsgSize,0,&os_err);

    if(os_err == OS_ERR_NONE)
        return pv_Msg;
    else
        return 0;
}

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
BOOL Mod_StdbusInit(StdbusHost_t* pst_Host,INT8U uch_Address)
{
    pst_Host->uch_Addr = uch_Address;
    return TRUE;
}

//==================================================================================
//| �������� | Mod_StdbusRegDev
//|----------|----------------------------------------------------------------------
//| �������� | ע��һ���豸
//|----------|----------------------------------------------------------------------
//| ������� | ��
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
BOOL Mod_StdbusRegDev(StdbusHost_t* pst_Host, StdbusDev_t* pst_Dev)
{
    if(pst_Dev == NULL || pst_Dev->pv_Handle == NULL || pst_Dev->puc_Buff == NULL)
        return FALSE;

    if( pst_Host->uch_UsePort >= pst_Host->uch_PortListLen)
        return FALSE;

    pst_Host->ppst_DevList[pst_Host->uch_UsePort++] = pst_Dev;
    pst_Dev->pv_HostHandle = pst_Host;
    
    return TRUE;
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
void Mod_StdbusRscPack(StdbusDev_t* pst_Dev )
{
    pst_Dev->pst_Fram.uch_Resv[0] = 0;
    pst_Dev->pst_Fram.uch_Resv[1] = 0;
    pst_Dev->pst_Fram.uch_Resv[2] = 0;
    pst_Dev->pst_Fram.uch_Resv[3] = 0;

    pst_Dev->pst_Fram.uch_AddrLen = 0;
    pst_Dev->pst_Fram.uch_AddrIndex = 0;
    pst_Dev->pst_Fram.puc_AddrList = 0;

    pst_Dev->pst_Fram.uch_Cmd = 0;
    pst_Dev->pst_Fram.uch_SubCmd = 0;
    pst_Dev->pst_Fram.uin_PayLoadLenth =  0;
    pst_Dev->pst_Fram.puc_PayLoad = 0;

    pst_Dev->e_State = e_StdbusIdle;
    pst_Dev->uch_LastByte = 0;
    pst_Dev->uin_BuffLenth = 0;
}

//==================================================================================
//| �������� | Mod_StdbusSend_Other
//|----------|----------------------------------------------------------------------
//| �������� | ����һ������
//|----------|----------------------------------------------------------------------
//| ������� | StdbusDev_t ���ݽṹ��
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
void Mod_StdbusSend_Other(StdbusDev_t * pst_Dev)
{

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
BOOL Mod_StdbusPortRecvOneByte(StdbusDev_t* pst_Dev,INT8U uch_Byte)
{
    if(pst_Dev == NULL || pst_Dev->pv_Handle == NULL || pst_Dev->puc_Buff == NULL)
    {
        return FALSE;
    }

    if (pst_Dev->e_State == e_StdbusIdle)
    {
        if (uch_Byte == 0x7b)
        {
            Mod_StdbusRscPack(pst_Dev);                  //�ͷű��˿ڵ�����
            pst_Dev->e_State = e_StdbusRecv;
            pst_Dev->puc_Buff[pst_Dev->uin_BuffLenth++] = uch_Byte;
        }
    }
    else if (pst_Dev->e_State == e_StdbusRecv)
    {
        if(pst_Dev->uin_BuffLenth <= pst_Dev->uin_BuffSize)
        {
            if (uch_Byte == 0x7d)                               //�ж��Ƿ���ܵ�֡β
            {
                pst_Dev->e_State = e_StdbusRecved;
                pst_Dev->puc_Buff[pst_Dev->uin_BuffLenth++] = uch_Byte;
                PostMsg(pst_Dev);
            }
            else if (uch_Byte == 0x7b)                          //�ٴν��յ�֡ͷ
            {
                Mod_StdbusRscPack(pst_Dev);                  //�ͷű��˿ڵ�����
                pst_Dev->e_State = e_StdbusRecv;
                pst_Dev->puc_Buff[pst_Dev->uin_BuffLenth++] = uch_Byte;
            }
            else                                                //�������
            {
                if(pst_Dev->uch_LastByte == 0x7c)
                    pst_Dev->puc_Buff[pst_Dev->uin_BuffLenth-1] ^= uch_Byte;
                else
                    pst_Dev->puc_Buff[pst_Dev->uin_BuffLenth++] = uch_Byte;
                pst_Dev->uch_LastByte =uch_Byte;
            }
        }
    }
    return TRUE;
}

//==================================================================================
//| �������� | Mod_StdbusFindPort
//|----------|----------------------------------------------------------------------
//| �������� | ���ݵ�ַ�����˿�
//|----------|----------------------------------------------------------------------
//| ������� | ��
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
StdbusDev_t* Mod_StdbusFindPort(StdbusHost_t * pst_Host,INT8U uch_FindAddr)
{
    INT8U i = 0;
    if(pst_Host == NULL || pst_Host->ppst_DevList == NULL ||  pst_Host->uch_UsePort == 0)
    {
        return NULL;
    }

    for ( i = 0; i < pst_Host->uch_UsePort; i++)
    {
        if (pst_Host->ppst_DevList[i]->uch_Addr == uch_FindAddr)
        {
            return pst_Host->ppst_DevList[i];
        }
    }
    return NULL;
}

//==================================================================================
//| �������� | Mod_StdbusDealFram
//|----------|----------------------------------------------------------------------
//| �������� | ����һ������
//|----------|----------------------------------------------------------------------
//| ������� | Stdbus���ݽṹ��
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
BOOL Mod_StdbusDealFram(StdbusDev_t * pst_Dev)
{
    if(pst_Dev->pst_Fram.uch_SubCmd == e_StdbusReadCmd ||
       pst_Dev->pst_Fram.uch_SubCmd == e_StdbusWriteCmd)
    {
        /* �����豸���ʱ��豸 ֱ������������ַ �������˿ں���ûص�  */
        StdbusDev_t* pst_MasterPort = Mod_StdbusFindPort(pst_Dev->pv_HostHandle,((StdbusHost_t*)pst_Dev->pv_HostHandle)->uch_Addr);
        if(pst_MasterPort == NULL)
            return FALSE;
        if(pst_MasterPort->cb_DealFram != NULL)
            return pst_MasterPort->cb_DealFram(&pst_Dev->pst_Fram);        //���ûص���������֡
    }
    else if(pst_Dev->pst_Fram.uch_SubCmd == e_StdbusReadAck ||
            pst_Dev->pst_Fram.uch_SubCmd == e_StdbusWriteAck)
    {
        /* ���豸���������豸����ܵ���Ӧ�� ������ַ�б��һ����ַ Դ��ַ ��������Ӧ�˿ں���ûص�*/
        StdbusDev_t* pst_SlavePort = Mod_StdbusFindPort(pst_Dev->pv_HostHandle,pst_Dev->pst_Fram.puc_AddrList[0]);
        if(pst_SlavePort == NULL)
            return FALSE;
        if(pst_SlavePort->cb_DealFram != NULL)
            return pst_SlavePort->cb_DealFram(&pst_Dev->pst_Fram);        //���ûص���������֡
    }
    return FALSE;
}

//==================================================================================
//| �������� | Mod_StdbusMakePack
//|----------|----------------------------------------------------------------------
//| �������� | ��֡������
//|----------|----------------------------------------------------------------------
//| ������� | Stdbus���ݽṹ��
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
void Mod_StdbusMakePack(StdbusDev_t* pst_Dev)
{
    /* �ı�puc_AddrList�ĳ��ȿ��ܳ������ݸ�����ע�⣡��*/
    uint16_t i = 0,j = 0 ,crc16 = 0;

    pst_Dev->puc_Buff[i++] = 0x7b;

    pst_Dev->puc_Buff[i++] = pst_Dev->pst_Fram.uch_Resv[0];
    pst_Dev->puc_Buff[i++] = pst_Dev->pst_Fram.uch_Resv[1];
    pst_Dev->puc_Buff[i++] = pst_Dev->pst_Fram.uch_Resv[2];
    pst_Dev->puc_Buff[i++] = pst_Dev->pst_Fram.uch_Resv[3];

    pst_Dev->puc_Buff[i++] = pst_Dev->pst_Fram.uch_AddrLen;
    pst_Dev->puc_Buff[i++] = pst_Dev->pst_Fram.uch_AddrIndex;

    for( j = 0; j < pst_Dev->pst_Fram.uch_AddrLen; j++)
        pst_Dev->puc_Buff[i++] = pst_Dev->pst_Fram.puc_AddrList[j];

    pst_Dev->puc_Buff[i++] = pst_Dev->pst_Fram.uch_Cmd;
    pst_Dev->puc_Buff[i++] = pst_Dev->pst_Fram.uch_SubCmd;
    pst_Dev->puc_Buff[i++] = (uint8_t)(pst_Dev->pst_Fram.uin_PayLoadLenth>>8);
    pst_Dev->puc_Buff[i++] = (uint8_t)(pst_Dev->pst_Fram.uin_PayLoadLenth&0xff);

    for( j = 0; j < pst_Dev->pst_Fram.uin_PayLoadLenth; j++)
        pst_Dev->puc_Buff[i++] = pst_Dev->pst_Fram.puc_PayLoad[j];

    GetCrc16Bit(pst_Dev->puc_Buff + 1,i-1, &crc16);
    pst_Dev->puc_Buff[i++] = (uint8_t)(crc16 >> 8);
    pst_Dev->puc_Buff[i++] = (uint8_t)(crc16 );

    pst_Dev->puc_Buff[i++] = 0x7d;
    pst_Dev->uin_BuffLenth = i;

    pst_Dev->e_State = e_StdbusSend;
    //Send_ComPack(pst_Fram);
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
void Mod_StdbusDealPack(StdbusDev_t* pst_Dev)
{
    INT16U i = 1;

    pst_Dev->pst_Fram.uch_Resv[0]     = pst_Dev->puc_Buff[i++];
    pst_Dev->pst_Fram.uch_Resv[1]     = pst_Dev->puc_Buff[i++];
    pst_Dev->pst_Fram.uch_Resv[2]     = pst_Dev->puc_Buff[i++];
    pst_Dev->pst_Fram.uch_Resv[3]     = pst_Dev->puc_Buff[i++];

    pst_Dev->pst_Fram.uch_AddrLen     = pst_Dev->puc_Buff[i++];
    pst_Dev->pst_Fram.uch_AddrIndex   = pst_Dev->puc_Buff[i++];
    pst_Dev->pst_Fram.puc_AddrList    = &pst_Dev->puc_Buff[i];

    i += pst_Dev->pst_Fram.uch_AddrLen;

    pst_Dev->pst_Fram.uch_Cmd         = pst_Dev->puc_Buff[i++];
    pst_Dev->pst_Fram.uch_SubCmd      = pst_Dev->puc_Buff[i++];
    pst_Dev->pst_Fram.uin_PayLoadLenth    =  (uint16_t)(pst_Dev->puc_Buff[i++]<<8);
    pst_Dev->pst_Fram.uin_PayLoadLenth    +=  pst_Dev->puc_Buff[i++];
    pst_Dev->pst_Fram.puc_PayLoad         =  &pst_Dev->puc_Buff[i];

    //�ж��Ƿ�����ĩ�ڵ�
    if(pst_Dev->pst_Fram.puc_AddrList[pst_Dev->pst_Fram.uch_AddrIndex] !=
        ((StdbusHost_t*)pst_Dev->pv_HostHandle)->uch_Addr)
    {
        //ת���������˿�
        STDBUS_DBG(">>STDBUS DBG:   ������ĩ�ڵ�ת������һ���ڵ�\r\n");
        Mod_StdbusSend_Other(pst_Dev);
        Mod_StdbusRscPack(pst_Dev);                    //�ͷű��˿ڵ�����
    }
    else
    {
        //�����豸���ʱ��豸 ��App��ȥ����
        STDBUS_DBG(">>STDBUS DBG:   �����豸���ʱ��豸 ��App��ȥ����\r\n");
        if(TRUE == Mod_StdbusDealFram(pst_Dev))
        {
            int i = 0;
            //��ת��ַ�б� ԭ·����
            for(i = 0 ; i < pst_Dev->pst_Fram.uch_AddrLen/2; i++)
            {
                uint8_t uch_temp = pst_Dev->pst_Fram.puc_AddrList[pst_Dev->pst_Fram.uch_AddrLen -1 -i];
                pst_Dev->pst_Fram.puc_AddrList[pst_Dev->pst_Fram.uch_AddrLen -1 -i] = pst_Dev->pst_Fram.puc_AddrList[i];    //len = 4 0<>3 1<>2 // len = 5  0<>4 1<>3
                pst_Dev->pst_Fram.puc_AddrList[i] = uch_temp;
            }
            pst_Dev->pst_Fram.uch_AddrIndex = 1;           //�ӵ�һ����ַλ�ÿ�ʼ
            pst_Dev->pst_Fram.uch_SubCmd ^= 0xff;          //ȡ��������
            Mod_StdbusMakePack(pst_Dev);
        }
        else
        {
            //����Ҫ�ظ�
            Mod_StdbusRscPack(pst_Dev);                    //�ͷű��˿ڵ�����
        }
    }


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
    StdbusDev_t * pst_Dev = (StdbusDev_t*)PendMsg();

    if(pst_Dev != NULL)
    {
        INT16U  uin_crc16;

        STDBUS_DBG(">>STDBUS DBG:   �������\r\n");
        GetCrc16Bit(pst_Dev->puc_Buff + 1,pst_Dev->uin_BuffLenth - 2,&uin_crc16);
        if (uin_crc16 == 0)
        {
            STDBUS_DBG(">>STDBUS DBG:   CRCУ��ͨ��\r\n");
            //CRCУ��ͨ��
            Mod_StdbusDealPack(pst_Dev);
        }
        else
        {
            STDBUS_DBG(">>STDBUS DBG:   CRCУ�鲻ͨ��\r\n");
            Mod_StdbusRscPack(pst_Dev);                  //�ͷű��˿ڵ�����
        }
    }
}
