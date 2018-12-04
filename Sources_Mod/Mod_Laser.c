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
    0,                            /* ���Լ��� */
    0.0,                          /* ͸���� */
    0.0,                          /* CO2Ũ�� */
    0.0,                          /* COŨ�� */
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
    Mod_StdbusWriteCmd(&st_LaserBoard,0x80,NULL,0);
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
            if(pst_Fram->uin_PayLoadLenth == 24)
            {
                st_Laser.ul_Count = Bsp_CnvArrToINT32U(&pst_Fram->puc_PayLoad[0],FALSE);
                st_Laser.f_Trans  = Bsp_CnvArrToFP32(&pst_Fram->puc_PayLoad[4],FALSE);
                st_Laser.lf_ConcentrationCO2  = Bsp_CnvArrToFP64(&pst_Fram->puc_PayLoad[8],FALSE);
                st_Laser.lf_ConcentrationCO  = Bsp_CnvArrToFP64(&pst_Fram->puc_PayLoad[16],FALSE);
            }
        }
        else if(pst_Fram->uch_SubCmd == e_StdbusWriteAck)
        {

        }

        break;
    }
    return FALSE;
}
