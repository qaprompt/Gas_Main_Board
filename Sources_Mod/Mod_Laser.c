#include "Mod_Include.h"

BOOL Mod_MeasLaserDealFram(StdbusFram_t* pst_Fram);

StdbusDev_t st_LaserBoard = {
    "�����",                               /* �豸���� */
    {0,0x20},                               /*��ַ�б�*/
    2,                                      /*��ַ�б���*/
    NULL,                                   /*�˿ھ��*/
    Mod_MeasLaserDealFram,                  /*������*/
};

LaserBoard_t st_Laser = {
    0,                                      /* ״̬ */
    0,                                      /* ���Լ��� */
    0,                                      /* ͸���� */
    0,                                      /* CO2��ֵ */
    0,                                      /* CO2Ũ�� */
    0,                                      /* CO��ֵ */
    0,                                      /* COŨ�� */
    &st_LaserBoard,
};


//==================================================================================
//| �������� | Mod_LaserPoll
//|----------|----------------------------------------------------------------------
//| �������� | �������Ҫ�������� ��ͣ���Ͳ�ѯŨ�ȵ�����
//|----------|----------------------------------------------------------------------
//| ������� | ��Ϣ�ľ��
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
void Mod_LaserPoll(void)
{
    OS_ERR  os_err;
  
    OSTimeDlyHMSM(0u, 0u, 0u, 200,
                OS_OPT_TIME_HMSM_STRICT | OS_OPT_TIME_PERIODIC,/* ����ģʽ */
                &os_err);
    Mod_StdbusReadCmd(&st_LaserBoard,0x80,NULL,0);
}

BOOL Mod_MeasLaserDealFram(StdbusFram_t* pst_Fram)
{
    if (pst_Fram == NULL)
        return FALSE;
    switch(pst_Fram->uch_Cmd)
    {
    case 0x80:
        if(pst_Fram->uch_SubCmd == e_StdbusReadAck)
        {
            //������
            // 0-4���Լ��� 4-8 ͸���� 8-16 CO2Ũ�� 9-13 CO1Ũ��
            if(pst_Fram->uin_PayLoadLenth == 25)
            {
                st_Laser.uch_State = pst_Fram->puc_PayLoad[0];
                st_Laser.ul_Count = Bsp_CnvArrToINT32U(&pst_Fram->puc_PayLoad[1],FALSE);
                st_Laser.f_Trans  = Bsp_CnvArrToFP32(&pst_Fram->puc_PayLoad[5],FALSE);
                st_Laser.lf_PeakCO2             = (FP64)Bsp_CnvArrToFP32(&pst_Fram->puc_PayLoad[9],FALSE);
                st_Laser.lf_ConcentrationCO2    = (FP64)Bsp_CnvArrToFP32(&pst_Fram->puc_PayLoad[13],FALSE);
                st_Laser.lf_PeakCO              = (FP64)Bsp_CnvArrToFP32(&pst_Fram->puc_PayLoad[17],FALSE);
                st_Laser.lf_ConcentrationCO     = (FP64)Bsp_CnvArrToFP32(&pst_Fram->puc_PayLoad[21],FALSE);
            }
  
        }
        else if(pst_Fram->uch_SubCmd == e_StdbusWriteAck)
        {

        }
        break;
    case 0xA2:
        if(pst_Fram->uch_SubCmd == e_StdbusReadAck)
        {
            //������
            // 0-1���Ե���� 2-x Ũ������
            INT16U  len =  Bsp_CnvArrToINT16U(&pst_Fram->puc_PayLoad[0],FALSE);
          
            if( ((pst_Fram->uin_PayLoadLenth - 2) / 4) == len)
            {
                for(int i = 0; i < len ; i++)
                    Mod_LaserCO2Notification(Bsp_CnvArrToFP32(&pst_Fram->puc_PayLoad[i*4+2],FALSE));
                Mod_LaserReply(&st_Laser);
            }
        }
        else if(pst_Fram->uch_SubCmd == e_StdbusWriteAck)
        {

        }
        break;
    case 0xA3:
        if(pst_Fram->uch_SubCmd == e_StdbusReadAck)
        {
            //������
            // 0-1���Ե���� 2-x Ũ������
            INT16U  len =  Bsp_CnvArrToINT16U(&pst_Fram->puc_PayLoad[0],FALSE);
          
            if( ((pst_Fram->uin_PayLoadLenth - 2) / 4) == len)
            {
                for(int i = 0; i < len ; i++)
                    Mod_LaserCONotification(Bsp_CnvArrToFP32(&pst_Fram->puc_PayLoad[i*4+2],FALSE));
                Mod_LaserReply(&st_Laser);
            }
            

        }
        else if(pst_Fram->uch_SubCmd == e_StdbusWriteAck)
        {

        }
        break;     
    }
    return FALSE;
}


BOOL Mod_LaserRequestCO2Reaule(LaserBoard_t* pst_Laser)
{
    if (pst_Laser == NULL)
        return FALSE;
    Mod_StdbusReadCmd(&st_LaserBoard,0xA2,NULL,0);
    return TRUE;
}

BOOL Mod_LaserRequestCOReaule(LaserBoard_t* pst_Laser)
{
    if (pst_Laser == NULL)
        return FALSE;
    Mod_StdbusReadCmd(&st_LaserBoard,0xA3,NULL,0);
    return TRUE;
}

__weak void Mod_LaserReply(LaserBoard_t* pst_Laser)
{

}

__weak void Mod_LaserCO2Notification(FP32 f_Concentration)
{

}

__weak void Mod_LaserCONotification(FP32 f_Concentration)
{
  
}


