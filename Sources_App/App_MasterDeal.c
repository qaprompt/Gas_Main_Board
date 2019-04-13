#include "App_Include.h"

static FP32 f_Buff[3840] = {0};//��ʱ

BOOL App_StdbusMasterDealFram(StdbusFram_t* pst_Fram)
{
    BOOL res = FALSE;
    //OS_ERR os_err;

    switch(pst_Fram->uch_Cmd)
    {
//==================================================================================
//                                ��ȡ������״̬ ����orδ����
//==================================================================================
    case 0x10:
        if(pst_Fram->uch_SubCmd == e_StdbusWriteCmd)
        {
        }
        else if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            //������
            pst_Fram->uin_PayLoadLenth = 1;
            //pst_Fram->puc_PayLoad[0] = USB4000.b_IsConnect;
            res = TRUE;    //Ӧ��
        }
        break;

//==================================================================================
//                                ���ù����ǻ���ʱ��
//==================================================================================
    case 0x11:
        if(pst_Fram->uch_SubCmd == e_StdbusWriteCmd)
        {
            if(pst_Fram->uin_PayLoadLenth == 4)
            {
                INT32U  i;

                TRACE_DBG(">>DBG>>      ���ù����ǻ���ʱ��\n\r");

                i = Bsp_CnvArrToINT32U(&pst_Fram->puc_PayLoad[0],FALSE);

                if( i > 65000000)
                    i = 65000000;

                USB4000_SetIntegTime(&USB4000,i);
                SaveToEeprom((INT32U)&USB4000.ul_SetIntegralTime);
                res = TRUE;    //Ӧ��
            }

        }
        else if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            //�������Ƿ����Ƿ��ڵ���
            pst_Fram->uin_PayLoadLenth = 4;

            Bsp_CnvINT32UToArr(&pst_Fram->puc_PayLoad[0],USB4000.ul_SetIntegralTime,FALSE);
            res = TRUE;    //Ӧ��
        }
        break;
//==================================================================================
//                              ���ù��������ƽ������
//==================================================================================
    case 0x12:
        if(pst_Fram->uch_SubCmd == e_StdbusWriteCmd)
        {
            if(pst_Fram->uin_PayLoadLenth == 1)
            {
                USB4000.uch_ScansToAverage = pst_Fram->puc_PayLoad[0];
                SaveToEeprom((INT32U)&USB4000.uch_ScansToAverage);
                res = TRUE;    //Ӧ��
            }
        }
        else if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            //�������Ƿ����Ƿ��ڵ���
            pst_Fram->uin_PayLoadLenth = 1;
            pst_Fram->puc_PayLoad[0] = USB4000.uch_ScansToAverage;
            res = TRUE;    //Ӧ��
        }
        break;
//==================================================================================
//                              ���ù����ǻ����˲�����
//==================================================================================
    case 0x13:
        if(pst_Fram->uch_SubCmd == e_StdbusWriteCmd)
        {
            if(pst_Fram->uin_PayLoadLenth == 1)
            {
                USB4000.uch_Boxcar = pst_Fram->puc_PayLoad[0];
                SaveToEeprom((INT32U)&USB4000.uch_Boxcar);
                res = TRUE;    //Ӧ��
            }
        }
        else if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            //�������Ƿ����Ƿ��ڵ���
            pst_Fram->uin_PayLoadLenth = 1;
            pst_Fram->puc_PayLoad[0] = USB4000.uch_Boxcar;
            res = TRUE;    //Ӧ��
        }
        break;
//==================================================================================
//                        ���ù������Ƿ���EDC(����������)
//==================================================================================
    case 0x14:
        if(pst_Fram->uch_SubCmd == e_StdbusWriteCmd)
        {
            if(pst_Fram->uin_PayLoadLenth == 1)
            {
                USB4000.b_EdcEnable = pst_Fram->puc_PayLoad[0];
                SaveToEeprom((INT32U)&USB4000.b_EdcEnable);
                res = TRUE;    //Ӧ��
            }
        }
        else if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            //�������Ƿ����Ƿ��ڵ���
            pst_Fram->uin_PayLoadLenth = 1;
            pst_Fram->puc_PayLoad[0] = USB4000.b_EdcEnable;
            res = TRUE;    //Ӧ��
        }
        break;
//==================================================================================
//                       ���ù������Ƿ���NLC(�����Բ���)
//==================================================================================
    case 0x15:
        if(pst_Fram->uch_SubCmd == e_StdbusWriteCmd)
        {
            if(pst_Fram->uin_PayLoadLenth == 1)
            {
                USB4000.b_NlcEnable = pst_Fram->puc_PayLoad[0];
                SaveToEeprom((INT32U)&USB4000.b_NlcEnable);
                res = TRUE;    //Ӧ��
            }
        }
        else if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            //�������Ƿ����Ƿ��ڵ���
            pst_Fram->uin_PayLoadLenth = 1;
            pst_Fram->puc_PayLoad[0] = USB4000.b_NlcEnable;
            res = TRUE;    //Ӧ��
        }
        break;

//==================================================================================
//                            �л���̬��̬����ģʽ
//==================================================================================
	case 0x18:
		if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
		{

		}
		else if(pst_Fram->uch_SubCmd == e_StdbusWriteCmd)
        {
            if(pst_Fram->uin_PayLoadLenth == 1)
            {
				if(pst_Fram->puc_PayLoad[0] == 0)
				{
					Mod_MeasureDoStaticMeasure(&st_Measure);
				}
				else
				{
					Mod_MeasureDoDynamicMeasure(&st_Measure);
				}
				res = 1;    //Ӧ��
            }
        }
		break;
//==================================================================================
//                          �޸�һ���궨��/��ȡһ���궨��
//==================================================================================
    case 0x22:
        if(pst_Fram->uch_SubCmd == e_StdbusWriteCmd)
        {
            //д�������޸�һ���궨��   byte0 byte0 ������ byte1�������� byte2-byte9 double ����
            if(pst_Fram->uin_PayLoadLenth == 11)
            {
                CalibPoint_t point;
                point.b_Use = pst_Fram->puc_PayLoad[2];
                point.f_X = Bsp_CnvArrToFP32(&pst_Fram->puc_PayLoad[3],FALSE);
                point.f_Y = Bsp_CnvArrToFP32(&pst_Fram->puc_PayLoad[7],FALSE);
                if( pst_Fram->puc_PayLoad[0] == 0 )
                    Mod_CalibPointListEditOnePoint(&st_CPList_GasNO,pst_Fram->puc_PayLoad[1],&point);
                else if( pst_Fram->puc_PayLoad[0] == 1 )
                    Mod_CalibPointListEditOnePoint(&st_CPList_GasHC,pst_Fram->puc_PayLoad[1],&point);
                    
                res = TRUE;    //Ӧ��
            }
        }
        else if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            //�������Ƕ�ȡһ���궨��
            //������        ���ر궨������
            //����һ������  ����ָ�������ı궨�������
            if(pst_Fram->uin_PayLoadLenth == 1)
            {
                //��ȡ��һҳ�������鳤��
                //if( pst_Fram->puc_PayLoad[0] == 0 )
                //{
                    pst_Fram->puc_PayLoad[1] = DEF_CALIBPOINT_MAX;
                    pst_Fram->uin_PayLoadLenth = 2;
                    res = TRUE;    //Ӧ��
                //}
            }
            else if(pst_Fram->uin_PayLoadLenth == 2)
            {
                CalibPoint_t point;
                if( pst_Fram->puc_PayLoad[0] == 0 )
                {   
                    Mod_CalibPointListReadOnePoint(&st_CPList_GasNO,pst_Fram->puc_PayLoad[1],&point);
                }
                else
                {
                    Mod_CalibPointListReadOnePoint(&st_CPList_GasHC,pst_Fram->puc_PayLoad[1],&point);
                }
                pst_Fram->puc_PayLoad[2] = point.b_Use;
                Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[3],point.f_X,FALSE);
                Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[7],point.f_Y,FALSE);                
                pst_Fram->uin_PayLoadLenth = 11;        
                res = TRUE;    //Ӧ��
            }
        }
        break;

//==================================================================================
//                                ���ý���/��ȡ����
//==================================================================================
    case 0x23:
        if(pst_Fram->uch_SubCmd == e_StdbusWriteCmd)
        {
            if(pst_Fram->uin_PayLoadLenth == 2)
            {
                if ( pst_Fram->puc_PayLoad[1] > 3)
                   res = FALSE;

                if( pst_Fram->puc_PayLoad[0] == 0 )
                {
                    st_GasMeasure.pst_Gas1->uch_NiheOrder = pst_Fram->puc_PayLoad[1];
                    SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas1->uch_NiheOrder));
                    pst_Fram->uin_PayLoadLenth = 2;
                    res = TRUE;    //Ӧ��
                }
                else
                {
                    st_GasMeasure.pst_Gas2->uch_NiheOrder = pst_Fram->puc_PayLoad[1];
                    SaveToEeprom((INT32U)(&st_GasMeasure.pst_Gas2->uch_NiheOrder));
                    pst_Fram->uin_PayLoadLenth = 2;
                    res = TRUE;    //Ӧ��
                }
            }
        }
        else if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            if(pst_Fram->uin_PayLoadLenth == 1)
            {
                if( pst_Fram->puc_PayLoad[0] == 0 )
                {
                    pst_Fram->puc_PayLoad[1] = st_GasMeasure.pst_Gas1->uch_NiheOrder;
                    pst_Fram->uin_PayLoadLenth = 2;
                    res = TRUE;    //Ӧ��
                }
                else
                {
                    pst_Fram->puc_PayLoad[1] = st_GasMeasure.pst_Gas2->uch_NiheOrder;
                    pst_Fram->uin_PayLoadLenth = 2;
                    res = TRUE;    //Ӧ��
                }
            }
        }

        break;

//==================================================================================
//                                  ������������
//==================================================================================
    case 0x24:
        if(pst_Fram->uch_SubCmd == e_StdbusWriteCmd)
        {
            //д�����ǽ�����������
            if(pst_Fram->uin_PayLoadLenth == 1)
            {
                Mod_GasMarkWorkLine(&st_GasMeasure,pst_Fram->puc_PayLoad[0]);
                res = TRUE;            //Ӧ�� ���޸�����ԭʼ���ݷ���
            }
        }
        break;
//==================================================================================
//                          ���ù�������ϵ��/��ȡ��������ϵ��
//==================================================================================
    case 0x25:
        if(pst_Fram->uch_SubCmd == e_StdbusWriteCmd)
        {
            //д�����ǽ�����������
            if(pst_Fram->uin_PayLoadLenth == 0)
            {

            }
        }
        else if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            //�������Ƕ�ȡ���ϵ��
            //������������ ��һ������ѡ���������ϵ�� �ڶ���������ѡ��An
            if(pst_Fram->uin_PayLoadLenth == 1)
            {
                if(pst_Fram->puc_PayLoad[0] == 0)
                {
                    Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[1],st_GasMeasure.pst_Gas1->af_NiheCoeff[0],FALSE);                
                    Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[5],st_GasMeasure.pst_Gas1->af_NiheCoeff[1],FALSE);   
                    Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[9],st_GasMeasure.pst_Gas1->af_NiheCoeff[2],FALSE);   
                    Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[13],st_GasMeasure.pst_Gas1->af_NiheCoeff[3],FALSE);                
                    Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[17],st_GasMeasure.pst_Gas1->af_NiheCoeff[4],FALSE);   
                    Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[21],st_GasMeasure.pst_Gas1->af_NiheCoeff[5],FALSE);   
                    pst_Fram->uin_PayLoadLenth = 25;
                    res = TRUE;    //Ӧ��
                }
            }
        }
        break;
//==================================================================================
//                              ����/��ȡ͸����ϵ��
//==================================================================================
    case 0x26:
        if(pst_Fram->uch_SubCmd == e_StdbusWriteCmd)
        {
            if(pst_Fram->uin_PayLoadLenth == 4)
            {
                TRACE_DBG(">>DBG>>      ����͸����ϵ��\n\r");
                st_GasMeasure.f_TransK = Bsp_CnvArrToFP32(&pst_Fram->puc_PayLoad[0],FALSE);
                SaveToEeprom((INT32U)&st_GasMeasure.f_TransK);
                res = TRUE;    //Ӧ��
            }
        }
        else if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            //�������Ƿ����Ƿ��ڵ���
            pst_Fram->uin_PayLoadLenth = 4;
            Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[0], st_GasMeasure.f_TransK,FALSE);
            res = TRUE;    //Ӧ��
        }
        break;
        
//==================================================================================
//                              ����/��ȡ���շ�λ�ò���
//==================================================================================
    case 0x27:
        if(pst_Fram->uch_SubCmd == e_StdbusWriteCmd)
        {
            if(pst_Fram->uin_PayLoadLenth == 28)
            {
                ///���շ����  
                st_GasMeasure.pst_Gas1->st_PeakRef.ul_LeftBackgroundLeftDot     = Bsp_CnvArrToINT16U(&pst_Fram->puc_PayLoad[0],FALSE);
                st_GasMeasure.pst_Gas1->st_PeakRef.ul_LeftBackgroundRightDot    = Bsp_CnvArrToINT16U(&pst_Fram->puc_PayLoad[2],FALSE);
                st_GasMeasure.pst_Gas1->st_PeakRef.ul_PeakLeftDot               = Bsp_CnvArrToINT16U(&pst_Fram->puc_PayLoad[4],FALSE);
                st_GasMeasure.pst_Gas1->st_PeakRef.ul_PeakCenterDot             = Bsp_CnvArrToINT16U(&pst_Fram->puc_PayLoad[6],FALSE);
                st_GasMeasure.pst_Gas1->st_PeakRef.ul_PeakRightDot              = Bsp_CnvArrToINT16U(&pst_Fram->puc_PayLoad[8],FALSE);
                st_GasMeasure.pst_Gas1->st_PeakRef.ul_RightBackgroundLeftDot    = Bsp_CnvArrToINT16U(&pst_Fram->puc_PayLoad[10],FALSE);        
                st_GasMeasure.pst_Gas1->st_PeakRef.ul_RightBackgroundRightDot   = Bsp_CnvArrToINT16U(&pst_Fram->puc_PayLoad[12],FALSE);
                
                st_GasMeasure.pst_Gas2->st_PeakRef.ul_LeftBackgroundLeftDot     = Bsp_CnvArrToINT16U(&pst_Fram->puc_PayLoad[14],FALSE);
                st_GasMeasure.pst_Gas2->st_PeakRef.ul_LeftBackgroundRightDot    = Bsp_CnvArrToINT16U(&pst_Fram->puc_PayLoad[16],FALSE);
                st_GasMeasure.pst_Gas2->st_PeakRef.ul_PeakLeftDot               = Bsp_CnvArrToINT16U(&pst_Fram->puc_PayLoad[18],FALSE);
                st_GasMeasure.pst_Gas2->st_PeakRef.ul_PeakCenterDot             = Bsp_CnvArrToINT16U(&pst_Fram->puc_PayLoad[20],FALSE);
                st_GasMeasure.pst_Gas2->st_PeakRef.ul_PeakRightDot              = Bsp_CnvArrToINT16U(&pst_Fram->puc_PayLoad[22],FALSE);
                st_GasMeasure.pst_Gas2->st_PeakRef.ul_RightBackgroundLeftDot    = Bsp_CnvArrToINT16U(&pst_Fram->puc_PayLoad[24],FALSE);        
                st_GasMeasure.pst_Gas2->st_PeakRef.ul_RightBackgroundRightDot   = Bsp_CnvArrToINT16U(&pst_Fram->puc_PayLoad[26],FALSE);
                
                ///�ۺϲ�������
				st_Measure.uin_InvalidDots = Bsp_CnvArrToINT16U(&pst_Fram->puc_PayLoad[28],FALSE);
				st_Measure.uin_ActiveDots  = Bsp_CnvArrToINT16U(&pst_Fram->puc_PayLoad[30],FALSE);
                st_Measure.ul_DeadTime     = Bsp_CnvArrToINT32U(&pst_Fram->puc_PayLoad[32],FALSE);
                st_Measure.ul_MesureTime   = Bsp_CnvArrToINT32U(&pst_Fram->puc_PayLoad[36],FALSE); 
                
                ///���շ����
                SaveToEeprom((INT32U)&st_GasMeasure.pst_Gas1->st_PeakRef.ul_LeftBackgroundLeftDot);
                SaveToEeprom((INT32U)&st_GasMeasure.pst_Gas1->st_PeakRef.ul_LeftBackgroundRightDot);
                SaveToEeprom((INT32U)&st_GasMeasure.pst_Gas1->st_PeakRef.ul_PeakLeftDot);
                SaveToEeprom((INT32U)&st_GasMeasure.pst_Gas1->st_PeakRef.ul_PeakCenterDot);
                SaveToEeprom((INT32U)&st_GasMeasure.pst_Gas1->st_PeakRef.ul_PeakRightDot);
                SaveToEeprom((INT32U)&st_GasMeasure.pst_Gas1->st_PeakRef.ul_RightBackgroundLeftDot);
                SaveToEeprom((INT32U)&st_GasMeasure.pst_Gas1->st_PeakRef.ul_RightBackgroundRightDot);
                
                SaveToEeprom((INT32U)&st_GasMeasure.pst_Gas2->st_PeakRef.ul_LeftBackgroundLeftDot);
                SaveToEeprom((INT32U)&st_GasMeasure.pst_Gas2->st_PeakRef.ul_LeftBackgroundRightDot);
                SaveToEeprom((INT32U)&st_GasMeasure.pst_Gas2->st_PeakRef.ul_PeakLeftDot);
                SaveToEeprom((INT32U)&st_GasMeasure.pst_Gas2->st_PeakRef.ul_PeakCenterDot);
                SaveToEeprom((INT32U)&st_GasMeasure.pst_Gas2->st_PeakRef.ul_PeakRightDot);
                SaveToEeprom((INT32U)&st_GasMeasure.pst_Gas2->st_PeakRef.ul_RightBackgroundLeftDot);
                SaveToEeprom((INT32U)&st_GasMeasure.pst_Gas2->st_PeakRef.ul_RightBackgroundRightDot); 
                
                ///�ۺϲ�������
                SaveToEeprom((INT32U)(&st_Measure.uin_InvalidDots));
                SaveToEeprom((INT32U)(&st_Measure.uin_ActiveDots));
                SaveToEeprom((INT32U)(&st_Measure.ul_DeadTime));
                SaveToEeprom((INT32U)(&st_Measure.ul_MesureTime));   
                
                res = TRUE;    //Ӧ��
            }
        }
        else if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            //�������Ƿ����Ƿ��ڵ���
            pst_Fram->uin_PayLoadLenth = 28;
            
            ///���շ����
            Bsp_CnvINT16UToArr(&pst_Fram->puc_PayLoad[0], st_GasMeasure.pst_Gas1->st_PeakRef.ul_LeftBackgroundLeftDot,FALSE);
            Bsp_CnvINT16UToArr(&pst_Fram->puc_PayLoad[2], st_GasMeasure.pst_Gas1->st_PeakRef.ul_LeftBackgroundRightDot,FALSE);
            Bsp_CnvINT16UToArr(&pst_Fram->puc_PayLoad[4], st_GasMeasure.pst_Gas1->st_PeakRef.ul_PeakLeftDot,FALSE);
            Bsp_CnvINT16UToArr(&pst_Fram->puc_PayLoad[6], st_GasMeasure.pst_Gas1->st_PeakRef.ul_PeakCenterDot,FALSE);         
            Bsp_CnvINT16UToArr(&pst_Fram->puc_PayLoad[8], st_GasMeasure.pst_Gas1->st_PeakRef.ul_PeakRightDot,FALSE);  
            Bsp_CnvINT16UToArr(&pst_Fram->puc_PayLoad[10],st_GasMeasure.pst_Gas1->st_PeakRef.ul_RightBackgroundLeftDot,FALSE);
            Bsp_CnvINT16UToArr(&pst_Fram->puc_PayLoad[12],st_GasMeasure.pst_Gas1->st_PeakRef.ul_RightBackgroundRightDot,FALSE);
            
            Bsp_CnvINT16UToArr(&pst_Fram->puc_PayLoad[14],st_GasMeasure.pst_Gas2->st_PeakRef.ul_LeftBackgroundLeftDot,FALSE);
            Bsp_CnvINT16UToArr(&pst_Fram->puc_PayLoad[16],st_GasMeasure.pst_Gas2->st_PeakRef.ul_LeftBackgroundRightDot,FALSE);
            Bsp_CnvINT16UToArr(&pst_Fram->puc_PayLoad[18],st_GasMeasure.pst_Gas2->st_PeakRef.ul_PeakLeftDot,FALSE);
            Bsp_CnvINT16UToArr(&pst_Fram->puc_PayLoad[20],st_GasMeasure.pst_Gas2->st_PeakRef.ul_PeakCenterDot,FALSE);         
            Bsp_CnvINT16UToArr(&pst_Fram->puc_PayLoad[22],st_GasMeasure.pst_Gas2->st_PeakRef.ul_PeakRightDot,FALSE);  
            Bsp_CnvINT16UToArr(&pst_Fram->puc_PayLoad[24],st_GasMeasure.pst_Gas2->st_PeakRef.ul_RightBackgroundLeftDot,FALSE);
            Bsp_CnvINT16UToArr(&pst_Fram->puc_PayLoad[26],st_GasMeasure.pst_Gas2->st_PeakRef.ul_RightBackgroundRightDot,FALSE);
            
            ///�ۺϲ�������
            Bsp_CnvINT16UToArr(&pst_Fram->puc_PayLoad[28],st_Measure.uin_InvalidDots,FALSE);     
            Bsp_CnvINT16UToArr(&pst_Fram->puc_PayLoad[30],st_Measure.uin_ActiveDots,FALSE);
            Bsp_CnvINT32UToArr(&pst_Fram->puc_PayLoad[32],st_Measure.ul_DeadTime,FALSE);
            Bsp_CnvINT32UToArr(&pst_Fram->puc_PayLoad[36],st_Measure.ul_MesureTime,FALSE);
            
            res = TRUE;    //Ӧ��
        }
        break;
        
//==================================================================================
//                                  ����/��ȡ����
//==================================================================================
    case 0x28:
        if(pst_Fram->uch_SubCmd == e_StdbusWriteCmd)
        {
            if(pst_Fram->uin_PayLoadLenth == 24)
            {
                ///�ۺϲ�������
				st_Measure.uin_InvalidDots = Bsp_CnvArrToINT32U(&pst_Fram->puc_PayLoad[0],FALSE);
				st_Measure.uin_ActiveDots  = Bsp_CnvArrToINT32U(&pst_Fram->puc_PayLoad[4],FALSE);
                st_Measure.ul_DeadTime     = Bsp_CnvArrToINT32U(&pst_Fram->puc_PayLoad[8],FALSE);
                st_Measure.ul_MesureTime   = Bsp_CnvArrToINT32U(&pst_Fram->puc_PayLoad[12],FALSE); 
                
                st_GasMeasure.ul_TransLeftDot  = Bsp_CnvArrToINT32U(&pst_Fram->puc_PayLoad[16],FALSE);
                st_GasMeasure.ul_TransRightDot = Bsp_CnvArrToINT32U(&pst_Fram->puc_PayLoad[20],FALSE); 
                
                ///�ۺϲ�������
                SaveToEeprom((INT32U)(&st_Measure.uin_InvalidDots));
                SaveToEeprom((INT32U)(&st_Measure.uin_ActiveDots));
                SaveToEeprom((INT32U)(&st_Measure.ul_DeadTime));
                SaveToEeprom((INT32U)(&st_Measure.ul_MesureTime));
                
                SaveToEeprom((INT32U)(&st_GasMeasure.ul_TransLeftDot));
                SaveToEeprom((INT32U)(&st_GasMeasure.ul_TransRightDot));
                res = TRUE;    //Ӧ��
            }
        }
        else if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            //�������Ƿ����Ƿ��ڵ���
            pst_Fram->uin_PayLoadLenth = 24;
            
            ///�ۺϲ�������
            Bsp_CnvINT32UToArr(&pst_Fram->puc_PayLoad[0],st_Measure.uin_InvalidDots,FALSE);     
            Bsp_CnvINT32UToArr(&pst_Fram->puc_PayLoad[4],st_Measure.uin_ActiveDots,FALSE);
            Bsp_CnvINT32UToArr(&pst_Fram->puc_PayLoad[8],st_Measure.ul_DeadTime,FALSE);
            Bsp_CnvINT32UToArr(&pst_Fram->puc_PayLoad[12],st_Measure.ul_MesureTime,FALSE);
            
            Bsp_CnvINT32UToArr(&pst_Fram->puc_PayLoad[16],st_GasMeasure.ul_TransLeftDot,FALSE);
            Bsp_CnvINT32UToArr(&pst_Fram->puc_PayLoad[20],st_GasMeasure.ul_TransRightDot,FALSE); 
            
            res = TRUE;    //Ӧ��
        }
        break;
//==================================================================================
//                                   ��ȡ10·�̹��ѹ
//==================================================================================
    case 0x30:
        if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            if(pst_Fram->uin_PayLoadLenth == 0)
            {
                uint16_t i = 0;
                pst_Fram->uin_PayLoadLenth = 40;

                for(i = 0; i < 10; i++)
                {
                    Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[i*4],st_Grey.pst_Channel[i].f_Volt,FALSE);
                }
            }
            res = TRUE;    //Ӧ��
        }
        break;
//==================================================================================
//                                   ��ȡ10·�̹��ֵ�궨��ѹ
//==================================================================================
    case 0x31:
        if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            if(pst_Fram->uin_PayLoadLenth == 0)
            {
                uint16_t i = 0;
                pst_Fram->uin_PayLoadLenth = 40;

                for(i = 0; i < 10; i++)
                {
                    Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[i*4],st_Grey.pst_Channel[i].f_AbsTransVolt,FALSE);
                }
            }
            res = TRUE;    //Ӧ��
        }
        break;
//==================================================================================
//                                   ��ȡ10·�̹ⱳ����ѹ
//==================================================================================
    case 0x32:
        if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            if(pst_Fram->uin_PayLoadLenth == 0)
            {
                uint16_t i = 0;
                pst_Fram->uin_PayLoadLenth = 40;

                for(i = 0; i < 10; i++)
                {
                    Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[i*4],st_Grey.pst_Channel[i].f_BkVolt,FALSE);
                }
            }
            res = TRUE;    //Ӧ��
        }
        break;
//==================================================================================
//                                   ��ȡ10·�̹�͸����
//==================================================================================
    case 0x33:
        if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            if(pst_Fram->uin_PayLoadLenth == 0)
            {
                uint16_t i = 0;
                pst_Fram->uin_PayLoadLenth = 40;

                for(i = 0; i < 10; i++)
                {
                    Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[i*4],st_Grey.pst_Channel[i].f_Trans,FALSE);
                }
            }
            res = TRUE;    //Ӧ��
        }
        break;
//==================================================================================
//                                   ��ȡ10·�̹�Ҷ�
//==================================================================================
    case 0x34:
        if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            if(pst_Fram->uin_PayLoadLenth == 0)
            {
                uint16_t i = 0;
                pst_Fram->uin_PayLoadLenth = 40;

                for(i = 0; i < 10; i++)
                {
                    Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[i*4],st_Grey.pst_Channel[i].f_Grey,FALSE);
                }
            }
            res = TRUE;    //Ӧ��
        }
        break;
//==================================================================================
//                                   �����̹⹤��״̬
//==================================================================================
    case 0x3a:
        if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            pst_Fram->uin_PayLoadLenth = 1;
            pst_Fram->puc_PayLoad[0] = st_Grey.e_Status;
            res = TRUE;
        }
        else if(pst_Fram->uch_SubCmd == e_StdbusWriteCmd)
        {
            if(pst_Fram->uin_PayLoadLenth == 1)
            {
                switch(pst_Fram->puc_PayLoad[0])
                {
                case 0:
                    Mod_GreyGotoIdle(&st_Grey);
                    break;
                case 1:
                    Mod_GreyGotoMeas(&st_Grey);
                    break;
                case 2:
                    Mod_GreyGotoCalib(&st_Grey);
                    break;
                default:
                    return FALSE;
                }
            }
            res = TRUE;    //Ӧ��
        }
        break;
//==================================================================================
//                         ��ȡ�̹⹤��״̬/��͸����/�ܻҶ�
//==================================================================================
    case 0x3b:
        if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            pst_Fram->uin_PayLoadLenth = 9;
            pst_Fram->puc_PayLoad[0] = st_Grey.e_Status;
            Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[1],st_Grey.f_Trans,FALSE);
            Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[5],st_Grey.f_Grey,FALSE);
            res = TRUE;
        }
        break;
//==================================================================================
//                                   ��ȡ����
//==================================================================================
    case 0x40:
        if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            if(pst_Fram->uin_PayLoadLenth == 0)
            {
                INT16U i = 0;
                INT16U len = 0;
                OS_ERR os_err;

                /* ���ع��׵� ������ ȷ�������ٴ���һ���� ���¹��� */
                OSSchedLock(&os_err);
                for(i = st_GasMeasure.ul_UseLeftDot; i < st_GasMeasure.ul_UseRightDot; i++)
                    f_Buff[len++] = st_GasMeasure.plf_Spectrum[i];
                OSSchedUnlock(&os_err);

                //��ȡ��һҳ�������鳤��
                Bsp_CnvINT16UToArr(&pst_Fram->puc_PayLoad[0], len, FALSE);
                pst_Fram->uin_PayLoadLenth = 2;
                res = TRUE;    //Ӧ��
            }
            else if(pst_Fram->uin_PayLoadLenth == 4)
            {
                //��һ�����ֽ���ReadAddress �ڶ������ֽ���ReadLenth
                INT16U i = 0;
                INT16U uin_Offset = Bsp_CnvArrToINT16U(&pst_Fram->puc_PayLoad[0], FALSE);
                INT16U uin_Lenth = Bsp_CnvArrToINT16U(&pst_Fram->puc_PayLoad[2], FALSE);

                pst_Fram->uin_PayLoadLenth = 4 + uin_Lenth * 4;
                for(i = 0; i<uin_Lenth;i++)
                {
                    Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[i*4+4],f_Buff[uin_Offset+i],FALSE);
                }
                res = TRUE;    //Ӧ��
            }
        }
        break;

//==================================================================================
//                                  ��ȡ�������
//==================================================================================
    case 0x41:
        if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            if(pst_Fram->uin_PayLoadLenth == 0)
            {
                INT16U i = 0;
                INT16U len = 0;
                OS_ERR os_err;

                /* ���ع��׵� ������ ȷ�������ٴ���һ���� ���¹��� */
                OSSchedLock(&os_err);
                for(i = st_GasMeasure.ul_UseLeftDot; i < st_GasMeasure.ul_UseRightDot; i++)
                    f_Buff[len++] = st_GasMeasure.pf_ZeroSpectrum[i];
                OSSchedUnlock(&os_err);

                //��ȡ��һҳ�������鳤��
                Bsp_CnvINT16UToArr(&pst_Fram->puc_PayLoad[0], len, FALSE);
                pst_Fram->uin_PayLoadLenth = 2;
                res = TRUE;    //Ӧ��
            }
            else if(pst_Fram->uin_PayLoadLenth == 4)
            {
                //��һ�����ֽ���ReadAddress �ڶ������ֽ���ReadLenth
                INT16U i = 0;
                INT16U uin_Offset = Bsp_CnvArrToINT16U(&pst_Fram->puc_PayLoad[0], FALSE);
                INT16U uin_Lenth = Bsp_CnvArrToINT16U(&pst_Fram->puc_PayLoad[2], FALSE);

                pst_Fram->uin_PayLoadLenth = 4 + uin_Lenth * 4;
                for(i = 0; i<uin_Lenth;i++)
                {
                    Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[i*4+4],f_Buff[uin_Offset+i],FALSE);
                }
                res = TRUE;    //Ӧ��
            }
        }
        break;

//==================================================================================
//                                  ��ȡ��������
//==================================================================================
    case 0x42:
        if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            if(pst_Fram->uin_PayLoadLenth == 0)
            {
                INT16U i = 0;
                INT16U len = 0;
                OS_ERR os_err;

                /* ���ع��׵� ������ ȷ�������ٴ���һ���� ���¹��� */
                OSSchedLock(&os_err);
                for(i = st_GasMeasure.ul_UseLeftDot; i < st_GasMeasure.ul_UseRightDot; i++)
                    f_Buff[len++] = st_GasMeasure.plf_BkgSpectrum[i];
                OSSchedUnlock(&os_err);

                //��ȡ��һҳ�������鳤��
                Bsp_CnvINT16UToArr(&pst_Fram->puc_PayLoad[0], len, FALSE);
                pst_Fram->uin_PayLoadLenth = 2;
                res = TRUE;    //Ӧ��
            }
            else if(pst_Fram->uin_PayLoadLenth == 4)
            {
                //��һ�����ֽ���ReadAddress �ڶ������ֽ���ReadLenth
                INT16U i = 0;
                INT16U uin_Offset = Bsp_CnvArrToINT16U(&pst_Fram->puc_PayLoad[0], FALSE);
                INT16U uin_Lenth = Bsp_CnvArrToINT16U(&pst_Fram->puc_PayLoad[2], FALSE);

                pst_Fram->uin_PayLoadLenth = 4 + uin_Lenth * 4;
                for(i = 0; i<uin_Lenth;i++)
                {
                    Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[i*4+4],f_Buff[uin_Offset+i],FALSE);
                }
                res = TRUE;    //Ӧ��
            }
        }
        break;
//==================================================================================
//                                  ��ȡ��ֹ���
//==================================================================================
    case 0x43:
        if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            if(pst_Fram->uin_PayLoadLenth == 0)
            {
                INT16U i = 0;
                INT16U len = 0;
                OS_ERR os_err;

                /* ���ع��׵� ������ ȷ�������ٴ���һ���� ���¹��� */
                OSSchedLock(&os_err);
                for(i = st_GasMeasure.ul_UseLeftDot; i < st_GasMeasure.ul_UseRightDot; i++)
                    f_Buff[len++] = st_GasMeasure.plf_DiffSpectrum[i];
                OSSchedUnlock(&os_err);

                //��ȡ��һҳ�������鳤��
                Bsp_CnvINT16UToArr(&pst_Fram->puc_PayLoad[0], len, FALSE);
                pst_Fram->uin_PayLoadLenth = 2;
                res = TRUE;    //Ӧ��
            }
            else if(pst_Fram->uin_PayLoadLenth == 4)
            {
                //��һ�����ֽ���ReadAddress �ڶ������ֽ���ReadLenth
                INT16U i = 0;
                INT16U uin_Offset = Bsp_CnvArrToINT16U(&pst_Fram->puc_PayLoad[0], FALSE);
                INT16U uin_Lenth = Bsp_CnvArrToINT16U(&pst_Fram->puc_PayLoad[2], FALSE);

                pst_Fram->uin_PayLoadLenth = 4 + uin_Lenth * 4;
                for(i = 0; i<uin_Lenth;i++)
                {
                    Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[i*4+4],f_Buff[uin_Offset+i],FALSE);
                }
                res = TRUE;    //Ӧ��
            }
        }
        break;
        
//==================================================================================
//                                  ��ȡ�������
//==================================================================================
    case 0x44:
        if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            if(pst_Fram->uin_PayLoadLenth == 0)
            {
                INT16U i = 0;
                INT16U len = 0;
                OS_ERR os_err;

                /* ���ع��׵� ������ ȷ�������ٴ���һ���� ���¹��� */
                OSSchedLock(&os_err);
                for(i = st_GasMeasure.ul_UseLeftDot; i < st_GasMeasure.ul_UseRightDot; i++)
                    f_Buff[len++] = st_GasMeasure.pf_ProcSpectrum[i];
                OSSchedUnlock(&os_err);

                //��ȡ��һҳ�������鳤��
                Bsp_CnvINT16UToArr(&pst_Fram->puc_PayLoad[0], len, FALSE);
                pst_Fram->uin_PayLoadLenth = 2;
                res = TRUE;    //Ӧ��
            }
            else if(pst_Fram->uin_PayLoadLenth == 4)
            {
                //��һ�����ֽ���ReadAddress �ڶ������ֽ���ReadLenth
                INT16U i = 0;
                INT16U uin_Offset = Bsp_CnvArrToINT16U(&pst_Fram->puc_PayLoad[0], FALSE);
                INT16U uin_Lenth = Bsp_CnvArrToINT16U(&pst_Fram->puc_PayLoad[2], FALSE);

                pst_Fram->uin_PayLoadLenth = 4 + uin_Lenth * 4;
                for(i = 0; i<uin_Lenth;i++)
                {
                    Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[i*4+4],f_Buff[uin_Offset+i],FALSE);
                }
                res = TRUE;    //Ӧ��
            }
        }
        break;
//==================================================================================
//                                  �л�����ģʽ
//==================================================================================
    case 0x4a:
        if(pst_Fram->uch_SubCmd == e_StdbusWriteCmd)
        {
            switch(pst_Fram->puc_PayLoad[0])
            {
            case eGasAdjZero:
                if(pst_Fram->uin_PayLoadLenth == 3)
                {
                    Mod_GasMeasureDoAdjZero(&st_GasMeasure,Bsp_CnvArrToINT16U(&pst_Fram->puc_PayLoad[1],FALSE));
                    res = TRUE;    //Ӧ��
                }
                break;
            case eGasCalibGas1:
            case eGasCalibGas2:
            case eGasCalibGasAll:
                if(pst_Fram->uin_PayLoadLenth == 11)
                {
                    Mod_GasMeasureDoCalib(&st_GasMeasure,
                                       (GasMeasureState_e)pst_Fram->puc_PayLoad[0],
                                       Bsp_CnvArrToINT16U(&pst_Fram->puc_PayLoad[1],FALSE),
                                       Bsp_CnvArrToFP32(&pst_Fram->puc_PayLoad[3],FALSE),
                                       Bsp_CnvArrToFP32(&pst_Fram->puc_PayLoad[7],FALSE));
                    res = TRUE;    //Ӧ��
                }
                break;
                
            case eGasCalibCorrectionGas1:
            case eGasCalibCorrectionGas2:
            case eGasCalibCorrectionGasAll:
                if(pst_Fram->uin_PayLoadLenth == 11)
                {
                    Mod_GasMeasureDoCalibCorrection(&st_GasMeasure,
                                       (GasMeasureState_e)pst_Fram->puc_PayLoad[0],
                                       Bsp_CnvArrToINT16U(&pst_Fram->puc_PayLoad[1],FALSE),
                                       Bsp_CnvArrToFP32(&pst_Fram->puc_PayLoad[3],FALSE),
                                       Bsp_CnvArrToFP32(&pst_Fram->puc_PayLoad[7],FALSE));
                    res = TRUE;    //Ӧ��
                }
                break;
            case eGasAbsMeasure:
                Mod_GasMeasureDoAbsMeasure(&st_GasMeasure);         //�л������Թ���״̬
                res = TRUE;    //Ӧ��
                break;
            case eGasDiffBackground:
                Mod_GasMeasureDoDiffBackground(&st_GasMeasure);     //�л�����ֱ�������״̬
                res = TRUE;
                break;
            case eGasDiffMeasure:
                Mod_GasMeasureDoDiffMeasure(&st_GasMeasure);        //�л�����ֲ�������״̬
                res = TRUE;    //Ӧ��
                break;
            default:
                break;

            }
        }
        else if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {  
            pst_Fram->uin_PayLoadLenth = 1;
            pst_Fram->puc_PayLoad[0] = st_GasMeasure.e_State;
            res = TRUE;    //Ӧ��
        }
        break;
#if 0
//==================================================================================
//                                  �л�����ģʽ
//==================================================================================
    case 0x4a:
        if(pst_Fram->uch_SubCmd == e_StdbusWriteCmd)
        {

            //byte1-5 float :�л����궨״̬��ʱ�·��ı궨Ũ��
            if(pst_Fram->uin_PayLoadLenth == 1)
            {
                switch(pst_Fram->puc_PayLoad[0])
                {
                case eGasAdjZero:
                    Mod_GasMeasureDoAdjZero(&st_GasMeasure);           //�л�����0״̬
                    res = TRUE;    //Ӧ��
                    break;
                case eGasAbsMeasure:
                    Mod_GasMeasureDoAbsMeasure(&st_GasMeasure);        //�л�������״̬
                    res = TRUE;    //Ӧ��
                    break;
                case eGasDiffMeasure:
                    Mod_GasMeasureDoDiffMeasure(&st_GasMeasure);        //�л�������״̬
                    res = TRUE;    //Ӧ��
                    break;
                case eGasWait:
                    Mod_GasMeasureDoWait(&st_GasMeasure);               //�л����ȴ�״̬
                    res = TRUE;    //Ӧ��
                    break;
                case eGasCalibTrans:
                    Mod_GasMeasureDoCalibTrans(&st_GasMeasure);         //�л���͸���ʱ궨�������궨��        
                    res = TRUE;    //Ӧ��
                    break;
                default:
                    break;
                }
            }
            else if(pst_Fram->uin_PayLoadLenth == 17)
            {
                FP64 f1,f2;

                TRACE_DBG(">>DBG>>      ���յ��궨����\n\r");

                f1 = Bsp_CnvArrToFP64(&pst_Fram->puc_PayLoad[1],FALSE);
                f2 = Bsp_CnvArrToFP64(&pst_Fram->puc_PayLoad[9],FALSE);
                
                if(pst_Fram->puc_PayLoad[0] == eGasCalibGas1 || pst_Fram->puc_PayLoad[0] == eGasCalibGas2 ||
                   pst_Fram->puc_PayLoad[0] == eGasCalibAll)
                {
                     Mod_GasMeasureGotoCalib(&st_GasMeasure,
                                            ((GasMeasureState_e)pst_Fram->puc_PayLoad[0]),
                                            f1,f2);
                     res = TRUE;    //Ӧ��
                }

                if(pst_Fram->puc_PayLoad[0] == eGasCalibCorrectionGas1 || pst_Fram->puc_PayLoad[0] == eGasCalibCorrectionGas2 ||
                   pst_Fram->puc_PayLoad[0] == eGasCalibCorrectionGasAll)
                {
                     Mod_GasMeasureGotoCalibCorrection(&st_GasMeasure,
                                            ((GasMeasureState_e)pst_Fram->puc_PayLoad[0]),
                                            f1,f2);
                     res = TRUE;    //Ӧ��
                }

            }

        }
        else if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            //�������Ƿ����Ƿ��ڵ���
            pst_Fram->uin_PayLoadLenth = 1;
            pst_Fram->puc_PayLoad[0] = st_GasMeasure.e_State;
            res = TRUE;    //Ӧ��
        }
        break;  
#endif
//==================================================================================
//                                  ��ȡ����״̬
//==================================================================================
    case 0x4b:
        if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            pst_Fram->uin_PayLoadLenth = 37;
            pst_Fram->puc_PayLoad[0] = st_GasMeasure.e_State;
            Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[1],st_GasMeasure.f_Trans,FALSE);
            if(st_GasMeasure.pst_Gas1 != NULL)
            {
                Bsp_CnvFP64ToArr(&pst_Fram->puc_PayLoad[5],st_GasMeasure.pst_Gas1->lf_PeakHight,FALSE);
                Bsp_CnvFP64ToArr(&pst_Fram->puc_PayLoad[13],st_GasMeasure.pst_Gas1->lf_Concentration,FALSE);
            }
            else
            {
                Bsp_CnvFP64ToArr(&pst_Fram->puc_PayLoad[5],0.0,FALSE);
                Bsp_CnvFP64ToArr(&pst_Fram->puc_PayLoad[13],0.0,FALSE);
            }
            if(st_GasMeasure.pst_Gas2 != NULL)
            {
                Bsp_CnvFP64ToArr(&pst_Fram->puc_PayLoad[21],st_GasMeasure.pst_Gas2->lf_PeakHight,FALSE);
                Bsp_CnvFP64ToArr(&pst_Fram->puc_PayLoad[29],st_GasMeasure.pst_Gas2->lf_Concentration,FALSE);
            }
            else
            {
                Bsp_CnvFP64ToArr(&pst_Fram->puc_PayLoad[21],0.0,FALSE);
                Bsp_CnvFP64ToArr(&pst_Fram->puc_PayLoad[29],0.0,FALSE);
            }
            res = TRUE;
        }
        break;      
//==================================================================================
//                                   �̵���IO����
//==================================================================================
    case 0x80:
        if(pst_Fram->uch_SubCmd == e_StdbusWriteCmd)
        {
            if(pst_Fram->uin_PayLoadLenth == 2)
            {
                BOOL b_State = (pst_Fram->puc_PayLoad[1] == FALSE) ? FALSE : TRUE ;
                Bsp_GpioWirte((GpioId_e)(e_IO_Relay0 + pst_Fram->puc_PayLoad[0]),b_State);
                res = TRUE;     //Ӧ��
            }
        }
        else if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            if(pst_Fram->uin_PayLoadLenth == 1)
            {
                pst_Fram->puc_PayLoad[1] = Bsp_GpioReadOut((GpioId_e)(e_IO_Relay0 + pst_Fram->puc_PayLoad[0]));
                pst_Fram->uin_PayLoadLenth = 2;
                res = TRUE;     //Ӧ��
            }
        }
        break;
       
//==================================================================================
//                                  ��ȡCO2������
//==================================================================================
	case 0xA0:
		if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
		{
			if(pst_Fram->uin_PayLoadLenth == 0)
			{
				//��ȡ��һҳ�������鳤��
				int i = 0;

				Bsp_CnvINT16UToArr(&pst_Fram->puc_PayLoad[0],st_Measure.st_SampleCO2.ul_Len,FALSE);

				for(i = 0; i < st_Measure.st_SampleCO2.ul_Len; i++)
				{
					Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[2+i*4],st_Measure.st_SampleCO2.af_Buff[i],FALSE);
				}

				pst_Fram->uin_PayLoadLenth = 2 + i * 4;

				res = 1;    //Ӧ��
			}
		}
		break;
//==================================================================================
//                                  ��ȡCO������
//==================================================================================
	case 0xA1:
		if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
		{
			if(pst_Fram->uin_PayLoadLenth == 0)
			{
				//��ȡ��һҳ�������鳤��
				int i = 0;

				Bsp_CnvINT16UToArr(&pst_Fram->puc_PayLoad[0],st_Measure.st_SampleCO.ul_Len,FALSE);

				for(i = 0; i < st_Measure.st_SampleCO.ul_Len; i++)
				{
					Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[2+i*4],st_Measure.st_SampleCO.af_Buff[i],FALSE);
				}

				pst_Fram->uin_PayLoadLenth = 2 + i * 4;

				res = 1;    //Ӧ��
			}
		}
		break;    
//==================================================================================
//                                  ��ȡNO������
//==================================================================================
	case 0xA2:
		if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
		{
			if(pst_Fram->uin_PayLoadLenth == 0)
			{
				//��ȡ��һҳ�������鳤��
				int i = 0;

				Bsp_CnvINT16UToArr(&pst_Fram->puc_PayLoad[0],st_Measure.st_SampleNO.ul_Len,FALSE);

				for(i = 0; i < st_Measure.st_SampleNO.ul_Len; i++)
				{
					Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[2+i*4],st_Measure.st_SampleNO.af_Buff[i],FALSE);
				}

				pst_Fram->uin_PayLoadLenth = 2 + i * 4;

				res = 1;    //Ӧ��
			}
		}
		break;
//==================================================================================
//                                  ��ȡHC������
//==================================================================================
	case 0xA3:
		if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
		{
			if(pst_Fram->uin_PayLoadLenth == 0)
			{
				//��ȡ��һҳ�������鳤��
				int i = 0;

				Bsp_CnvINT16UToArr(&pst_Fram->puc_PayLoad[0],st_Measure.st_SampleHC.ul_Len,FALSE);

				for(i = 0; i < st_Measure.st_SampleHC.ul_Len; i++)
				{
					Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[2+i*4],st_Measure.st_SampleHC.af_Buff[i],FALSE);
				}

				pst_Fram->uin_PayLoadLenth = 2 + i * 4;

				res = 1;    //Ӧ��
			}
		}
		break;
//==================================================================================
//                                  ��ȡƽ����Ũ��
//==================================================================================
	case 0xA6:
		if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
		{
			if(pst_Fram->uin_PayLoadLenth ==0)
			{
				pst_Fram->uin_PayLoadLenth = 8;
				Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[0],(FP32)st_Measure.lf_NO,FALSE);
				Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[4],(FP32)st_Measure.lf_HC,FALSE);
				res = 1;    //Ӧ��
			}
		}
		break;
//==================================================================================
//                                  ��ȡ�̶Ȳ�����
//==================================================================================
	case 0xAA:
		if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
		{
			if(pst_Fram->uin_PayLoadLenth == 1)
			{
				//��ȡ��һҳ�������鳤��
				int i = 0;
                int index = pst_Fram->puc_PayLoad[0];
				Bsp_CnvINT16UToArr(&pst_Fram->puc_PayLoad[0],st_Measure.st_SampleGrey[index].ul_Len,FALSE);

				for(i = 0; i < st_Measure.st_SampleGrey[index].ul_Len; i++)
				{
					Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[2+i*4],st_Measure.st_SampleGrey[index].af_Buff[i],FALSE);
				}

				pst_Fram->uin_PayLoadLenth = 2 + i * 4;

				res = 1;    //Ӧ��
			}
		}
		break;     
        
    default:
        break;

    }
    return res;
}
