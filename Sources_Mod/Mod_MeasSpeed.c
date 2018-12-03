#include "Mod_Include.h"

BOOL Mod_MeasSpeedDealFram(StdbusFram_t* pst_Fram);

StdbusDev_t st_StdbusMeasSpeed = {
    {0,0x30},                               /*��ַ�б�*/
    2,                                      /*��ַ�б���*/
    NULL,                                   /*�˿ھ��*/
    Mod_MeasSpeedDealFram,                  /*������*/
};

MeasSpeed_t st_MeasSpeed = {
    0,                          /* ����ֵ */
    0,                          /* �������� */
    0,                          /* ��/Сʱ �൱�� ǧ��/Сʱ �Ŵ�1000�� */
    0,                          /* ��/��^2 */
    &st_StdbusMeasSpeed,
};

void Mod_MeasSpeedPoll(void)
{
    Mod_StdbusWriteCmd(&st_StdbusMeasSpeed,0x30,NULL,0);
}

BOOL Mod_MeasSpeedDealFram(StdbusFram_t* pst_Fram)
{
    if (pst_Fram == NULL)
        return FALSE;
    switch(pst_Fram->uch_Cmd)
    {
    case 0x40:
        if(pst_Fram->uch_SubCmd == e_StdbusReadAck)
        {
            //������
            // 0-4���Լ��� 5 ���ٷ��� 5-8 �����ٶ� 9-13 �������ٶ�

            st_MeasSpeed.ul_Count = Bsp_CnvArrToINT32U(&pst_Fram->puc_PayLoad[0],FALSE);
            st_MeasSpeed.uch_Dirction = pst_Fram->puc_PayLoad[4];
            st_MeasSpeed.f_Speed_mph = Bsp_CnvArrToFP32(&pst_Fram->puc_PayLoad[5],FALSE);
            st_MeasSpeed.f_Acc_mps2 = Bsp_CnvArrToFP32(&pst_Fram->puc_PayLoad[9],FALSE);

        }
        else if(pst_Fram->uch_SubCmd == e_StdbusWriteAck)
        {

        }

        break;
    }
    return FALSE;
}
