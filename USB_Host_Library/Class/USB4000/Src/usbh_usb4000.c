//==================================================================================
//| �ļ����� | usbh_usb4000.c
//|----------|----------------------------------------------------------------------
//| �ļ����� | USB4000�����ǲ���
//|----------|----------------------------------------------------------------------
//| ������� | ��
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//==================================================================================

/* Includes ------------------------------------------------------------------*/
#include "usbh_usb4000.h"


#define     DEF_USB4000_DBG_EN           TRUE

#if (DEF_USB4000_DBG_EN == TRUE)
    #define USB4000_DBG(...)            do {                                \
                                            OS_ERR os_err;                  \
                                            OSSchedLock(&os_err);           \
                                            printf(__VA_ARGS__);            \
                                            printf("\r\n");            \
                                            OSSchedUnlock(&os_err);         \
                                        }while(0)
#else
    #define USB4000_DBG(...)
#endif


static USBH_StatusTypeDef USBH_USB4000_InterfaceInit  (USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef USBH_USB4000_InterfaceDeInit  (USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef USBH_USB4000_Process(USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef USBH_USB4000_ClassRequest (USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef USBH_USB4000_SOFProcess(USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef USBH_USB4000_Init(USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef USBH_USB4000_SetTriggerMode(USBH_HandleTypeDef *phost,INT8U TriggerMode);

static USBH_StatusTypeDef USBH_USB4000_SetIntegralTime(USBH_HandleTypeDef *phost,INT32U ITime);

static USBH_StatusTypeDef USBH_USB4000_GetStatus(USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef USBH_USB4000_GetSpectrum(USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef USBH_USB4000_GetInformation(USBH_HandleTypeDef *phost,INT8U Cmd,INT8U* auc_Buff);

static USBH_StatusTypeDef USBH_USB4000_QueryInformation(USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef USBH_USB4000_ProcessSpectrum(USBH_HandleTypeDef *phost);

USBH_ClassTypeDef  USB4000_Class = 
{
  "USB4000",
  USB_USB4000_CLASS,
  USBH_USB4000_InterfaceInit,
  USBH_USB4000_InterfaceDeInit,
  USBH_USB4000_ClassRequest,
  USBH_USB4000_Process,
  USBH_USB4000_SOFProcess,
};

static uint8_t auc_SerialNumber[30]={0};

//#pragma location = (0x68000000)        
static int16_t spa_buff[3840]={0};

//#pragma location = (0x68002000)
static int32_t  sum_spa[3840] = {0};

//#pragma location = (0x68006000)
static double   wavelenth_buff[3840]={0};

//#pragma location = (0x6800E000 + 0x8000)    //0x6800A000
static double   process_spa[3840] = {0};
    
USB4000_HandleTypeDef   USB4000 = {

    0,//              OutPipe1;
    0,//              OutEp1; 
    0,//              InPipe1; 
    0,//              InEp1;
    0,//              InPipe2; 
    0,//              InEp2;
    0,//              InPipe6; 
    0,//              InEp6;
    FALSE,//          sof_signal;
    0,//              rx_count;
    FALSE,//          b_SetFlag;
    10000,//              ul_SetIntegralTime;
    auc_SerialNumber,
    {0,0,0,0,0,0,0,0,0,0,0},//auin_EdcIndexs[11];             
    {0,0,0,0},              //alf_WlcCoeff[4];
    {0,0,0,0,0,0,0,0},      //alf_NlcCoeff[8];  
    0,//              uch_NlcOrder;
    spa_buff,//       pin_Spectrum;           
    sum_spa,//        pl_SumSpectrum;             
    process_spa,//    plf_ProcessSpectrum;    
    wavelenth_buff,// plf_WaveLenth;          
    0,//              uin_Pixels;             
    0,//              uch_ScansToAverage;     
    0,//              uch_ScansConut;         
    0,//              uch_Boxcar;             
    0,//              ul_IntegralTime;               
    FALSE,//          b_EdcEnable;            
    FALSE,//          b_NlcEnable;            
         
    FALSE,//          b_Open;
    FALSE,//          b_IsConnect;
    FALSE,//          b_HighSpeed;  
};

//==================================================================================
//| �������� | USBH_USB4000_InterfaceInit
//|----------|----------------------------------------------------------------------
//| �������� | USB4000�����ǽӿڳ�ʼ�� �����ڴ� ����˵�
//|----------|----------------------------------------------------------------------
//| ������� | phost�� �������
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
static USBH_StatusTypeDef USBH_USB4000_InterfaceInit (USBH_HandleTypeDef *phost)
{	  
    USB4000_HandleTypeDef* USB4000_Handle = NULL;
    uint8_t max_ep = 0;
    uint8_t num = 0;
    
    max_ep = ( (phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].bNumEndpoints <= USBH_MAX_NUM_ENDPOINTS) ? 
              phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].bNumEndpoints :
                  USBH_MAX_NUM_ENDPOINTS); 
    
    USBH_UsrLog ("EndPoint Num = %x.", max_ep);

    //ע�������ݵ�����
    phost->pActiveClass->pData = &USB4000;//(USB4000_HandleTypeDef *)USBH_malloc (sizeof(USB4000_HandleTypeDef));  
    USB4000_Handle =  (USB4000_HandleTypeDef *) phost->pActiveClass->pData; 
    
    /*�����õĶ˵�����*/
    /*�ڽӿ��������в���EPs������*/
    /*ѡ��ϵ͵����֣����ⳬ���ѷ���Ļ�����*/
    /*����˵������������� �ӽӿ��������ж˵��ַ */
    for ( ;num < max_ep; num++)
    {
        uint8_t bEndpointAddress = phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[num].bEndpointAddress;            
        uint16_t len;
        uint8_t poll;
        uint8_t* ref_ep;
        uint8_t* ref_pipe;
            
        //USBH_UsrLog ("bEndpointAddress = %x.", bEndpointAddress);

        if(bEndpointAddress & 0x80)
        {
            switch (bEndpointAddress)
            {
            case 0x81:  /* �˵�1��������˵� */
                ref_ep = &USB4000_Handle->InEp1;
                ref_pipe = &USB4000_Handle->InPipe1;
                break;  
            case 0x82:  /* �˵�2��������˵� */
                ref_ep = &USB4000_Handle->InEp2;
                ref_pipe = &USB4000_Handle->InPipe2;
                break;
            case 0x86:  /* �˵�6��������˵� */
                ref_ep = &USB4000_Handle->InEp6;
                ref_pipe = &USB4000_Handle->InPipe6;
                break;
            default:
                break;
            }
           
            *ref_ep = phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[num].bEndpointAddress;
            *ref_pipe = USBH_AllocPipe(phost, *ref_ep);
            
            len     = phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[num].wMaxPacketSize;
            poll    = phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[num].bInterval ;
            
            /* Open pipe for IN endpoint */
            USBH_OpenPipe  (phost,
                            *ref_pipe,
                            *ref_ep,
                            phost->device.address,
                            phost->device.speed,
                            USB_EP_TYPE_BULK,
                            len); 

            USBH_LL_SetToggle (phost, *ref_pipe, 1);

            USBH_UsrLog ("InEp addr = %x pipe = %x len = %x poll = %x", *ref_ep,*ref_pipe,len,poll);
            
        }
        else
        {
            ref_ep = &USB4000_Handle->OutEp1;
            ref_pipe = &USB4000_Handle->OutPipe1;
          
            *ref_ep = (phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[num].bEndpointAddress);
            *ref_pipe = USBH_AllocPipe(phost, *ref_ep);

            len     = phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[num].wMaxPacketSize;
            poll    = phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[num].bInterval ;          
            
            /* Open pipe for OUT endpoint  */
            USBH_OpenPipe  (phost,
                            *ref_pipe,
                            *ref_ep,                            
                            phost->device.address,
                            phost->device.speed,
                            USB_EP_TYPE_BULK,
                            len); 
           
            USBH_LL_SetToggle (phost,*ref_pipe, 1);
            
            USBH_UsrLog ("OutEp addr = %x pipe = %x len = %x poll = %x", *ref_ep,*ref_pipe,len,poll);
        }

    }
    
    return USBH_OK;
}

//==================================================================================
//| �������� | USBH_USB4000_InterfaceDeInit
//|----------|----------------------------------------------------------------------
//| �������� | USB4000�����ǽӿڷ���ʼ�� �ͷ��ڴ� ����˵�
//|----------|----------------------------------------------------------------------
//| ������� | phost�� �������
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
USBH_StatusTypeDef USBH_USB4000_InterfaceDeInit (USBH_HandleTypeDef *phost)
{
    USB4000_HandleTypeDef *USB4000_Handle =  (USB4000_HandleTypeDef *) phost->pActiveClass->pData; 

    // �ͷŸ��ֹܵ� 
    if(USB4000_Handle->OutPipe1 != 0)
    {
        USBH_ClosePipe  (phost, USB4000_Handle->OutPipe1);
        USBH_FreePipe  (phost, USB4000_Handle->OutPipe1);
        USB4000_Handle->OutPipe1 = 0;     /* Reset the pipe as Free */  
    }

    if(USB4000_Handle->InPipe1 != 0)
    {
        USBH_ClosePipe  (phost, USB4000_Handle->InPipe1);
        USBH_FreePipe  (phost, USB4000_Handle->InPipe1);
        USB4000_Handle->InPipe1 = 0;     /* Reset the pipe as Free */  
    }

    if(USB4000_Handle->InPipe2 != 0)
    {
        USBH_ClosePipe  (phost, USB4000_Handle->InPipe2);
        USBH_FreePipe  (phost, USB4000_Handle->InPipe2);
        USB4000_Handle->InPipe2 = 0;     /* Reset the pipe as Free */  
    }

    if(USB4000_Handle->InPipe6 != 0)
    {
        USBH_ClosePipe  (phost, USB4000_Handle->InPipe6);
        USBH_FreePipe  (phost, USB4000_Handle->InPipe6);
        USB4000_Handle->InPipe6 = 0;     /* Reset the pipe as Free */  
    }

    /* �ͷ������� */
    if(phost->pActiveClass->pData)
    {
        //USBH_free (phost->pActiveClass->pData);
        phost->pActiveClass->pData = NULL;
    }
    return USBH_OK;
}

//==================================================================================
//| �������� | USBH_USB4000_ClassRequest
//|----------|----------------------------------------------------------------------
//| �������� | ��׼Э���ʼ�� USB4000���Զ����豸����Ҫ
//|----------|----------------------------------------------------------------------
//| ������� | phost�� �������
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
static USBH_StatusTypeDef USBH_USB4000_ClassRequest (USBH_HandleTypeDef *phost)
{   
    /* ��׼��Э������ ��ȡ����������  �Զ�����豸Ӧ�ò��ð� */
    USBH_UsrLog ("ClassRequest");
    return USBH_OK; 
}

static USBH_StatusTypeDef USBH_USB4000_Delay(void)
{
    //for(int i = 0; i < 15; i ++){}
    //Bsp_DelayUs(1);
    return USBH_OK; 
}

//==================================================================================
//| �������� | USBH_USB4000_Process
//|----------|----------------------------------------------------------------------
//| �������� | USB4000����
//|----------|----------------------------------------------------------------------
//| ������� | phost�� �������
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
static USBH_StatusTypeDef USBH_USB4000_Process (USBH_HandleTypeDef *phost)
{
    OS_ERR os_err;
  
    USB4000_HandleTypeDef *USB4000_Handle =  (USB4000_HandleTypeDef *) phost->pActiveClass->pData; 

    USB4000_Handle->b_First = TRUE;
    USB4000_DBG (">>USBH_DBG:   �����ǳ�ʼ��");
    USB4000_Handle->e_State = USB4000_INIT;
    
    

    while(phost->gState == HOST_CLASS)
    {  
        switch(USB4000_Handle->e_State)
        {
            //��ʼ�������� ����ȡ����
        case USB4000_INIT:
                while(USBH_USB4000_Init(phost) != USBH_OK)
                {
                    OSTimeDlyHMSM(0u, 0u, 0u, 10u,
                                  OS_OPT_TIME_HMSM_STRICT ,
                                  &os_err);  
                    if(phost->gState != HOST_CLASS)
                    {
                        USB4000_Handle->e_State = USB4000_DISCONNECT;
                        return USBH_FAIL;
                    }
                }
                
                USBH_ClrFeature(phost,USB4000_Handle->OutEp1);
                USBH_ClrFeature(phost,USB4000_Handle->InEp1);
                USBH_ClrFeature(phost,USB4000_Handle->InEp2);
                USBH_ClrFeature(phost,USB4000_Handle->InEp6);
                OSTimeDlyHMSM(0u, 0u, 5u,0u,
                              OS_OPT_TIME_HMSM_STRICT ,
                              &os_err); 
                USB4000_DBG (">>USBH_DBG:   ��ȡ������״̬");
                while(USBH_USB4000_GetStatus(phost) != USBH_OK)
                {
                    OSTimeDlyHMSM(0u, 0u, 0u, 10u,
                                  OS_OPT_TIME_HMSM_STRICT ,
                                  &os_err);   
                    if(phost->gState != HOST_CLASS)
                    {
                        USB4000_Handle->e_State = USB4000_DISCONNECT;
                        return USBH_FAIL;
                    }
                }

                USB4000_DBG (">>USBH_DBG:   ��ȡ�����ǲ���");
                
                while(USBH_USB4000_QueryInformation(phost) != USBH_OK)
                {
                    OSTimeDlyHMSM(0u, 0u, 0u, 10u,
                                  OS_OPT_TIME_HMSM_STRICT ,
                                  &os_err);   
                }            
/*        
                for(INT16U i = 0; i < 16; i++)
                {
                    while(USBH_USB4000_GetInformation(phost,i,NULL) != USBH_OK)
                    {
                        OSTimeDlyHMSM(0u, 0u, 0u, 10u,
                                      OS_OPT_TIME_HMSM_STRICT ,
                                      &os_err);   
                    }
                }
*/   
                USB4000_Handle->e_State = USB4000_CONFIGURE;
                break;
            //��ʼ�������� ���ù����ǲ���
            case USB4000_CONFIGURE:
                USB4000_DBG (">>USBH_DBG:   ���ô���ģʽ");
                while(USBH_USB4000_SetTriggerMode(phost,0) != USBH_OK)
                {
                    OSTimeDlyHMSM(0u, 0u, 0u, 10u,
                                  OS_OPT_TIME_HMSM_STRICT ,
                                  &os_err);  
                                  
                    if(phost->gState != HOST_CLASS)
                    {
                        USB4000_Handle->e_State = USB4000_DISCONNECT;
                        return USBH_FAIL;
                    }              
                }
                USB4000_DBG (">>USBH_DBG:   ���û���ʱ��");
                while(USBH_USB4000_SetIntegralTime(phost,USB4000_Handle->ul_SetIntegralTime) != USBH_OK)
                //while(USBH_USB4000_SetIntegralTime(phost,10000) != USBH_OK)
                {
                    OSTimeDlyHMSM(0u, 0u, 0u, 10u,
                                  OS_OPT_TIME_HMSM_STRICT ,
                                  &os_err);   
                                  
                     if(phost->gState != HOST_CLASS)
                    {
                        USB4000_Handle->e_State = USB4000_DISCONNECT;
                        return USBH_FAIL;
                    }              
                }
                
                OSTimeDlyHMSM(0u, 0u, 1u,0u,
                              OS_OPT_TIME_HMSM_STRICT ,/* ����ģʽ */
                              &os_err); 
              
                USB4000_DBG (">>USBH_DBG:   ��ȡ������״̬");
                while(USBH_USB4000_GetStatus(phost) != USBH_OK)
                {
                    OSTimeDlyHMSM(0u, 0u, 0u, 10u,
                                  OS_OPT_TIME_HMSM_STRICT ,
                                  &os_err);

                    if(phost->gState != HOST_CLASS)
                    {
                        USB4000_Handle->e_State = USB4000_DISCONNECT;
                        return USBH_FAIL;
                    }            
                }

                //USBH_ClrFeature(phost,USB4000_Handle->OutEp1);
                //USBH_ClrFeature(phost,USB4000_Handle->InEp1);
                //USBH_ClrFeature(phost,USB4000_Handle->InEp2);
                //USBH_ClrFeature(phost,USB4000_Handle->InEp6);
                OSTimeDlyHMSM(0u, 0u, 1u, 00u,
                              OS_OPT_TIME_HMSM_STRICT ,
                              &os_err); 

                USB4000_DBG (">>USBH_DBG:   ��ʼ��ȡ����");               
                USB4000_Handle->e_State = USB4000_CONNECT;              
                break;
            //��ȡ����
            case USB4000_CONNECT:
                USBH_USB4000_GetSpectrum(phost);
                break;
            
        }

    }
    
    USB4000_DBG ("Exit ClassProcess");

    return USBH_OK;
}

//==================================================================================
//| �������� | USBH_USB4000_SOFProcess
//|----------|----------------------------------------------------------------------
//| �������� | USB4000֡��ʼ�źŴ���
//|----------|----------------------------------------------------------------------
//| ������� | phost�� �������
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
static USBH_StatusTypeDef USBH_USB4000_SOFProcess(USBH_HandleTypeDef *phost)
{
    //USBH_UsrLog("USBH_USB4000_SOFProcess\r\n");
    USB4000_HandleTypeDef *USB4000_Handle =  (USB4000_HandleTypeDef *) phost->pActiveClass->pData; 
    
    USB4000_Handle->sof_signal = TRUE;
    
    return USBH_OK;
}

//==================================================================================
//| �������� | USBH_USB4000_Init
//|----------|----------------------------------------------------------------------
//| �������� | USB4000 ��ʼ��
//|----------|----------------------------------------------------------------------
//| ������� | phost�� �������
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
static USBH_StatusTypeDef USBH_USB4000_Init(USBH_HandleTypeDef *phost)
{
    USB4000_HandleTypeDef *USB4000_Handle =  (USB4000_HandleTypeDef *) phost->pActiveClass->pData; 
    USBH_URBStateTypeDef URB_Status = USBH_URB_IDLE;
    INT8U uc_SendBuff[1];
    uc_SendBuff[0] = 0x01;
    USBH_BulkSendData(phost,uc_SendBuff,1,USB4000_Handle->OutPipe1,1);
    
    while(URB_Status != USBH_URB_DONE)
    {
        URB_Status = USBH_LL_GetURBState(phost, USB4000_Handle->OutPipe1);    
    }
    USBH_UsrLog ("USBH_URB_DONE");
    return USBH_OK;   
}

//==================================================================================
//| �������� | USBH_USB4000_SetIntegralTime
//|----------|----------------------------------------------------------------------
//| �������� | USB4000 ���û���ʱ��
//|----------|----------------------------------------------------------------------
//| ������� | phost�� �������
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
static USBH_StatusTypeDef USBH_USB4000_SetIntegralTime(USBH_HandleTypeDef *phost,INT32U IntegTime)
{
    USB4000_HandleTypeDef *USB4000_Handle =  (USB4000_HandleTypeDef *) phost->pActiveClass->pData; 
    USBH_URBStateTypeDef URB_Status = USBH_URB_IDLE;
    INT8U uc_SendBuff[5]; 
    INT16U uc_ErrCnt = 0; 
    
    uc_SendBuff[0] = 0x02;
    Bsp_CnvINT32UToArr(&uc_SendBuff[1],IntegTime,FALSE);
    
    /* ��SOF�ź�ͬ�� �����ʱ1ms 
    USB4000_Handle->sof_signal = FALSE;
    while(USB4000_Handle->sof_signal == TRUE){}*/

    if(USBH_BulkSendData(phost,uc_SendBuff,5,USB4000_Handle->OutPipe1,1)==USBH_FAIL)
    {
        USBH_UsrLog ("USBH_BulkSendData USBH_FAIL");
    }

    while(URB_Status != USBH_URB_DONE)
    {
        USBH_USB4000_Delay();
        URB_Status = USBH_LL_GetURBState(phost, USB4000_Handle->OutPipe1);
        if(++uc_ErrCnt> 1000)
        {
            USBH_UsrLog ("USBH_URB_NOTREADY");
            return USBH_FAIL;
        }
    }
    
    return USBH_OK;
}

//==================================================================================
//| �������� | USBH_USB4000_SetTriggerMode
//|----------|----------------------------------------------------------------------
//| �������� | USB4000 ���ô���ģʽ
//|----------|----------------------------------------------------------------------
//| ������� | phost�� �������
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
static USBH_StatusTypeDef USBH_USB4000_SetTriggerMode(USBH_HandleTypeDef *phost,INT8U TriggerMode)
{
    USB4000_HandleTypeDef *USB4000_Handle =  (USB4000_HandleTypeDef *) phost->pActiveClass->pData; 
    USBH_URBStateTypeDef URB_Status = USBH_URB_IDLE;
    INT8U uc_SendBuff[3]; 
    INT16U uc_ErrCnt = 0; 
    
    uc_SendBuff[0] = 0x0a;
    uc_SendBuff[1] = TriggerMode;
    uc_SendBuff[2] = 0x00;   
    /* ��SOF�ź�ͬ�� �����ʱ1ms 
    USB4000_Handle->sof_signal = FALSE;
    while(USB4000_Handle->sof_signal == TRUE){}*/

    if(USBH_BulkSendData(phost,uc_SendBuff,3,USB4000_Handle->OutPipe1,1)==USBH_FAIL)
    {
        USBH_UsrLog ("USBH_BulkSendData USBH_FAIL");
    }

    while(URB_Status != USBH_URB_DONE)
    {
        USBH_USB4000_Delay();
        URB_Status = USBH_LL_GetURBState(phost, USB4000_Handle->OutPipe1);
        if(++uc_ErrCnt> 1000)
        {
            USBH_UsrLog ("USBH_URB_NOTREADY");
            return USBH_FAIL;
        }
    }
    
    return USBH_OK;
}


//==================================================================================
//| �������� | USBH_USB4000_GetStatus
//|----------|----------------------------------------------------------------------
//| �������� | USB4000 ����״̬
//|----------|----------------------------------------------------------------------
//| ������� | phost�� �������
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
static USBH_StatusTypeDef USBH_USB4000_GetStatus(USBH_HandleTypeDef *phost)
{
    USB4000_HandleTypeDef *USB4000_Handle =  (USB4000_HandleTypeDef *) phost->pActiveClass->pData; 
    USBH_URBStateTypeDef URB_Status = USBH_URB_IDLE;
    INT8U uc_SendBuff[1];
    INT8U auc_Buff[512]={0};
    INT16U uc_ErrCnt = 0; 
    
    //SysTick->CTRL &= (~SysTick_CTRL_ENABLE_Msk);
    
    uc_SendBuff[0] = 0xfe;
    
    /* ��SOF�ź�ͬ�� �����ʱ1ms 
    USB4000_Handle->sof_signal = FALSE;
    while(USB4000_Handle->sof_signal == TRUE){}*/
    //Bsp_IntDis();
    USBH_BulkSendData(phost,uc_SendBuff,1,USB4000_Handle->OutPipe1,1);
    //Bsp_IntEn();
    while(URB_Status != USBH_URB_DONE)
    {
        //USBH_USB4000_Delay();
        //Bsp_IntDis();
        //__NOP();
        URB_Status = USBH_LL_GetURBState(phost, USB4000_Handle->OutPipe1);
        //Bsp_IntEn();
        if(++uc_ErrCnt> 3)
        {
            uc_ErrCnt = 0;
            USBH_UsrLog ("OUTEP1 USBH_URB_NOTREADY");
            USBH_BulkSendData(phost,uc_SendBuff,1,USB4000_Handle->OutPipe1,1);
            //return USBH_FAIL;
        }
    }
    URB_Status = USBH_URB_IDLE;
    
    //Bsp_IntDis();
    USBH_BulkReceiveData(phost, auc_Buff, 16, USB4000_Handle->InPipe1);
    //Bsp_IntEn();
    while(URB_Status != USBH_URB_DONE)
    {
        //USBH_USB4000_Delay();
        //Bsp_IntDis();
        //__NOP();
        URB_Status = USBH_LL_GetURBState(phost, USB4000_Handle->InPipe1);
        //Bsp_IntDis();
        if(++uc_ErrCnt> 50)
        {
            uc_ErrCnt = 0;
            USBH_UsrLog ("INEP1 USBH_URB_NOTREADY");
            //USBH_BulkReceiveData(phost, auc_Buff, 512, USB4000_Handle->InPipe1);
            //SysTick->CTRL |= (SysTick_CTRL_ENABLE_Msk);
            return USBH_FAIL;
        }
    }
    USBH_UsrLog ("USBH_URB_DONE");

    USB4000_Handle->uin_Pixels = 3648;
    USB4000_Handle->ul_IntegralTime = Bsp_CnvArrToINT32U(&auc_Buff[2],FALSE);
    USB4000_Handle->b_HighSpeed = (auc_Buff[14] == 0x80)? TRUE:FALSE;
    
    USBH_UsrLog ("Pixels = %u",Bsp_CnvArrToINT16U(&auc_Buff[0],FALSE));
    USBH_UsrLog ("Trigger Mode = %u",auc_Buff[7]);
    USBH_UsrLog ("IntegralTime = %u",USB4000_Handle->ul_IntegralTime);
    USBH_UsrLog ("Packets In Spectra = %u",auc_Buff[9]);
    USBH_UsrLog ("Packet Count = %u",auc_Buff[11]);
    USBH_UsrLog ("USBH_URB_DONE");
    //SysTick->CTRL |= (SysTick_CTRL_ENABLE_Msk);
    return USBH_OK;
}

//==================================================================================
//| �������� | USBH_USB4000_GetInformation
//|----------|----------------------------------------------------------------------
//| �������� | USB4000 ��ȡһ����Ϣ
//|----------|----------------------------------------------------------------------
//| ������� | phost�� �������
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
static USBH_StatusTypeDef USBH_USB4000_GetInformation(USBH_HandleTypeDef *phost,INT8U Cmd,INT8U* string)
{
    USB4000_HandleTypeDef *USB4000_Handle =  (USB4000_HandleTypeDef *) phost->pActiveClass->pData; 
    USBH_URBStateTypeDef URB_Status = USBH_URB_IDLE;
    INT8U uc_SendBuff[2];
    INT8U auc_Buff[512]={0};
    INT16U uc_ErrCnt = 0; 
    
    uc_SendBuff[0] = 0x05;
    uc_SendBuff[1] = Cmd;
    
    USBH_BulkSendData(phost,uc_SendBuff,2,USB4000_Handle->OutPipe1,1);
    
    while(URB_Status != USBH_URB_DONE)
    {
        USBH_USB4000_Delay();
        URB_Status = USBH_LL_GetURBState(phost, USB4000_Handle->OutPipe1);
        if(++uc_ErrCnt> 3)
        {
            USBH_UsrLog ("USBH_URB_NOTREADY");
            return USBH_FAIL;
        }
    }
    URB_Status = USBH_URB_IDLE;
    
    USBH_BulkReceiveData(phost, auc_Buff, 64, USB4000_Handle->InPipe1);
    
    while(URB_Status != USBH_URB_DONE)
    {
        USBH_USB4000_Delay();
        URB_Status = USBH_LL_GetURBState(phost, USB4000_Handle->InPipe1);
        if(++uc_ErrCnt> 1000)
        {
            USBH_UsrLog ("USBH_URB_NOTREADY");
            return USBH_FAIL;
        }
    }
    
    if(auc_Buff[0] == uc_SendBuff[0] && auc_Buff[1] == uc_SendBuff[1])
    {
        if(string != NULL)
        {
            memcpy(string,&auc_Buff[2],62);
        }
        USBH_UsrLog ("������Ϣ��� %u %s",Cmd,&auc_Buff[2]);
        return USBH_OK;
    }
    else
    {
        return USBH_FAIL;
    }
}
//==================================================================================
//| �������� | USBH_USB4000_QueryInformation
//|----------|----------------------------------------------------------------------
//| �������� | USB4000 ��ȡ���е���Ϣ
//|----------|----------------------------------------------------------------------
//| ������� | phost�� �������
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
static USBH_StatusTypeDef USBH_USB4000_QueryInformation(USBH_HandleTypeDef *phost)
{
    USB4000_HandleTypeDef *USB4000_Handle =  (USB4000_HandleTypeDef *) phost->pActiveClass->pData; 
    static INT8U   aauc_Str[64] = {0};
      
    for(int i = 0; i < 16; )
    {
        if(USBH_OK == USBH_USB4000_GetInformation(phost,i,&aauc_Str[0]))
        {
            switch (i)
            {
            case 0: //�ַ������к�
                memcpy(USB4000_Handle->puc_SerialNumber,(char const*)&aauc_Str[0],20);
                break;
            case 1: //����У������ Wavelength Calibration Coefficient
            case 2:
            case 3:
            case 4:
                USB4000_Handle->alf_WlcCoeff[i-1] = atof((char const*)&aauc_Str[0]);
                break;
            case 5: //��ɢ�ⳣ�� Stray light constant
                break;
            case 6: //�����Բ������� non-linearity correction coefficient
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:              
            case 12:                
            case 13: 
                  USB4000_Handle->alf_NlcCoeff[i-6] = atof((char const*)&aauc_Str[0]);
                  break;  
                  
            case 14: //�����Բ����� Polynomial order of non-linearity calibration
                  USB4000_Handle->uch_NlcOrder = atoi((char const*)&aauc_Str[0]);
                  break;          
            }
          
            //printf("��Ϣ%u = ",i);
            //printf((const char*)&aauc_Str[2]);
            //printf("\r\n");
            i++;
        }
        else
        {
            /*
            OS_ERR os_err;
            OSTimeDlyHMSM(0u, 0u, 0u, 100,
                        OS_OPT_TIME_HMSM_STRICT , 
                        &os_err);
            */
        }
        
        if(phost->gState != HOST_CLASS)
            break;
    }
    return USBH_OK;
}

//==================================================================================
//| �������� | USBH_USB4000_GetSpectrum
//|----------|----------------------------------------------------------------------
//| �������� | USB4000 ��ȡһ�Ź��ײ�����
//|----------|----------------------------------------------------------------------
//| ������� | phost�� �������
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
static USBH_StatusTypeDef USBH_USB4000_GetSpectrum(USBH_HandleTypeDef *phost)
{  
    USB4000_HandleTypeDef *USB4000_Handle =  (USB4000_HandleTypeDef *) phost->pActiveClass->pData; 
    uint8_t* puc_buff = (uint8_t*)USB4000_Handle->pin_Spectrum;
    USBH_URBStateTypeDef URB_Status = USBH_URB_IDLE;
    INT8U uc_SendBuff[1];
    INT8U auc_Buff[512]={0};
    INT16U uc_ErrCnt = 0; 
    INT16U i,j;
    OS_ERR os_err;

    
    uc_SendBuff[0] = 0x09;
    USBH_BulkSendData(phost,uc_SendBuff,1,USB4000_Handle->OutPipe1,1); 
    while(URB_Status != USBH_URB_DONE)
    {
;
        URB_Status = USBH_LL_GetURBState(phost, USB4000_Handle->OutPipe1);
        if(++uc_ErrCnt> 3)
        {
            //USBH_UsrLog ("EP1 SEND FAIL = %u",URB_Status);
            USBH_BulkSendData(phost,uc_SendBuff,1,USB4000_Handle->OutPipe1,1);
            uc_ErrCnt = 0;
        }
    }
    
    //USBH_USB4000_ProcessSpectrum(phost);
    i = USB4000_Handle->ul_SetIntegralTime /1000 - 6;
    OSTimeDlyHMSM(0u, 0u, 0u,i,
                  OS_OPT_TIME_HMSM_STRICT ,
                  &os_err);
    
/*   
    OSTimeDlyHMSM(0u, 0u, 0u,6u,
                  OS_OPT_TIME_HMSM_STRICT ,
                  &os_err);  
*/ 
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, (GPIO_PinState)1);

    
    for(i = 0 ; i < 4; i++)
    {
        uc_ErrCnt = 0;
        URB_Status = USBH_URB_IDLE;
        
        USBH_BulkReceiveData(phost, auc_Buff, 512, USB4000_Handle->InPipe6); 
        
        do
        {
            URB_Status = USBH_LL_GetURBState(phost, USB4000_Handle->InPipe6);
            if(++uc_ErrCnt> 1000)
            {
                USBH_UsrLog ("EP6 RECV FAIL = %u",URB_Status);
                uc_ErrCnt = 0;
                break;
            }
        }
        while(URB_Status != USBH_URB_DONE);
        for(j = 0; j < 512;j++)
        {
            puc_buff[i*512 + j] = auc_Buff[i];
        }
        USBH_UsrLog ("R %u",i);
    }
    
    for(i = 4 ; i < 16; i++)
    {
        uc_ErrCnt = 0;
        URB_Status = USBH_URB_IDLE;
        
        USBH_BulkReceiveData(phost, auc_Buff, 512, USB4000_Handle->InPipe2);  
        
        do
        {
            URB_Status = USBH_LL_GetURBState(phost, USB4000_Handle->InPipe2);
            if(++uc_ErrCnt> 1000)
            {
                USBH_UsrLog ("EP2 RECV FAIL = %u",URB_Status);
                uc_ErrCnt = 0;
                break;
            }
        }
        while(URB_Status != USBH_URB_DONE);

        if(((i == 15) || (USB4000_Handle->b_First == TRUE && i == 14)) && auc_Buff[0] == 0x69)
        {
            USB4000_Handle->b_First = FALSE;
            USBH_UsrLog ("���׽��ܳɹ�");
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, (GPIO_PinState)0);
            
            USBH_USB4000_ProcessSpectrum(phost);
            //OSTimeDlyHMSM(0u, 0u, 0u,6u,
            //              OS_OPT_TIME_HMSM_STRICT ,
            //              &os_err); 
            return USBH_OK;
        }
        else
        {
            for(j = 0; j < 512;j++)
            {
                puc_buff[i*512 + j] = auc_Buff[i];
            } 
        }
        //USBH_UsrLog ("R %u",i);
    }
    
    USBH_UsrLog ("���׽���ʧ��");
    USBH_UsrLog ("auc_Buff[0] = %x",auc_Buff[0]);
    return USBH_FAIL;
}
    


//==================================================================================
//| �������� | USBH_USB4000_ProcessSpectrum
//|----------|----------------------------------------------------------------------
//| �������� | USB4000 ����һ�Ź���
//|----------|----------------------------------------------------------------------
//| ������� | phost�� �������
//|----------|----------------------------------------------------------------------
//| ���ز��� | ��
//|----------|----------------------------------------------------------------------
//| ������� | wjb
//==================================================================================
static USBH_StatusTypeDef USBH_USB4000_ProcessSpectrum(USBH_HandleTypeDef *phost)
{
    USB4000_HandleTypeDef *USB4000_Handle =  (USB4000_HandleTypeDef *) phost->pActiveClass->pData;
    uint16_t i = 0;
    uint8_t ready = 0;
    
    
    /* ���ɨ��ƽ�� */
    if (USB4000_Handle->uch_ScansToAverage > 1)
    {
        /* �ۼƹ��� */
        for(i = 0; i < USB4000_Handle->uin_Pixels; i++)
        {
            USB4000_Handle->pl_SumSpectrum[i] += USB4000_Handle->pin_Spectrum[i];
        }
      
        if(++USB4000_Handle->uch_ScansConut >= USB4000_Handle->uch_ScansToAverage)
        {
            USB4000_Handle->uch_ScansConut = 0;
            
            /* ��ƽ�� */
            for(i = 0; i < USB4000_Handle->uin_Pixels; i++)
            {
                USB4000_Handle->plf_ProcessSpectrum[i] = ((double)USB4000_Handle->pl_SumSpectrum[i]) / 
                                                          USB4000_Handle->uch_ScansToAverage;
            }
            
            /* ������� */
            for(i = 0; i < USB4000_Handle->uin_Pixels; i++)
            {
                USB4000_Handle->pl_SumSpectrum[i] = 0;
            }
            
            ready = 1;
        }
    }
    else 
    {
        USB4000_Handle->uch_ScansConut = 0;

        for(i = 0; i < USB4000_Handle->uin_Pixels; i++)
        {
            USB4000_Handle->pl_SumSpectrum[i] = 0;
            USB4000_Handle->plf_ProcessSpectrum[i] = USB4000_Handle->pin_Spectrum[i];
        }
        
        ready = 1;
    }
    
    if( ready != 0 )
    {
        if(USB4000_Handle->b_EdcEnable == TRUE)
        {
            double value = 0.0;
            int usableDarkCount = 0;
            
            for (int i = 0; i < 11; i++)
            {
                int edcPixelIndex = USB4000_Handle->auin_EdcIndexs[i];
                if (edcPixelIndex <  USB4000_Handle->uin_Pixels)
                {
                    value += USB4000_Handle->plf_ProcessSpectrum[edcPixelIndex];
                    usableDarkCount++;
                }
            }
            
            if (usableDarkCount > 0)
            {
                value /= usableDarkCount;
                for (int i = 0; i < USB4000_Handle->uin_Pixels; i++)
                    USB4000_Handle->plf_ProcessSpectrum[i] -= value;
            }
        }
        
        /* ������У�� */
        if(USB4000_Handle->b_EdcEnable == TRUE && USB4000_Handle->b_NlcEnable == TRUE)
        {
            
        }
        
        if (USB4000_Handle->uch_Boxcar > 0)
        {
            Mod_FilterBoxCar(USB4000_Handle->plf_ProcessSpectrum , USB4000_Handle->uin_Pixels, USB4000_Handle->uch_Boxcar);
        }
        
        
        USB4000_EvnetHandle(USB4000_Handle);
          

    }
    
    return USBH_OK;
}

void USB4000_SetIntegTime(USB4000_HandleTypeDef *USB4000_Handle, INT32U IntegTime)
{
    USB4000_Handle->b_SetFlag = TRUE;
    USB4000_Handle->ul_SetIntegralTime = IntegTime;
}

__weak void USB4000_EvnetHandle(USB4000_HandleTypeDef *USB4000_Handle)
{

}


/* ������ ��ȡ����ͨ����״̬  */
/**
while(phost->gState == HOST_CLASS)
{
    USBH_URBStateTypeDef URB_Status = USBH_URB_IDLE;
    void* pv_Msg = 0;
    INT16U uin_Len = 0;
    
    pv_Msg = OSTaskQPend(100,OS_OPT_PEND_BLOCKING,&uin_Len,NULL,&os_err);
    if(os_err != OS_ERR_NONE)
    {
        USBH_UsrLog ("��ʱ");
        continue;
    }

    switch((INT32U)pv_Msg)
    {
    case USBH_PORT_EVENT:
        USBH_UsrLog ("USBH_PORT_EVENT");
        break;  
    case USBH_URB_EVENT:
        USBH_UsrLog ("USBH_URB_EVENT");
        break;
    case USBH_CONTROL_EVENT:
        USBH_UsrLog ("USBH_CONTROL_EVENT");        
        break;
    case USBH_CLASS_EVENT:
        USBH_UsrLog ("USBH_CLASS_EVENT");
        break;
    case USBH_STATE_CHANGED_EVENT:
        USBH_UsrLog ("USBH_STATE_CHANGED_EVENT");
        break;
    }
    
    URB_Status = USBH_LL_GetURBState(phost, 0);

    USBH_UsrLog ("0 = %u",URB_Status);
    
    URB_Status = USBH_LL_GetURBState(phost, 1);

    USBH_UsrLog ("1 = %u",URB_Status);
    
    URB_Status = USBH_LL_GetURBState(phost, USB4000_Handle->OutPipe1);

    USBH_UsrLog ("OutPipe1 = %u",URB_Status);
    
    URB_Status = USBH_LL_GetURBState(phost, USB4000_Handle->InPipe1);
    USBH_UsrLog ("InPipe1 = %u",URB_Status);
    
    URB_Status = USBH_LL_GetURBState(phost, USB4000_Handle->InPipe2);
    USBH_UsrLog ("InPipe2 = %u",URB_Status);
    
    URB_Status = USBH_LL_GetURBState(phost, USB4000_Handle->InPipe6);
    USBH_UsrLog ("InPipe6 = %u",URB_Status);
    
}
return USBH_OK;
*/
    

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
