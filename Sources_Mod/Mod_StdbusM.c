//==================================================================================================
//| �ļ����� | Mod_StdbusM.c
//|--------- |--------------------------------------------------------------------------------------
//| �ļ����� | StdbusЭ����������
//|          | �ȴ������� ����������Ӷ˿� �����˿�������豸
//|--------- |--------------------------------------------------------------------------------------
//| ��Ȩ���� |
//|----------|--------------------------------------------------------------------------------------
//|  �汾    |  ʱ��       |  ����     | ����
//|--------- |-------------|-----------|------------------------------------------------------------
//|  V1.0    | 2018.11.29  |  wjb      |
//==================================================================================================
#include "App_Include.h"

#define DEF_STDBUS_DBG_EN           TRUE

#if (DEF_STDBUS_DBG_EN == TRUE)
    #define STDBUS_DBG(...)             do {                                \
                                            OS_ERR os_err;                  \
                                            OSSchedLock(&os_err);           \
                                            printf(__VA_ARGS__);            \
                                            OSSchedUnlock(&os_err);         \
                                        }while(0)
#else
    #define STDBUS_DBG(...)             
#endif

StdbusPort_t*   ast_PortList[DEF_STDBUS_PORTLIST_MAX];

StdbusHost_t st_StdbusHost = {
    ast_PortList,                   //pst_PortList
    DEF_STDBUS_PORTLIST_MAX,        //uch_PortListLen
    0,                              //uch_UsePort
    DEF_STDBUS_HOST_ADDR,           //uch_Address
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
void PostMsg(StdbusPort_t* pst_Port)
{
    OS_ERR  os_err;
    OSTaskQPost(pst_Port->pv_Msg,(void*)pst_Port->e_State,1,OS_OPT_POST_FIFO ,&os_err);
    if(os_err != OS_ERR_NONE)
    {
        switch( os_err )
        {
        case OS_ERR_MSG_POOL_EMPTY:
            STDBUS_DBG(">>STDBUS DBG:   %s OS_ERR_MSG_POOL_EMPTY\r\n",pst_Port->pch_Name);
            break;
        case OS_ERR_OPT_INVALID:
            STDBUS_DBG(">>STDBUS DBG:   %s OS_ERR_OPT_INVALID\r\n",pst_Port->pch_Name);
            break;
        case OS_ERR_OS_NOT_RUNNING:
            STDBUS_DBG(">>STDBUS DBG:   %s OS_ERR_OS_NOT_RUNNING\r\n",pst_Port->pch_Name); 
            break;
        case OS_ERR_Q_MAX:
            STDBUS_DBG(">>STDBUS DBG:   %s OS_ERR_Q_MAX\r\n",pst_Port->pch_Name);
            break;
        case OS_ERR_STATE_INVALID:
            STDBUS_DBG(">>STDBUS DBG:   %s OS_ERR_STATE_INVALID\r\n",pst_Port->pch_Name);
            break;
        default:
            break;
        }
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
void* PendMsg(StdbusPort_t* pst_Port)
{
    OS_ERR  os_err;
    uint16_t ui_MsgSize = 0;
    void * pv_Msg;
    pv_Msg = OSTaskQPend(0,OS_OPT_PEND_BLOCKING,&ui_MsgSize,0,&os_err);

    if(os_err == OS_ERR_NONE)
        return pv_Msg;
    else
        return 0;
}


BOOL UnLockPort(StdbusPort_t* pst_Port)
{
    if(pst_Port == NULL || pst_Port->pv_Lock == NULL)
        return FALSE;
#ifdef  OS_SUPPORT
    OS_ERR os_err;
    if(pst_Port->pv_Lock == NULL)                                       /*������ֻ��һ���豸����Ҫ�� */
        return TRUE;
    OSSemPost(pst_Port->pv_Lock,OS_OPT_POST_1,&os_err);                /* �������ռ�� */
    if(os_err != OS_ERR_NONE)
    {
        switch( os_err )
        {
        case OS_ERR_OBJ_PTR_NULL:
            STDBUS_DBG(">>STDBUS DBG:   %s OS_ERR_OBJ_PTR_NULL\r\n",pst_Port->pch_Name);
            break;
        case OS_ERR_OBJ_TYPE:
            STDBUS_DBG(">>STDBUS DBG:   %s OS_ERR_OBJ_TYPE\r\n",pst_Port->pch_Name);
            break;
        case OS_ERR_OPT_INVALID:
            STDBUS_DBG(">>STDBUS DBG:   %s OS_ERR_OPT_INVALID\r\n",pst_Port->pch_Name);
            break;
        case OS_ERR_OS_NOT_RUNNING:
            STDBUS_DBG(">>STDBUS DBG:   %s OS_ERR_OS_NOT_RUNNING\r\n",pst_Port->pch_Name); 
            break;
        case OS_ERR_SEM_OVF:
            STDBUS_DBG(">>STDBUS DBG:   %s OS_ERR_SEM_OVF\r\n",pst_Port->pch_Name);
            break;
        default:
            break;
        }
    }
    return TRUE;
#else
    pst_Port->pv_Lock = (void*)FALSE;
    return TRUE;
#endif
}

BOOL LockPort(StdbusPort_t* pst_Port)
{
    if(pst_Port == NULL)
        return FALSE;

#ifdef  OS_SUPPORT
    OS_ERR os_err;
    if(pst_Port->pv_Lock == NULL)                                       /*������ֻ��һ���豸����Ҫ�� */
        return TRUE;
    OSSemPend(pst_Port->pv_Lock, 0,OS_OPT_PEND_BLOCKING,NULL,&os_err);  /* ռ������ */
    return TRUE;
#else
    if(pst_Port->pv_Lock == TRUE)
    {
        return FALSE;                           /*�����ѱ�ռ��*/
    }
    else
    {
        pst_Port->pv_Lock = (void*)TRUE;        /*ռ������*/
        return TRUE;
    }
#endif
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
//| �������� | Mod_StdbusRegPort
//|----------|----------------------------------------------------------------------
//| �������� | ע��һ���˿ڵ�����
//|----------|----------------------------------------------------------------------
//| ������� | ��
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
BOOL Mod_StdbusRegPort(StdbusHost_t* pst_Host, StdbusPort_t* pst_Port)
{
    if(pst_Port == NULL || pst_Port->pv_Handle == NULL || pst_Port->puc_Buff == NULL)
        return FALSE;

    if( pst_Host->uch_UsePort >= pst_Host->uch_PortListLen)
        return FALSE;

    pst_Host->ppst_PortList[pst_Host->uch_UsePort++] = pst_Port;
    pst_Port->pv_HostHandle = pst_Host;

    return TRUE;
}

//==================================================================================
//| �������� | Mod_StdbusRegDev
//|----------|----------------------------------------------------------------------
//| �������� | ע��һ���豸���˿�
//|----------|----------------------------------------------------------------------
//| ������� | ��
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
BOOL Mod_StdbusRegDev(StdbusPort_t* pst_Port,StdbusDev_t* pst_Dev)
{
    StdbusHost_t* pst_Host = pst_Port->pv_HostHandle;

    if(pst_Dev == NULL || pst_Port == NULL || pst_Port->pv_Handle == NULL ||
       pst_Port->ppst_DevList == NULL || pst_Port->uch_DevListLen == 0 )
        return FALSE;

    if( pst_Port->uch_DevUse >= pst_Port->uch_DevListLen)
        return FALSE;

    pst_Port->ppst_DevList[pst_Port->uch_DevUse++] = pst_Dev;
    pst_Dev->pv_PortHandle = pst_Port;
    pst_Dev->puc_AddrList[0] = pst_Host->uch_Addr;
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
void Mod_StdbusRscPack(StdbusPort_t* pst_Port )
{
    if(pst_Port == NULL)
        return;
    
    pst_Port->pst_Fram.uch_Resv[0] = 0;
    pst_Port->pst_Fram.uch_Resv[1] = 0;
    pst_Port->pst_Fram.uch_Resv[2] = 0;
    pst_Port->pst_Fram.uch_Resv[3] = 0;

    pst_Port->pst_Fram.uch_AddrLen = 0;
    pst_Port->pst_Fram.uch_AddrIndex = 0;
    pst_Port->pst_Fram.puc_AddrList = 0;

    pst_Port->pst_Fram.uch_Cmd = 0;
    pst_Port->pst_Fram.uch_SubCmd = 0;
    pst_Port->pst_Fram.uin_PayLoadLenth =  0;
    pst_Port->pst_Fram.puc_PayLoad = 0;

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
void Mod_StdbusPortSendOneByte(StdbusPort_t * pst_Port)
{
    if(pst_Port->e_State == e_StdbusSend)
    {
        INT8U uch_data = 0;

        if (pst_Port->uin_BuffIndex == 0)
        {
            if( pst_Port->pv_Handle == (void *)&COM4)
            {
                Bsp_Rs485de(eRs485Trans);
            }

            pst_Port->uin_BuffIndex = 0;
            Bsp_UartSend(pst_Port->pv_Handle,
                        &pst_Port->puc_Buff[pst_Port->uin_BuffIndex++],
                        1);
        }
        else if (pst_Port->uin_BuffIndex < pst_Port->uin_BuffLenth - 1 )
        {
            uch_data = pst_Port->puc_Buff[pst_Port->uin_BuffIndex++];
            if (pst_Port->uch_LastByte == 0x7c)
            {
                uch_data ^= 0x7c;
            }
            else if(uch_data == 0x7b || uch_data == 0x7c|| uch_data == 0x7d)
            {
                uch_data = 0x7c;
                pst_Port->uin_BuffIndex--;

            }
            pst_Port->uch_LastByte = uch_data;

            Bsp_UartSend(pst_Port->pv_Handle,
                          &uch_data,
                          1);

        }
        else if (pst_Port->uin_BuffIndex < pst_Port->uin_BuffLenth )
        {
            uch_data = pst_Port->puc_Buff[pst_Port->uin_BuffIndex++];
            Bsp_UartSend(pst_Port->pv_Handle,
                          &uch_data,
                          1);
        }
        else
        {
            if( pst_Port->pv_Handle == (void *)&COM4)
                Bsp_Rs485de(eRs485Recv);
            pst_Port->e_State = e_StdbusSended;
            PostMsg(pst_Port);                          //���� ������� ��Ϣ
        }
    }
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
BOOL Mod_StdbusPortRecvOneByte(StdbusPort_t* pst_Port,INT8U uch_Byte)
{
    if(pst_Port == NULL || pst_Port->pv_Handle == NULL || pst_Port->puc_Buff == NULL)
    {
        return FALSE;
    }

    if (pst_Port->e_State == e_StdbusIdle)
    {
        if (uch_Byte == 0x7b)
        {
            Mod_StdbusRscPack(pst_Port);                  //�ͷű��˿ڵ�����
            pst_Port->e_State = e_StdbusRecv;
            pst_Port->puc_Buff[pst_Port->uin_BuffLenth++] = uch_Byte;
            //LockPort(pst_Port);                         /*ռ������ */
        }
    }
    else if (pst_Port->e_State == e_StdbusRecv)
    {
        if(pst_Port->uin_BuffLenth < pst_Port->uin_BuffSize)    //20181203 <= ��ΪС��
        {
            if (uch_Byte == 0x7d)                               //�ж��Ƿ���ܵ�֡β
            {
                pst_Port->e_State = e_StdbusRecved;
                pst_Port->puc_Buff[pst_Port->uin_BuffLenth++] = uch_Byte;
                STDBUS_DBG("3\n");
                PostMsg(pst_Port);
            }
            else if (uch_Byte == 0x7b)                          //�ٴν��յ�֡ͷ
            {
                Mod_StdbusRscPack(pst_Port);                  //�ͷű��˿ڵ�����
                pst_Port->e_State = e_StdbusRecv;
                pst_Port->puc_Buff[pst_Port->uin_BuffLenth++] = uch_Byte;
                STDBUS_DBG("r\n");
            }
            else                                                //�������
            {
                if(pst_Port->uch_LastByte == 0x7c)
                    pst_Port->puc_Buff[pst_Port->uin_BuffLenth-1] ^= uch_Byte;
                else
                    pst_Port->puc_Buff[pst_Port->uin_BuffLenth++] = uch_Byte;
                pst_Port->uch_LastByte =uch_Byte;
            }
        }
        else                                                //20181203 ��ӳ����޸���������
        {
            STDBUS_DBG("r\n");
            Mod_StdbusRscPack(pst_Port);                  //�ͷű��˿ڵ�����
            UnLockPort(pst_Port);
        }
    }
    return TRUE;
}

//==================================================================================
//| �������� | Mod_StdbusFindDev
//|----------|----------------------------------------------------------------------
//| �������� | ���ݵ�ַ�����豸
//|----------|----------------------------------------------------------------------
//| ������� | ��
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
StdbusDev_t* Mod_StdbusFindDev(StdbusPort_t * pst_Port,INT8U uch_FindAddr)
{
    INT8U i = 0;
    if(pst_Port == NULL || pst_Port->ppst_DevList == NULL ||  pst_Port->uch_DevUse == 0)
    {
        return NULL;
    }

    for ( i = 0; i < pst_Port->uch_DevUse; i++)
    {
        StdbusDev_t* pst_Dev = pst_Port->ppst_DevList[i];
        if(pst_Dev == NULL || pst_Dev->puc_AddrList == NULL || pst_Dev->uch_AddrLen == 0)
            continue;

        if (pst_Dev->puc_AddrList[pst_Dev->uch_AddrLen-1] == uch_FindAddr)
        {
            return pst_Dev;
        }
    }
    return NULL;
}

//==================================================================================
//| �������� | Mod_StdbusFindDev
//|----------|----------------------------------------------------------------------
//| �������� | ���ݵ�ַ������Ӧ�Ķ˿�
//|----------|----------------------------------------------------------------------
//| ������� | ��
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
StdbusPort_t* Mod_StdbusFindPort(StdbusHost_t * pst_Host,INT8U uch_FindAddr)
{
    INT8U i = 0;
    if(pst_Host == NULL || pst_Host->ppst_PortList == NULL ||  pst_Host->uch_UsePort == 0)
    {
        return NULL;
    }

    for ( i = 0; i < pst_Host->uch_UsePort; i++)
    {
        StdbusDev_t* pst_Dev = Mod_StdbusFindDev(pst_Host->ppst_PortList[i],uch_FindAddr);
        if(pst_Dev != NULL)
            return pst_Host->ppst_PortList[i];
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
BOOL Mod_StdbusDealFram(StdbusPort_t * pst_Port)
{
    if(pst_Port->pst_Fram.uch_SubCmd == e_StdbusReadCmd ||
       pst_Port->pst_Fram.uch_SubCmd == e_StdbusWriteCmd)
    {
        /* �����豸���ʱ��豸 ֱ������������ַ �������˿ں���ûص�  */
        return App_StdbusMasterDealFram(&pst_Port->pst_Fram);        //���ûص���������֡
    }
    else if(pst_Port->pst_Fram.uch_SubCmd == e_StdbusReadAck ||
            pst_Port->pst_Fram.uch_SubCmd == e_StdbusWriteAck)
    {
        /* ���豸���������豸����ܵ���Ӧ�� ������Ӧ���豸
           ����֡��ַ�б��һ����ַ Դ��ַ
           �豸��ַ�б�����һ����ַ�ǲ����Ǹ��豸�ĵ�ַ */
        StdbusDev_t* pst_Dev = Mod_StdbusFindDev(pst_Port,pst_Port->pst_Fram.puc_AddrList[0]);
        if(pst_Dev == NULL)
            return FALSE;
        if(pst_Dev->cb_DealFram != NULL)
            return pst_Dev->cb_DealFram(&pst_Port->pst_Fram);        //���ûص���������֡
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
void Mod_StdbusMakePack(StdbusPort_t* pst_Port)
{
    /* �ı�puc_AddrList�ĳ��ȿ��ܳ������ݸ�����ע�⣡��*/
    uint16_t i = 0,j = 0 ,crc16 = 0;
    STDBUS_DBG(">>STDBUS DBG:   �����ʼ\r\n");
    pst_Port->puc_Buff[i++] = 0x7b;

    pst_Port->puc_Buff[i++] = pst_Port->pst_Fram.uch_Resv[0];
    pst_Port->puc_Buff[i++] = pst_Port->pst_Fram.uch_Resv[1];
    pst_Port->puc_Buff[i++] = pst_Port->pst_Fram.uch_Resv[2];
    pst_Port->puc_Buff[i++] = pst_Port->pst_Fram.uch_Resv[3];

    pst_Port->puc_Buff[i++] = pst_Port->pst_Fram.uch_AddrLen;
    pst_Port->puc_Buff[i++] = pst_Port->pst_Fram.uch_AddrIndex;

    if(pst_Port->pst_Fram.uch_AddrLen != 0 && pst_Port->pst_Fram.puc_AddrList != NULL)
    {
        for( j = 0; j < pst_Port->pst_Fram.uch_AddrLen; j++)
            pst_Port->puc_Buff[i++] = pst_Port->pst_Fram.puc_AddrList[j];
    }

    pst_Port->puc_Buff[i++] = pst_Port->pst_Fram.uch_Cmd;
    pst_Port->puc_Buff[i++] = pst_Port->pst_Fram.uch_SubCmd;
    pst_Port->puc_Buff[i++] = (uint8_t)(pst_Port->pst_Fram.uin_PayLoadLenth>>8);
    pst_Port->puc_Buff[i++] = (uint8_t)(pst_Port->pst_Fram.uin_PayLoadLenth&0xff);

    
    if(pst_Port->pst_Fram.uin_PayLoadLenth != 0 && pst_Port->pst_Fram.puc_PayLoad != NULL)
    {
        for( j = 0; j < pst_Port->pst_Fram.uin_PayLoadLenth; j++)
            pst_Port->puc_Buff[i++] = pst_Port->pst_Fram.puc_PayLoad[j];
    }

    GetCrc16Bit(pst_Port->puc_Buff + 1,i-1, &crc16);
    pst_Port->puc_Buff[i++] = (uint8_t)(crc16 >> 8);
    pst_Port->puc_Buff[i++] = (uint8_t)(crc16 );

    pst_Port->puc_Buff[i++] = 0x7d;
    pst_Port->uin_BuffLenth = i;

    pst_Port->e_State = e_StdbusSend;
    pst_Port->uin_BuffIndex = 0;
    STDBUS_DBG(">>STDBUS DBG:   ������\r\n");
    Mod_StdbusPortSendOneByte(pst_Port);
}


//==================================================================================
//| �������� | Mod_StdbusSend_Other
//|----------|----------------------------------------------------------------------
//| �������� | ����һ������
//|----------|----------------------------------------------------------------------
//| ������� | stdbusPort_tev_t ���ݽṹ��
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
BOOL Mod_StdbusSend_Other(StdbusPort_t * pst_Port)
{
    StdbusPort_t * pst_OtherPort;
    INT8U          uch_NextDevAddr = 0;
    uch_NextDevAddr = pst_Port->pst_Fram.puc_AddrList[++pst_Port->pst_Fram.uch_AddrIndex];  /*��ȡ��һ���豸�ĵ�ַ ������ַλ��*/

    pst_OtherPort = Mod_StdbusFindPort(pst_Port->pv_HostHandle,uch_NextDevAddr);
    if(pst_OtherPort == NULL)
        return FALSE;

    if( LockPort(pst_OtherPort) == FALSE)                                    /* ����˿ڱ�ռ�þ��˳� ʹ��OS�����һֱ���� */
        return FALSE;

    STDBUS_DBG(">>STDBUS DBG:   ͨ��%sת��\r\n",pst_OtherPort->pch_Name);
    
    pst_OtherPort->pst_Fram.uch_Resv[0]      = pst_Port->pst_Fram.uch_Resv[0];      /*����*/
    pst_OtherPort->pst_Fram.uch_Resv[1]      = pst_Port->pst_Fram.uch_Resv[1];
    pst_OtherPort->pst_Fram.uch_Resv[2]      = pst_Port->pst_Fram.uch_Resv[2];
    pst_OtherPort->pst_Fram.uch_Resv[3]      = pst_Port->pst_Fram.uch_Resv[3];

    pst_OtherPort->pst_Fram.puc_AddrList     = pst_Port->pst_Fram.puc_AddrList;      /*��ַ�б�*/
    pst_OtherPort->pst_Fram.uch_AddrLen      = pst_Port->pst_Fram.uch_AddrLen;       /*��ַ�б���*/
    pst_OtherPort->pst_Fram.uch_AddrIndex    = pst_Port->pst_Fram.uch_AddrIndex;     /*��ǰλ��*/

    pst_OtherPort->pst_Fram.uch_Cmd          = pst_Port->pst_Fram.uch_Cmd ;          /*��������*/
    pst_OtherPort->pst_Fram.uch_SubCmd       = pst_Port->pst_Fram.uch_SubCmd;        /*�������� 0x55 0x66 0xaa 0x99*/

    pst_OtherPort->pst_Fram.uin_PayLoadLenth = pst_Port->pst_Fram.uin_PayLoadLenth;  /*�����غɳ���*/
    pst_OtherPort->pst_Fram.puc_PayLoad      = pst_Port->pst_Fram.puc_PayLoad;       /*�����غ�*/

    Mod_StdbusMakePack(pst_OtherPort);
    return TRUE;
}

//==================================================================================
//| �������� | Mod_StdbusWriteCmd
//|----------|----------------------------------------------------------------------
//| �������� | ��֡������
//|----------|----------------------------------------------------------------------
//| ������� | Stdbus���ݽṹ��
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
BOOL Mod_StdbusWriteCmd(StdbusDev_t* pst_Dev,INT8U uch_Cmd,INT8U* puc_Payload, INT16U puc_PayloadLen)
{
    StdbusPort_t* pst_Port = pst_Dev->pv_PortHandle;

    if( LockPort(pst_Port) == FALSE)                         /* ����˿ڱ�ռ�þ��˳� ʹ��OS�����һֱ���� */
        return FALSE;

    STDBUS_DBG(">>STDBUS DBG:   %s ����\r\n",pst_Port->pch_Name);
    
    pst_Port->pst_Fram.uch_Resv[0] = 0;                             /*����*/
    pst_Port->pst_Fram.uch_Resv[1] = 0;
    pst_Port->pst_Fram.uch_Resv[2] = 0;
    pst_Port->pst_Fram.uch_Resv[3] = 0;

    pst_Port->pst_Fram.puc_AddrList = pst_Dev->puc_AddrList;       /*��ַ�б�*/
    pst_Port->pst_Fram.uch_AddrLen = pst_Dev->uch_AddrLen;         /*��ַ�б���*/
    pst_Port->pst_Fram.uch_AddrIndex = 1;                          /*��ǰλ��*/

    pst_Port->pst_Fram.uch_Cmd = uch_Cmd;                          /*��������*/
    pst_Port->pst_Fram.uch_SubCmd = e_StdbusWriteCmd;              /*�������� 0x55 0x66 0xaa 0x99*/

    pst_Port->pst_Fram.uin_PayLoadLenth = puc_PayloadLen;          /*�����غɳ���*/
    pst_Port->pst_Fram.puc_PayLoad = puc_Payload;                  /*�����غ�*/

    Mod_StdbusMakePack(pst_Port);
    return TRUE;
}

//==================================================================================
//| �������� | Mod_StdbusReadCmd
//|----------|----------------------------------------------------------------------
//| �������� | ��֡������
//|----------|----------------------------------------------------------------------
//| ������� | Stdbus���ݽṹ��
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
BOOL Mod_StdbusReadCmd(StdbusDev_t* pst_Dev,INT8U uch_Cmd,INT8U* puc_Payload, INT16U puc_PayloadLen)
{
    StdbusPort_t* pst_Port = pst_Dev->pv_PortHandle;

    if( LockPort(pst_Port) == FALSE)                         /* ����˿ڱ�ռ�þ��˳� ʹ��OS�����һֱ���� */
        return FALSE;

    pst_Port->pst_Fram.uch_Resv[0] = 0;                             /*����*/
    pst_Port->pst_Fram.uch_Resv[1] = 0;
    pst_Port->pst_Fram.uch_Resv[2] = 0;
    pst_Port->pst_Fram.uch_Resv[3] = 0;

    pst_Port->pst_Fram.puc_AddrList = pst_Dev->puc_AddrList;       /*��ַ�б�*/
    pst_Port->pst_Fram.uch_AddrLen = pst_Dev->uch_AddrLen;         /*��ַ�б���*/
    pst_Port->pst_Fram.uch_AddrIndex = 1;                          /*��ǰλ��*/

    pst_Port->pst_Fram.uch_Cmd = uch_Cmd;                          /*��������*/
    pst_Port->pst_Fram.uch_SubCmd = e_StdbusReadCmd;               /*�������� 0x55 0x66 0xaa 0x99*/

    pst_Port->pst_Fram.uin_PayLoadLenth = puc_PayloadLen;          /*�����غɳ���*/
    pst_Port->pst_Fram.puc_PayLoad = puc_Payload;                  /*�����غ�*/

    Mod_StdbusMakePack(pst_Port);
    return TRUE;
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
void Mod_StdbusDealPack(StdbusPort_t* pst_Port)
{
    StdbusHost_t* pst_Host = pst_Port->pv_HostHandle;

    INT16U i = 1;

    pst_Port->pst_Fram.uch_Resv[0]     = pst_Port->puc_Buff[i++];
    pst_Port->pst_Fram.uch_Resv[1]     = pst_Port->puc_Buff[i++];
    pst_Port->pst_Fram.uch_Resv[2]     = pst_Port->puc_Buff[i++];
    pst_Port->pst_Fram.uch_Resv[3]     = pst_Port->puc_Buff[i++];

    pst_Port->pst_Fram.uch_AddrLen     = pst_Port->puc_Buff[i++];
    pst_Port->pst_Fram.uch_AddrIndex   = pst_Port->puc_Buff[i++];
    pst_Port->pst_Fram.puc_AddrList    = &pst_Port->puc_Buff[i];

    i += pst_Port->pst_Fram.uch_AddrLen;

    pst_Port->pst_Fram.uch_Cmd         = pst_Port->puc_Buff[i++];
    pst_Port->pst_Fram.uch_SubCmd      = pst_Port->puc_Buff[i++];
    pst_Port->pst_Fram.uin_PayLoadLenth    =  (uint16_t)(pst_Port->puc_Buff[i++]<<8);
    pst_Port->pst_Fram.uin_PayLoadLenth    +=  pst_Port->puc_Buff[i++];
    pst_Port->pst_Fram.puc_PayLoad         =  &pst_Port->puc_Buff[i];

    //�ж��ǵ�ǰ�ڵ��Ƿ���������ַ
    if(pst_Port->pst_Fram.puc_AddrList[pst_Port->pst_Fram.uch_AddrIndex] !=
       pst_Host->uch_Addr )
    {
        return;
    }

    //�ж��Ƿ�����ĩ�ڵ�
    if(pst_Port->pst_Fram.puc_AddrList[pst_Port->pst_Fram.uch_AddrLen-1] !=
       pst_Host->uch_Addr )
    {
        //ת���������˿�
        STDBUS_DBG(">>STDBUS DBG:   %s�������ڵ� ͨ�������˿�ת��\r\n",pst_Port->pch_Name);
        Mod_StdbusSend_Other(pst_Port);
        STDBUS_DBG(">>STDBUS DBG:   ת�����\r\n");
        Mod_StdbusRscPack(pst_Port);                    //�ͷű��˿ڵ�����
        UnLockPort(pst_Port);                           /* �ͷ����� */ 
        STDBUS_DBG(">>STDBUS DBG:   %s ת����� �ͷ�����\r\n",pst_Port->pch_Name);
    }
    else
    {
        //�����豸���ʱ��豸 ��App��ȥ����
        STDBUS_DBG(">>STDBUS DBG:   %s �����豸���ʱ��豸 ��App��ȥ����\r\n",pst_Port->pch_Name);
 /*     20181204 ��ʱɾ��  
        if(TRUE == Mod_StdbusDealFram(pst_Port))
        {
            int i = 0;
            //��ת��ַ�б� ԭ·����
            for(i = 0 ; i < pst_Port->pst_Fram.uch_AddrLen/2; i++)
            {
                uint8_t uch_temp = pst_Port->pst_Fram.puc_AddrList[pst_Port->pst_Fram.uch_AddrLen -1 -i];
                pst_Port->pst_Fram.puc_AddrList[pst_Port->pst_Fram.uch_AddrLen -1 -i] = pst_Port->pst_Fram.puc_AddrList[i];    //len = 4 0<>3 1<>2 // len = 5  0<>4 1<>3
                pst_Port->pst_Fram.puc_AddrList[i] = uch_temp;
            }
            pst_Port->pst_Fram.uch_AddrIndex = 1;           //�ӵ�һ����ַλ�ÿ�ʼ
            pst_Port->pst_Fram.uch_SubCmd ^= 0xff;          //ȡ��������
            Mod_StdbusMakePack(pst_Port);
            STDBUS_DBG(">>STDBUS DBG:   ������ɷ���Ӧ��\r\n");
        }
        else*/
        {
            //����Ҫ�ظ�
            STDBUS_DBG(">>STDBUS DBG:   ������ɲ���ҪӦ���ͷ�����\r\n");
            Mod_StdbusRscPack(pst_Port);                    //�ͷű��˿ڵ�����
            UnLockPort(pst_Port);                           /* �ͷ����� */
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
void Mod_StdbusPortPoll(StdbusPort_t * pst_Port)
{
    INT16U  uin_crc16 = 0xFFFF;
    StdbusState_e pv_Msg = (StdbusState_e)PendMsg(pst_Port);
    switch (pv_Msg)
    {
        case e_StdbusRecved:
            STDBUS_DBG(">>STDBUS DBG:   %s �������\r\n",pst_Port->pch_Name);
            /*
            if(GetCrc16Bit(pst_Port->puc_Buff + 1,pst_Port->uin_BuffLenth - 2,&uin_crc16) != TRUE )
            {
                STDBUS_DBG(">>STDBUS DBG:   CRCУ�鲻ͨ���ͷ�����\r\n");
                Mod_StdbusRscPack(pst_Port);                  //�ͷű��˿ڵ�����
                UnLockPort(pst_Port);                         // �ͷ����� 
            }
  
            if (uin_crc16 == 0)
            {
                STDBUS_DBG(">>STDBUS DBG:   CRCУ��ͨ��\r\n");
                //CRCУ��ͨ��
                Mod_StdbusDealPack(pst_Port);
            }
            else*/
            {
                STDBUS_DBG(">>STDBUS DBG:   CRCУ�鲻ͨ���ͷ�����\r\n");
                Mod_StdbusRscPack(pst_Port);                  //�ͷű��˿ڵ�����
                //UnLockPort(pst_Port);                         /* �ͷ����� */
            }

            break;
        case e_StdbusSended:
            Mod_StdbusRscPack(pst_Port);                  // �ͷű��˿ڵ�����
            UnLockPort(pst_Port);                         // �ͷ����� 
            STDBUS_DBG(">>STDBUS DBG:   %s �������\r\n",pst_Port->pch_Name);
            break;
        default:
            break;
    }
}

__weak BOOL App_StdbusMasterDealFram(StdbusFram_t* pst_Fram)  //������
{
    return FALSE;
}
