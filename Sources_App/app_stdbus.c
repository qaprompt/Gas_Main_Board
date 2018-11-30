#include "app_cfg.h"
#include "mod_stdbusm.h"

static INT32U al_Buff[3800] = {0};//��ʱ

uint16_t Uint8TOUint16(uint8_t *puc_data)
{
    uint16_t * p = (uint16_t*)puc_data;
    uint16_t tmp = *p;
    
    return __REV16(tmp);
}

BOOL App_StdbusMasterDealFram(StdbusFram_t* pst_Fram) 
{
    BOOL res = FALSE;
    OS_ERR os_err;
    
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
            pst_Fram->puc_PayLoad[0] = USB4000.b_IsConnect;
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
                
                USB4000.b_SetFlag = TRUE;
                USB4000.ul_SetIntegralTime = i;
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
//                              ���ù������Ƿ���EDC(���Ӱ���������)
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
//                              ���ù������Ƿ���NLC(�����Բ���)
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
//                                  �л�����ģʽ 
//==================================================================================
    case 0x20:
        if(pst_Fram->uch_SubCmd == e_StdbusWriteCmd) 
        {   
            //byte0: 0:�л�������״̬
            //       1:�л�������״̬
            //       2:�л����궨״̬
            //byte1-5 float :�л����궨״̬��ʱ�·��ı궨Ũ��
            if(pst_Fram->uin_PayLoadLenth == 1)
            {
                if(pst_Fram->puc_PayLoad[0] == 0)
                {
                    Mod_GasAnalysisGoAdjZero(&GasAnalysis);            //�л�����0״̬    
                    res = TRUE;    //Ӧ��
                }   
                else if(pst_Fram->puc_PayLoad[0] == 1)
                {
                    Mod_GasAnalysisGoAnalysis(&GasAnalysis);        //�л�������״̬
                    res = TRUE;    //Ӧ��
                }
            }
            else if(pst_Fram->uin_PayLoadLenth == 5)
            {
                FP32 f;
                
                if(pst_Fram->puc_PayLoad[0] == 2)
                {
                    TRACE_DBG(">>DBG>>      ���յ��궨����\n\r");
                    
                    f = Bsp_CnvArrToFP32(&pst_Fram->puc_PayLoad[1],FALSE);
                    GasAnalysis.f_RefConcentration = f;            //����Ũ��
                    Mod_GasAnalysisGoCalibration(&GasAnalysis);
                    res = TRUE;    //Ӧ��                
                }
            }
            
        }
        else if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            //�������Ƿ����Ƿ��ڵ���
            pst_Fram->uin_PayLoadLenth = 1;
            pst_Fram->puc_PayLoad[0] = GasAnalysis.e_Ops;
            res = TRUE;    //Ӧ��
        }
        break;
     
//==================================================================================
//                          �޸�һ���궨��/��ȡһ���궨��
//==================================================================================
    case 0x22:
        if(pst_Fram->uch_SubCmd == e_StdbusWriteCmd)
        {   
            //д�������޸�һ���궨��   byte0 ������ byte1�������� byte2-byte9 double ����      
            if(pst_Fram->uin_PayLoadLenth == 10)
            {
                CaliPoint_t* p = GasAnalysis.pst_CaliPointList;
                INT8U i = pst_Fram->puc_PayLoad[0];
                INT8U uch_ParamType = pst_Fram->puc_PayLoad[1];
                FP32  f_Param = 0;
                f_Param = Bsp_CnvArrToFP32(&pst_Fram->puc_PayLoad[2],FALSE);
                
                if(i < DEF_MAX_POINT_NUM)
                {
                    switch(uch_ParamType)
                    {
                    case 0:
                        p[i].b_Use = (BOOL)f_Param;
                        SaveToEepromExt((uint32_t)(&p[i]),sizeof(CaliPoint_t));
                        break;
                    case 1:
                        p[i].f_Concentration = f_Param;
                        SaveToEepromExt((uint32_t)(&p[i]),sizeof(CaliPoint_t));
                        break; 
                    case 2:
                        p[i].f_Hi204_4 = f_Param;
                        SaveToEepromExt((uint32_t)(&p[i]),sizeof(CaliPoint_t));
                        break;
                    case 3:
                        p[i].f_Hi214_8 = f_Param;
                        SaveToEepromExt((uint32_t)(&p[i]),sizeof(CaliPoint_t));
                        break;
                    case 4:
                        p[i].f_Hi226_0 = f_Param;
                        SaveToEepromExt((uint32_t)(&p[i]),sizeof(CaliPoint_t));
                        break;
                    default:
                        break;
                    }
                    res = TRUE;            //Ӧ�� ���޸�����ԭʼ���ݷ���
                }
            }
        }
        else if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            //�������Ƕ�ȡһ���궨�� 
            //������        ���ر궨������ 
            //����һ������  ����ָ�������ı궨�������
            if(pst_Fram->uin_PayLoadLenth == 0)
            {
                uint16_t i = 0;
                //��ȡ��һҳ�������鳤��
                pst_Fram->puc_PayLoad[0] = (uint8_t)(GasAnalysis.uc_CaliPointSize);
                pst_Fram->uin_PayLoadLenth = 1;
                res = TRUE;    //Ӧ��
            }
            else if(pst_Fram->uin_PayLoadLenth == 1)     
            {
                uint8_t i = pst_Fram->puc_PayLoad[0];
                CaliPoint_t* p = GasAnalysis.pst_CaliPointList;
                
                if(i < DEF_MAX_POINT_NUM)
                {
                    pst_Fram->puc_PayLoad[1] = p[i].b_Use;
                    
                    Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[2], p[i].f_Concentration, FALSE);
                    Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[6], p[i].f_Hi204_4, FALSE);              
                    Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[10], p[i].f_Hi214_8, FALSE);
                    Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[14], p[i].f_Hi226_0, FALSE);

                    pst_Fram->uin_PayLoadLenth = 18;                
                }
                
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
            if(pst_Fram->uin_PayLoadLenth == 1)
            {
                INT8U   t = pst_Fram->puc_PayLoad[0];
                if ( t > 3)
                    t = 3;
                  
                GasAnalysis.uc_WorkLineOrder = t;
                pst_Fram->uin_PayLoadLenth = 1;
            }
        }
        else if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            if(pst_Fram->uin_PayLoadLenth == 0)
            {
                pst_Fram->puc_PayLoad[0] = GasAnalysis.uc_WorkLineOrder;
                pst_Fram->uin_PayLoadLenth = 1;                
            }
        }
        res = TRUE;    //Ӧ��
        break;
//==================================================================================
//                                  ������������
//==================================================================================
    case 0x24:
        if(pst_Fram->uch_SubCmd == e_StdbusWriteCmd) 
        {   
            //д�����ǽ�����������
            if(pst_Fram->uin_PayLoadLenth == 0)
            {   
                Mod_GasAnalysisMarkWorkLine(&GasAnalysis);
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
            if(pst_Fram->uin_PayLoadLenth == 2)     
            {
                uint8_t i = pst_Fram->puc_PayLoad[1];
                
                float* p;
                
                switch(pst_Fram->puc_PayLoad[0])
                {
                case 0:
                    p = GasAnalysis.pf_a204;
                    break;
                    
                case 1:
                    p = GasAnalysis.pf_a214;
                    break;
                    
                case 2:
                    p = GasAnalysis.pf_a226;
                    break;
                
                default:
                    break;
                }
                //ǰ�����������޸�ֱ�ӷ���
                Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[2],p[i],FALSE);
                pst_Fram->uin_PayLoadLenth = 6;
                res = TRUE;    //Ӧ��
            }
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
                //    Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[i*4],ast_GreyChannle[i].f_Volt,FALSE);
                }
            }
            res = TRUE;    //Ӧ��
        }
        break;
        
//==================================================================================
//                                   ��ȡ����    
//==================================================================================
    case 0x40:
        if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
             //��һ���ֽ���PageIndex  �ڶ������ֽ���ReadAddress ��������ֽ���ReadLenth
          
            if(pst_Fram->uin_PayLoadLenth == 0)
            {
                uint16_t i = 0;
                //��ȡ��һҳ�������鳤��
                pst_Fram->puc_PayLoad[0] = (uint8_t)(GasAnalysis.uin_Pixels>>8);
                pst_Fram->puc_PayLoad[1] = (uint8_t)(GasAnalysis.uin_Pixels&0x00ff);
                pst_Fram->uin_PayLoadLenth = 2;
                
                /* ���ع��׵� ������ ȷ�������ٴ���һ���� ���¹���*/
                CPU_IntDis();//OSSchedLock(&os_err);
                for(i = 0; i < GasAnalysis.uin_Pixels; i++)
                {
                    double data = GasAnalysis.plf_Spectrum[i] * 100;
                    al_Buff[i] = (INT32U)data;
                }
                CPU_IntEn();OSSchedUnlock(&os_err);
            }
            else if(pst_Fram->uin_PayLoadLenth == 4)     
            {
                uint16_t i = 0;
                uint16_t uin_Offset = Uint8TOUint16(pst_Fram->puc_PayLoad);
                uint16_t uin_Lenth = Uint8TOUint16(pst_Fram->puc_PayLoad + 2);
                
                pst_Fram->uin_PayLoadLenth = 4 + uin_Lenth * 4;
                for(i = 0; i<uin_Lenth;i++)
                {
                    int32_t j = al_Buff[uin_Offset + i];
                    
                    pst_Fram->puc_PayLoad[i * 4 + 0 + 4] = (uint8_t)(j>>24);
                    pst_Fram->puc_PayLoad[i * 4 + 1 + 4] = (uint8_t)(j>>16);
                    pst_Fram->puc_PayLoad[i * 4 + 2 + 4] = (uint8_t)(j>>8);
                    pst_Fram->puc_PayLoad[i * 4 + 3 + 4] = (uint8_t)(j&0xff);
                }
            }
            res = TRUE;    //Ӧ��
        }
        break;

//==================================================================================
//                                  ��ȡ��������    
//==================================================================================
    case 0x41:
        if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
             //��һ���ֽ���PageIndex  �ڶ������ֽ���ReadAddress ��������ֽ���ReadLenth
          
            if(pst_Fram->uin_PayLoadLenth == 0)
            {
                uint16_t i = 0;
                //��ȡ��һҳ�������鳤��
                pst_Fram->puc_PayLoad[0] = (uint8_t)(GasAnalysis.uin_Pixels>>8);
                pst_Fram->puc_PayLoad[1] = (uint8_t)(GasAnalysis.uin_Pixels&0x00ff);
                pst_Fram->uin_PayLoadLenth = 2;
                
                /* ���ع��׵� ������ ȷ�������ٴ���һ���� ���¹���*/
                CPU_IntDis();//OSSchedLock(&os_err);
                for(i = 0; i < GasAnalysis.uin_Pixels; i++)
                {
                    double data = GasAnalysis.plf_BkSpectrum[i] * 100;
                    al_Buff[i] = (INT32U)data;
                }
                CPU_IntEn();//OSSchedUnlock(&os_err);
            }
            else if(pst_Fram->uin_PayLoadLenth == 4)     
            {
                uint16_t i = 0;
                uint16_t uin_Offset = Uint8TOUint16(pst_Fram->puc_PayLoad);
                uint16_t uin_Lenth = Uint8TOUint16(pst_Fram->puc_PayLoad + 2);
                
                pst_Fram->uin_PayLoadLenth = 4 + uin_Lenth * 4;
                for(i = 0; i<uin_Lenth;i++)
                {
                    int32_t j = al_Buff[uin_Offset + i];
                    
                    pst_Fram->puc_PayLoad[i * 4 + 0 + 4] = (uint8_t)(j>>24);
                    pst_Fram->puc_PayLoad[i * 4 + 1 + 4] = (uint8_t)(j>>16);
                    pst_Fram->puc_PayLoad[i * 4 + 2 + 4] = (uint8_t)(j>>8);
                    pst_Fram->puc_PayLoad[i * 4 + 3 + 4] = (uint8_t)(j&0xff);
                }
            }
            res = TRUE;    //Ӧ��
        }
        break;
//==================================================================================
//                                  ��ȡ��Թ���    
//==================================================================================
    case 0x42:
        if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
             //��һ���ֽ���PageIndex  �ڶ������ֽ���ReadAddress ��������ֽ���ReadLenth
          
            if(pst_Fram->uin_PayLoadLenth == 0)
            {
                uint16_t i = 0;

                //��ȡ��һҳ�������鳤��
                pst_Fram->puc_PayLoad[0] = (uint8_t)(GasAnalysis.uin_Pixels>>8);
                pst_Fram->puc_PayLoad[1] = (uint8_t)(GasAnalysis.uin_Pixels&0x00ff);
                pst_Fram->uin_PayLoadLenth = 2;
                
                /* ���ع��׵� ������ ȷ�������ٴ���һ���� ���¹��� */
                CPU_IntDis();//OSSchedLock(&os_err);
                for(i = 0; i < GasAnalysis.uin_Pixels; i++)
                {
                    double data = GasAnalysis.plf_AbsSpectrum[i] * 100;
                    al_Buff[i] = (INT32U)data;
                }
                CPU_IntEn();//OSSchedUnlock(&os_err);
                
                pst_Fram->puc_PayLoad[0] = (uint8_t)(i>>8);
                pst_Fram->puc_PayLoad[1] = (uint8_t)(i&0x00ff);
                pst_Fram->uin_PayLoadLenth = 2;

            }
            else if(pst_Fram->uin_PayLoadLenth == 4)     
            {
                uint16_t i = 0;
                uint16_t uin_Offset = Uint8TOUint16(pst_Fram->puc_PayLoad);
                uint16_t uin_Lenth = Uint8TOUint16(pst_Fram->puc_PayLoad + 2);
                
                pst_Fram->uin_PayLoadLenth = 4 + uin_Lenth * 4;
                for(i = 0; i<uin_Lenth;i++)
                {
                    uint32_t j = al_Buff[uin_Offset + i];
                    
                    pst_Fram->puc_PayLoad[i * 4 + 0 + 4] = (uint8_t)(j>>24);
                    pst_Fram->puc_PayLoad[i * 4 + 1 + 4] = (uint8_t)(j>>16);
                    pst_Fram->puc_PayLoad[i * 4 + 2 + 4] = (uint8_t)(j>>8);
                    pst_Fram->puc_PayLoad[i * 4 + 3 + 4] = (uint8_t)(j&0xff);
                }
            }
            res = TRUE;    //Ӧ��
        }
        break;  

//==================================================================================
//                                 ��ȡ���շ�߶�    
//==================================================================================
    case 0x50:
        if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            if(pst_Fram->uin_PayLoadLenth == 0)
            {
                //�����������շ�ĸ߶�
                pst_Fram->puc_PayLoad[0] = 3;
                Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[1],GasAnalysis.f_Hi204_4,FALSE);
                Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[5],GasAnalysis.f_Hi214_8,FALSE);
                Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[9],GasAnalysis.f_Hi226_0,FALSE);
                pst_Fram->uin_PayLoadLenth = 13;
                res = TRUE;    //Ӧ��
            }
            else if(pst_Fram->uin_PayLoadLenth == 1)     
            {
                //�����������ض�Ӧ�����շ�߶�
            }

        }
        break;
//==================================================================================
//                                    ��ȡŨ��  
//==================================================================================
    case 0x51:
        if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            if(pst_Fram->uin_PayLoadLenth == 0)
            {
                //�����������շ�ĸ߶�
                pst_Fram->puc_PayLoad[0] = 3;
                Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[1],GasAnalysis.f_Concentration_204,FALSE);
                Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[5],GasAnalysis.f_Concentration_214,FALSE);
                Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[9],GasAnalysis.f_Concentration_226,FALSE);
                pst_Fram->uin_PayLoadLenth = 13;
                res = TRUE;    //Ӧ��
            }
            else if(pst_Fram->uin_PayLoadLenth == 1)     
            {
                //�����������ض�Ӧ�����շ�߶�
            }
        }
        break;      
    default:
        break;
      
    }


    return res;
}

#if 0

#define e_StdbusReadCmd     0x55
#define e_StdbusWriteCmd    0x66

static INT32U al_Buff[3800] = {0};//��ʱ

uint16_t Uint8TOUint16(uint8_t *puc_data)
{
    uint16_t * p = (uint16_t*)puc_data;
    uint16_t tmp = *p;
    
    return __REV16(tmp);
}


//==================================================================================
//| �������� | DealAppCmd
//|----------|----------------------------------------------------------------------
//| �������� | ������λ���������������� 
//|----------|----------------------------------------------------------------------
//| ������� | ��
//|----------|----------------------------------------------------------------------
//| ���ز��� | 0:��Ӧ�� 1:Ӧ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
uint8_t Deal_SlavePack(StdBus_t* pst_Fram)
{
    uint8_t res = 0;
    OS_ERR os_err;
    
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
            pst_Fram->puc_PayLoad[0] = USB4000.b_IsConnect;
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
                
                USB4000.b_SetFlag = TRUE;
                USB4000.ul_SetIntegralTime = i;
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
//                              ���ù������Ƿ���EDC(���Ӱ���������)
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
//                              ���ù������Ƿ���NLC(�����Բ���)
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
//                                  �л�����ģʽ 
//==================================================================================
    case 0x20:
        if(pst_Fram->uch_SubCmd == e_StdbusWriteCmd) 
        {   
            //byte0: 0:�л�������״̬
            //       1:�л�������״̬
            //       2:�л����궨״̬
            //byte1-5 float :�л����궨״̬��ʱ�·��ı궨Ũ��
            if(pst_Fram->uin_PayLoadLenth == 1)
            {
                if(pst_Fram->puc_PayLoad[0] == 0)
                {
                    Mod_GasAnalysisGoAdjZero(&GasAnalysis);            //�л�����0״̬    
                    res = TRUE;    //Ӧ��
                }   
                else if(pst_Fram->puc_PayLoad[0] == 1)
                {
                    Mod_GasAnalysisGoAnalysis(&GasAnalysis);        //�л�������״̬
                    res = TRUE;    //Ӧ��
                }
            }
            else if(pst_Fram->uin_PayLoadLenth == 5)
            {
                FP32 f;
                
                if(pst_Fram->puc_PayLoad[0] == 2)
                {
                    TRACE_DBG(">>DBG>>      ���յ��궨����\n\r");
                    
                    f = Bsp_CnvArrToFP32(&pst_Fram->puc_PayLoad[1],FALSE);
                    GasAnalysis.f_RefConcentration = f;            //����Ũ��
                    Mod_GasAnalysisGoCalibration(&GasAnalysis);
                    res = TRUE;    //Ӧ��                
                }
            }
            
        }
        else if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            //�������Ƿ����Ƿ��ڵ���
            pst_Fram->uin_PayLoadLenth = 1;
            pst_Fram->puc_PayLoad[0] = GasAnalysis.e_Ops;
            res = TRUE;    //Ӧ��
        }
        break;
     
//==================================================================================
//                          �޸�һ���궨��/��ȡһ���궨��
//==================================================================================
    case 0x22:
        if(pst_Fram->uch_SubCmd == e_StdbusWriteCmd)
        {   
            //д�������޸�һ���궨��   byte0 ������ byte1�������� byte2-byte9 double ����      
            if(pst_Fram->uin_PayLoadLenth == 10)
            {
                CaliPoint_t* p = GasAnalysis.pst_CaliPointList;
                INT8U i = pst_Fram->puc_PayLoad[0];
                INT8U uch_ParamType = pst_Fram->puc_PayLoad[1];
                FP32  f_Param = 0;
                f_Param = Bsp_CnvArrToFP32(&pst_Fram->puc_PayLoad[2],FALSE);
                
                if(i < DEF_MAX_POINT_NUM)
                {
                    switch(uch_ParamType)
                    {
                    case 0:
                        p[i].b_Use = (BOOL)f_Param;
                        SaveToEepromExt((uint32_t)(&p[i]),sizeof(CaliPoint_t));
                        break;
                    case 1:
                        p[i].f_Concentration = f_Param;
                        SaveToEepromExt((uint32_t)(&p[i]),sizeof(CaliPoint_t));
                        break; 
                    case 2:
                        p[i].f_Hi204_4 = f_Param;
                        SaveToEepromExt((uint32_t)(&p[i]),sizeof(CaliPoint_t));
                        break;
                    case 3:
                        p[i].f_Hi214_8 = f_Param;
                        SaveToEepromExt((uint32_t)(&p[i]),sizeof(CaliPoint_t));
                        break;
                    case 4:
                        p[i].f_Hi226_0 = f_Param;
                        SaveToEepromExt((uint32_t)(&p[i]),sizeof(CaliPoint_t));
                        break;
                    default:
                        break;
                    }
                    res = TRUE;            //Ӧ�� ���޸�����ԭʼ���ݷ���
                }
            }
        }
        else if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            //�������Ƕ�ȡһ���궨�� 
            //������        ���ر궨������ 
            //����һ������  ����ָ�������ı궨�������
            if(pst_Fram->uin_PayLoadLenth == 0)
            {
                uint16_t i = 0;
                //��ȡ��һҳ�������鳤��
                pst_Fram->puc_PayLoad[0] = (uint8_t)(GasAnalysis.uc_CaliPointSize);
                pst_Fram->uin_PayLoadLenth = 1;
                res = TRUE;    //Ӧ��
            }
            else if(pst_Fram->uin_PayLoadLenth == 1)     
            {
                uint8_t i = pst_Fram->puc_PayLoad[0];
                CaliPoint_t* p = GasAnalysis.pst_CaliPointList;
                
                if(i < DEF_MAX_POINT_NUM)
                {
                    pst_Fram->puc_PayLoad[1] = p[i].b_Use;
                    
                    Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[2], p[i].f_Concentration, FALSE);
                    Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[6], p[i].f_Hi204_4, FALSE);              
                    Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[10], p[i].f_Hi214_8, FALSE);
                    Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[14], p[i].f_Hi226_0, FALSE);

                    pst_Fram->uin_PayLoadLenth = 18;                
                }
                
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
            if(pst_Fram->uin_PayLoadLenth == 1)
            {
                INT8U   t = pst_Fram->puc_PayLoad[0];
                if ( t > 3)
                    t = 3;
                  
                GasAnalysis.uc_WorkLineOrder = t;
                pst_Fram->uin_PayLoadLenth = 1;
            }
        }
        else if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            if(pst_Fram->uin_PayLoadLenth == 0)
            {
                pst_Fram->puc_PayLoad[0] = GasAnalysis.uc_WorkLineOrder;
                pst_Fram->uin_PayLoadLenth = 1;                
            }
        }
        res = TRUE;    //Ӧ��
        break;
//==================================================================================
//                                  ������������
//==================================================================================
    case 0x24:
        if(pst_Fram->uch_SubCmd == e_StdbusWriteCmd) 
        {   
            //д�����ǽ�����������
            if(pst_Fram->uin_PayLoadLenth == 0)
            {   
                Mod_GasAnalysisMarkWorkLine(&GasAnalysis);
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
            if(pst_Fram->uin_PayLoadLenth == 2)     
            {
                uint8_t i = pst_Fram->puc_PayLoad[1];
                
                float* p;
                
                switch(pst_Fram->puc_PayLoad[0])
                {
                case 0:
                    p = GasAnalysis.pf_a204;
                    break;
                    
                case 1:
                    p = GasAnalysis.pf_a214;
                    break;
                    
                case 2:
                    p = GasAnalysis.pf_a226;
                    break;
                
                default:
                    break;
                }
                //ǰ�����������޸�ֱ�ӷ���
                Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[2],p[i],FALSE);
                pst_Fram->uin_PayLoadLenth = 6;
                res = TRUE;    //Ӧ��
            }
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
                //    Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[i*4],ast_GreyChannle[i].f_Volt,FALSE);
                }
            }
            res = TRUE;    //Ӧ��
        }
        break;
        
//==================================================================================
//                                   ��ȡ����    
//==================================================================================
    case 0x40:
        if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
             //��һ���ֽ���PageIndex  �ڶ������ֽ���ReadAddress ��������ֽ���ReadLenth
          
            if(pst_Fram->uin_PayLoadLenth == 0)
            {
                uint16_t i = 0;
                //��ȡ��һҳ�������鳤��
                pst_Fram->puc_PayLoad[0] = (uint8_t)(GasAnalysis.uin_Pixels>>8);
                pst_Fram->puc_PayLoad[1] = (uint8_t)(GasAnalysis.uin_Pixels&0x00ff);
                pst_Fram->uin_PayLoadLenth = 2;
                
                /* ���ع��׵� ������ ȷ�������ٴ���һ���� ���¹���*/
                CPU_IntDis();//OSSchedLock(&os_err);
                for(i = 0; i < GasAnalysis.uin_Pixels; i++)
                {
                    double data = GasAnalysis.plf_Spectrum[i] * 100;
                    al_Buff[i] = (INT32U)data;
                }
                CPU_IntEn();OSSchedUnlock(&os_err);
            }
            else if(pst_Fram->uin_PayLoadLenth == 4)     
            {
                uint16_t i = 0;
                uint16_t uin_Offset = Uint8TOUint16(pst_Fram->puc_PayLoad);
                uint16_t uin_Lenth = Uint8TOUint16(pst_Fram->puc_PayLoad + 2);
                
                pst_Fram->uin_PayLoadLenth = 4 + uin_Lenth * 4;
                for(i = 0; i<uin_Lenth;i++)
                {
                    int32_t j = al_Buff[uin_Offset + i];
                    
                    pst_Fram->puc_PayLoad[i * 4 + 0 + 4] = (uint8_t)(j>>24);
                    pst_Fram->puc_PayLoad[i * 4 + 1 + 4] = (uint8_t)(j>>16);
                    pst_Fram->puc_PayLoad[i * 4 + 2 + 4] = (uint8_t)(j>>8);
                    pst_Fram->puc_PayLoad[i * 4 + 3 + 4] = (uint8_t)(j&0xff);
                }
            }
            res = TRUE;    //Ӧ��
        }
        break;

//==================================================================================
//                                  ��ȡ��������    
//==================================================================================
    case 0x41:
        if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
             //��һ���ֽ���PageIndex  �ڶ������ֽ���ReadAddress ��������ֽ���ReadLenth
          
            if(pst_Fram->uin_PayLoadLenth == 0)
            {
                uint16_t i = 0;
                //��ȡ��һҳ�������鳤��
                pst_Fram->puc_PayLoad[0] = (uint8_t)(GasAnalysis.uin_Pixels>>8);
                pst_Fram->puc_PayLoad[1] = (uint8_t)(GasAnalysis.uin_Pixels&0x00ff);
                pst_Fram->uin_PayLoadLenth = 2;
                
                /* ���ع��׵� ������ ȷ�������ٴ���һ���� ���¹���*/
                CPU_IntDis();//OSSchedLock(&os_err);
                for(i = 0; i < GasAnalysis.uin_Pixels; i++)
                {
                    double data = GasAnalysis.plf_BkSpectrum[i] * 100;
                    al_Buff[i] = (INT32U)data;
                }
                CPU_IntEn();//OSSchedUnlock(&os_err);
            }
            else if(pst_Fram->uin_PayLoadLenth == 4)     
            {
                uint16_t i = 0;
                uint16_t uin_Offset = Uint8TOUint16(pst_Fram->puc_PayLoad);
                uint16_t uin_Lenth = Uint8TOUint16(pst_Fram->puc_PayLoad + 2);
                
                pst_Fram->uin_PayLoadLenth = 4 + uin_Lenth * 4;
                for(i = 0; i<uin_Lenth;i++)
                {
                    int32_t j = al_Buff[uin_Offset + i];
                    
                    pst_Fram->puc_PayLoad[i * 4 + 0 + 4] = (uint8_t)(j>>24);
                    pst_Fram->puc_PayLoad[i * 4 + 1 + 4] = (uint8_t)(j>>16);
                    pst_Fram->puc_PayLoad[i * 4 + 2 + 4] = (uint8_t)(j>>8);
                    pst_Fram->puc_PayLoad[i * 4 + 3 + 4] = (uint8_t)(j&0xff);
                }
            }
            res = TRUE;    //Ӧ��
        }
        break;
//==================================================================================
//                                  ��ȡ��Թ���    
//==================================================================================
    case 0x42:
        if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
             //��һ���ֽ���PageIndex  �ڶ������ֽ���ReadAddress ��������ֽ���ReadLenth
          
            if(pst_Fram->uin_PayLoadLenth == 0)
            {
                uint16_t i = 0;

                //��ȡ��һҳ�������鳤��
                pst_Fram->puc_PayLoad[0] = (uint8_t)(GasAnalysis.uin_Pixels>>8);
                pst_Fram->puc_PayLoad[1] = (uint8_t)(GasAnalysis.uin_Pixels&0x00ff);
                pst_Fram->uin_PayLoadLenth = 2;
                
                /* ���ع��׵� ������ ȷ�������ٴ���һ���� ���¹��� */
                CPU_IntDis();//OSSchedLock(&os_err);
                for(i = 0; i < GasAnalysis.uin_Pixels; i++)
                {
                    double data = GasAnalysis.plf_AbsSpectrum[i] * 100;
                    al_Buff[i] = (INT32U)data;
                }
                CPU_IntEn();//OSSchedUnlock(&os_err);
                
                pst_Fram->puc_PayLoad[0] = (uint8_t)(i>>8);
                pst_Fram->puc_PayLoad[1] = (uint8_t)(i&0x00ff);
                pst_Fram->uin_PayLoadLenth = 2;

            }
            else if(pst_Fram->uin_PayLoadLenth == 4)     
            {
                uint16_t i = 0;
                uint16_t uin_Offset = Uint8TOUint16(pst_Fram->puc_PayLoad);
                uint16_t uin_Lenth = Uint8TOUint16(pst_Fram->puc_PayLoad + 2);
                
                pst_Fram->uin_PayLoadLenth = 4 + uin_Lenth * 4;
                for(i = 0; i<uin_Lenth;i++)
                {
                    uint32_t j = al_Buff[uin_Offset + i];
                    
                    pst_Fram->puc_PayLoad[i * 4 + 0 + 4] = (uint8_t)(j>>24);
                    pst_Fram->puc_PayLoad[i * 4 + 1 + 4] = (uint8_t)(j>>16);
                    pst_Fram->puc_PayLoad[i * 4 + 2 + 4] = (uint8_t)(j>>8);
                    pst_Fram->puc_PayLoad[i * 4 + 3 + 4] = (uint8_t)(j&0xff);
                }
            }
            res = TRUE;    //Ӧ��
        }
        break;  

//==================================================================================
//                                 ��ȡ���շ�߶�    
//==================================================================================
    case 0x50:
        if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            if(pst_Fram->uin_PayLoadLenth == 0)
            {
                //�����������շ�ĸ߶�
                pst_Fram->puc_PayLoad[0] = 3;
                Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[1],GasAnalysis.f_Hi204_4,FALSE);
                Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[5],GasAnalysis.f_Hi214_8,FALSE);
                Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[9],GasAnalysis.f_Hi226_0,FALSE);
                pst_Fram->uin_PayLoadLenth = 13;
                res = TRUE;    //Ӧ��
            }
            else if(pst_Fram->uin_PayLoadLenth == 1)     
            {
                //�����������ض�Ӧ�����շ�߶�
            }

        }
        break;
//==================================================================================
//                                    ��ȡŨ��  
//==================================================================================
    case 0x51:
        if(pst_Fram->uch_SubCmd == e_StdbusReadCmd)
        {
            if(pst_Fram->uin_PayLoadLenth == 0)
            {
                //�����������շ�ĸ߶�
                pst_Fram->puc_PayLoad[0] = 3;
                Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[1],GasAnalysis.f_Concentration_204,FALSE);
                Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[5],GasAnalysis.f_Concentration_214,FALSE);
                Bsp_CnvFP32ToArr(&pst_Fram->puc_PayLoad[9],GasAnalysis.f_Concentration_226,FALSE);
                pst_Fram->uin_PayLoadLenth = 13;
                res = TRUE;    //Ӧ��
            }
            else if(pst_Fram->uin_PayLoadLenth == 1)     
            {
                //�����������ض�Ӧ�����շ�߶�
            }
        }
        break;      
    default:
        break;
      
    }


    return res;
}

uint8_t Deal_MasterPack(StdBus_t* pst_Fram)
{
    
    return 0;
}
#endif