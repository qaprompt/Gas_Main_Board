//==================================================================================================
//| �ļ����� | Task_GreyProc.c
//|--------- |--------------------------------------------------------------------------------------
//| �ļ����� | �Ҷȴ���
//|--------- |--------------------------------------------------------------------------------------
//| ��Ȩ���� | 
//|----------|--------------------------------------------------------------------------------------
//|  �汾    |  ʱ��       |  ����     | ����
//|--------- |-------------|-----------|------------------------------------------------------------
//|  V1.0    | 2018.11.02  |  wjb      | ���� ����֤�Ƿ�����DMA������ȡLT1867
//==================================================================================================
#include "app_cfg.h"
#include "bsp.h"
void t(void)
{
    INT16U i;
    INT16U buff[8] = {0};
    for (i = 0; i < 8; i++)
    {
        buff[i] = Bsp_LTC1867SampleAvg(&st_LTC1867A,i,1);   
    }      
}

void Task_GreyProc(void  *p_arg)
{
    OS_ERR  os_err;
    
    while(TRUE)
    {


        {
            INT16U i;
            INT16U buff[8] = {0};
            for (i = 0; i < 8; i++)
            {
                static INT8U b[100] = {0};
                buff[i] = Bsp_LTC1867SampleAvg(&st_LTC1867B,i,1);  
                sprintf(b,"b[%u] = %u\r\n",i,buff[i]);
                TRACE_DBG((INT8U*)b);
                
                                /* ÿ0.1s�ɼ�����һ�� �Ҷ���Ϣ */
                OSTimeDlyHMSM(0u, 0u, 0u, 100,
                              OS_OPT_TIME_HMSM_STRICT | OS_OPT_TIME_PERIODIC,/* ����ģʽ */
                              &os_err);
            }
            
            for (i = 0; i < 2; i++)
            {
                static INT8U b[100] = {0};
                buff[i] = Bsp_LTC1867SampleAvg(&st_LTC1867A,i,1);  
                sprintf(b,"b[%u] = %u\r\n",i+8,buff[i]);
                TRACE_DBG((INT8U*)b);
                
                                /* ÿ0.1s�ɼ�����һ�� �Ҷ���Ϣ */
                OSTimeDlyHMSM(0u, 0u, 0u, 100,
                              OS_OPT_TIME_HMSM_STRICT | OS_OPT_TIME_PERIODIC,/* ����ģʽ */
                              &os_err);
            } 
            
        }
        OSTimeDlyHMSM(0u, 0u, 0u, 100,
              OS_OPT_TIME_HMSM_STRICT | OS_OPT_TIME_PERIODIC,/* ����ģʽ */
              &os_err);
        TRACE_DBG(">>DBG:       �Ҷȴ���\r\n");
        
        BSP_Led2Toggle();
    }
}
