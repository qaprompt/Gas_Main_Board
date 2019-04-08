
#include "App_Include.h"

#define DEF_CALIBPOINT_DBG_EN           FALSE

#if (DEF_CALIBPOINT_DBG_EN == TRUE)
    #define CALIBPOINT_DBG(...)         do {                                \
                                            OS_ERR os_err;                  \
                                            OSSchedLock(&os_err);           \
                                            printf(__VA_ARGS__);            \
                                            OSSchedUnlock(&os_err);         \
                                        }while(0)
#else
    #define CALIBPOINT_DBG(...)             
#endif

CalibPoint_t ast_CalibPoint_GasNO[DEF_CALIBPOINT_MAX] = {0};
CalibPoint_t ast_CalibPoint_GasHC[DEF_CALIBPOINT_MAX] = {0};

CalibPointList_t st_CPList_GasNO = {
    &ast_CalibPoint_GasNO[0],
    DEF_CALIBPOINT_MAX,
    0,
};

CalibPointList_t st_CPList_GasHC = {
    &ast_CalibPoint_GasNO[0],
    DEF_CALIBPOINT_MAX,
    0,
};


BOOL Mod_CalibPointListInit(CalibPointList_t* pst_CpointList)
{
    BOOL res = TRUE;
    INT32U i; 
    if(pst_CpointList == NULL || pst_CpointList->pst_List == NULL)
        return FALSE;
    pst_CpointList->ul_Use = 0;
    for(i = 0; i < pst_CpointList->ul_Lenth; i++)
    {
        if (pst_CpointList->pst_List[i].b_Use != TRUE &&
            pst_CpointList->pst_List[i].b_Use != FALSE)    //��Ϊ0 �� 1 ��������� ��Ҫ���¸�ֵ
        {                              
            pst_CpointList->pst_List[i].b_Use = FALSE;
            pst_CpointList->pst_List[i].f_X = 0.0;
            pst_CpointList->pst_List[i].f_Y = 0.0;
            res = FALSE;
        }
        else if (pst_CpointList->pst_List[i].b_Use == TRUE)
        {
            pst_CpointList->ul_Use++;
        }
    }
    CALIBPOINT_DBG("У׼���б���ʹ��: %d\r\n",pst_CpointList->ul_Use);   
    return res;
}


BOOL Mod_CalibPointListClear(CalibPointList_t* pst_CpointList)
{
    INT32U  i;
    
    
    if(pst_CpointList == NULL || pst_CpointList->pst_List == NULL)
        return FALSE;

    for(i = 0; i < pst_CpointList->ul_Lenth; i++)
    {
        pst_CpointList->pst_List[i].b_Use = FALSE;
        pst_CpointList->pst_List[i].f_X = 0.0;
        pst_CpointList->pst_List[i].f_Y = 0.0;
        SaveToEepromExt((INT32U)(&pst_CpointList->pst_List[i]),sizeof(CalibPoint_t));
    }
    Mod_CalibPointListInit(pst_CpointList);     //ˢ��ʹ�ø���
    CALIBPOINT_DBG("У׼���б����\r\n"); 
    return TRUE;
}

BOOL Mod_CalibPointListReadOnePoint(CalibPointList_t* pst_CpointList, INT32U ul_Index, CalibPoint_t* pst_Point)
{
    if(pst_CpointList == NULL || pst_CpointList->pst_List == NULL || ul_Index >= pst_CpointList->ul_Lenth )
        return FALSE;
    pst_Point->b_Use = pst_CpointList->pst_List[ul_Index].b_Use;
    pst_Point->f_X = pst_CpointList->pst_List[ul_Index].f_X;
    pst_Point->f_Y = pst_CpointList->pst_List[ul_Index].f_Y;
    CALIBPOINT_DBG("��ȡУ׼���б� %d\r\n",ul_Index); 
    return TRUE;
}

BOOL Mod_CalibPointListEditOnePoint(CalibPointList_t* pst_CpointList, INT32U ul_Index, CalibPoint_t* pst_Point)
{
    if(pst_CpointList == NULL || pst_CpointList->pst_List == NULL || ul_Index >= pst_CpointList->ul_Lenth )
        return FALSE;
    pst_CpointList->pst_List[ul_Index].b_Use = pst_Point->b_Use;
    pst_CpointList->pst_List[ul_Index].f_X = pst_Point->f_X;
    pst_CpointList->pst_List[ul_Index].f_Y = pst_Point->f_Y;
    SaveToEepromExt((INT32U)(&pst_CpointList->pst_List[ul_Index]),sizeof(CalibPoint_t));
    CALIBPOINT_DBG("�༭У׼���б� %d\r\n",ul_Index); 
    Mod_CalibPointListInit(pst_CpointList);     //ˢ��ʹ�ø���
    return TRUE;
}

BOOL Mod_CalibPointListAddOnePoint(CalibPointList_t* pst_CpointList, CalibPoint_t* pst_Point)
{
    INT32U  i;
    if(pst_CpointList == NULL || pst_CpointList->pst_List == NULL)
        return FALSE;
    if(pst_CpointList->ul_Use < pst_CpointList->ul_Lenth)
    {
        for(i = 0; i < pst_CpointList->ul_Lenth; i++)
        {
            if(pst_CpointList->pst_List[i].b_Use == FALSE)
            {
                pst_CpointList->pst_List[i].b_Use = TRUE;
                pst_CpointList->pst_List[i].f_X = pst_Point->f_X;
                pst_CpointList->pst_List[i].f_Y = pst_Point->f_Y;
                SaveToEepromExt((INT32U)(&pst_CpointList->pst_List[i]),sizeof(CalibPoint_t));
                break;
            }
        }
        Mod_CalibPointListInit(pst_CpointList);     //ˢ��ʹ�ø���
        CALIBPOINT_DBG("���һ��У׼��\r\n"); 
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL Mod_CalibPointListDeleteOnePoint(CalibPointList_t* pst_CpointList, INT32U ul_Index)
{
    if(pst_CpointList == NULL || pst_CpointList->pst_List == NULL || ul_Index >= pst_CpointList->ul_Lenth )
        return FALSE;
    pst_CpointList->pst_List[ul_Index].b_Use = FALSE;
    pst_CpointList->pst_List[ul_Index].f_X = 0.0;
    pst_CpointList->pst_List[ul_Index].f_Y = 0.0;
    SaveToEepromExt((INT32U)(&pst_CpointList->pst_List[ul_Index]),sizeof(CalibPoint_t));
    Mod_CalibPointListInit(pst_CpointList);     //ˢ��ʹ�ø���
    CALIBPOINT_DBG("ɾ��һ��У׼��%d\r\n",ul_Index); 
    return TRUE;
}

BOOL Mod_CalibPointListNihe(CalibPointList_t* pst_CpointList,INT8U uch_NiheOrder,FP32* pf_NiheCoeff)
{
    FP32    af_X[DEF_CALIBPOINT_MAX] = {0.0};
    FP32    af_Y[DEF_CALIBPOINT_MAX] = {0.0};
    INT32U  ul_Use = 0;
    INT32U  i,j;
	FP32 t;
    if(pst_CpointList == NULL || pst_CpointList->pst_List == NULL)
        return FALSE;

    for(i = 0; i < pst_CpointList->ul_Lenth; i++)
    {
        if (pst_CpointList->pst_List[i].b_Use == TRUE)
        {
            af_X[ul_Use] = pst_CpointList->pst_List[i].f_X;
            af_Y[ul_Use] = pst_CpointList->pst_List[i].f_Y;
            ul_Use++;
        }
    }
    

    /* ��С�������� */
	for(i = 0; i < ul_Use-1; i++)
	{
		for(j = i+1; j < ul_Use; j++)
		{
			if(af_X[j] < af_X[i])
			{
				t = af_X[j];				//����������
				af_X[j] = af_X[i];
				af_X[i] = t;
                
				t = af_Y[j];				//����������
				af_Y[j] = af_Y[i];
				af_Y[i] = t;
			}
		}
	}

    if(ul_Use == 0)
        return FALSE;

    NiHe1(af_X,af_Y,ul_Use, pf_NiheCoeff,uch_NiheOrder);
    
    CALIBPOINT_DBG("���У׼���б�\r\n");
    CALIBPOINT_DBG("��Ͻ���:%d, ��ϵ���:%d\r\n",uch_NiheOrder,ul_Use);
    for(i = 0; i < ul_Use; i++)
        CALIBPOINT_DBG("��ϵ�%d X = %f, Y = %f\r\n",i,af_X[i],af_Y[i]);

    for(i = 0; i <= uch_NiheOrder; i++)
    {
        CALIBPOINT_DBG("���ϵ��[%d] = %e\r\n",i,pf_NiheCoeff[i]); 
    }
    
    return TRUE;
}
