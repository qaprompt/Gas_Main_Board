#include "App_Include.h"

#define     DEF_MEASURE_DBG_EN           TRUE

#if (DEF_MEASURE_DBG_EN == TRUE)
    #define MEASURE_DBG(...)            do {                                \
                                            OS_ERR os_err;                  \
                                            OSSchedLock(&os_err);           \
                                            printf(__VA_ARGS__);            \
                                            OSSchedUnlock(&os_err);         \
                                        }while(0)
#else
    #define MEASURE_DBG(...)
#endif

Measure_t st_Measure = {
    e_MeasureIdle,                  /* ����״̬ */
    10,                             /* ����ʱ�� */
    1000,                           /* ����ʱ�� */
    0,		                        /* ��Ч�� */
    DEF_SAMPLE_DOT_MAX,			    /* ��Ч�� */
    
    
    {{0},0,DEF_SAMPLE_DOT_MAX},     /* HC������ */
    {{0},0,DEF_SAMPLE_DOT_MAX},     /* NO������ */
    //{{0},0,DEF_SAMPLE_DOT_MAX},     /* CO������ */
    //{{0},0,DEF_SAMPLE_DOT_MAX},     /* CO2������ */
    
    {   
    {{0},0,DEF_SAMPLE_DOT_MAX},     /* �̶Ȳ����� */
    {{0},0,DEF_SAMPLE_DOT_MAX},
    {{0},0,DEF_SAMPLE_DOT_MAX},
    {{0},0,DEF_SAMPLE_DOT_MAX},
    {{0},0,DEF_SAMPLE_DOT_MAX},
    {{0},0,DEF_SAMPLE_DOT_MAX},
    {{0},0,DEF_SAMPLE_DOT_MAX},
    {{0},0,DEF_SAMPLE_DOT_MAX},
    {{0},0,DEF_SAMPLE_DOT_MAX},
    {{0},0,DEF_SAMPLE_DOT_MAX},
    },
    
    0.0,                /* ƽ��CO2 */
    0.0,                /* ƽ��CO */
    0.0,                /* ƽ��NO */
    0.0,                /* ƽ��HC */
    0.0,                /* ƽ���̶�ֵ */    
    /* ���Խ�� */
    {
    0,          /* ������� */
    {0},        /* ���� */
    0.0,        /* �ٶ� */
    0.0,        /* ���ٶ� */
    0.0,        /* �ȹ���  */
    0.0,        /* CO2Ũ�� */
    0.0,        /* CO1Ũ�� */
    0.0,        /* NOŨ�� */
    0.0,        /* HCŨ�� */
    0.0,        /* �̶� */
    }
};

void DoorCloseFnct (void *p_tmr,  void *p_arg);
OS_TMR CloseDoorTmr;

static void InitSem(void)
{
    OS_ERR os_err;
    OSTaskSemSet(&TaskMeasureTCB,0U,&os_err);
}

static void PostSem(void)
{
    OS_ERR os_err;
    OSTaskSemPost(&TaskMeasureTCB,OS_OPT_POST_NONE,&os_err);
}

static BOOL PendSem(INT32U ul_Time)
{
    OS_ERR os_err;
    OSTaskSemPend(ul_Time,OS_OPT_PEND_BLOCKING,NULL,&os_err);
    if (os_err != OS_ERR_NONE)
        return FALSE;
    return TRUE;
}

static void PostMsg(void* pv_Msg)
{
    OS_ERR  os_err;
    OSTaskQPost(&TaskMeasureTCB,(void*)pv_Msg,1,OS_OPT_POST_FIFO ,&os_err);
    if(os_err != OS_ERR_NONE)
    {
    }
}

static void* PendMeg(void)
{
    OS_ERR  os_err;
    INT16U  uin_Size = 0;
    void * pv_Msg;
    pv_Msg = OSTaskQPend(0,OS_OPT_PEND_BLOCKING,&uin_Size,0,&os_err);

    if(os_err == OS_ERR_NONE)
        return pv_Msg;
    else
        return 0;
}

static void StartTimeOutCheck(void)
{
    OS_ERR os_err;
    OSTmrStart(&CloseDoorTmr,&os_err);         //������ʱ���
}

static void StopTimeOutCheck(void)
{
    OS_ERR os_err;
    OSTmrStop(&CloseDoorTmr,OS_OPT_TMR_NONE,NULL,&os_err);
}


static void InitTimeOutCheck(void)
{
    OS_ERR os_err;
    OSTmrCreate(&CloseDoorTmr,          /* p_tmr */
                "MeasSpeedTimeOut",     /* p_name */
                50,                     /* dly 50* 0.1s = 5s*/
                0,                      /* period */
                OS_OPT_TMR_ONE_SHOT,    /* opt */
                DoorCloseFnct,          /* p_callback */
                0,                      /* p_callback_arg */
                &os_err);               /* p_err */   
}


static void SortBuff(FP32* pf_Buff,INT16U uin_Len)
{
	INT16U i,j;
	FP32 t;
	for(i = 0; i < uin_Len-1; i++)
	{
		for(j = i+1; j < uin_Len; j++)
		{
			if(pf_Buff[j] > pf_Buff[i])
			{
				t = pf_Buff[j];				//����������
				pf_Buff[j] = pf_Buff[i];
				pf_Buff[i] = t;
			}
		}
	}
}

static FP64 WindowFilter(FP32* pf_Buff,INT16U uin_Use,INT16U uin_InvalidDots, INT16U uin_ActiveDots)
{
    INT16U  i;
    FP64 f = 0;
    FP32 af_Buff[DEF_SAMPLE_DOT_MAX] = {0};
    
    //memcpy(af_Buff,pf_Buff,sizeof(af_Buff[DEF_SAMPLE_DOT_MAX]));
    
    for(i = 0; i < DEF_SAMPLE_DOT_MAX; i++)
    {
        af_Buff[i] = pf_Buff[i];
    }
    
    
    SortBuff(af_Buff,uin_Use);
    
    /* �����ȥn1�����ֵ֮�� ʣ��ֵ��n2����Чֵ��ƽ��ֵ */
    if (uin_Use > uin_InvalidDots)
    {
        /* �ܵ��� ������Ч���� */
        INT16U num = uin_Use - uin_InvalidDots;		//����ȥ��N1����Ч���ʣ�����Ʒ����

        if (num  > uin_ActiveDots)					//�ж�ʣ������Ƿ����N2����Ч����
            num  = uin_ActiveDots;

        for( i = 0; i <	num; i++)
            f += af_Buff[uin_InvalidDots + i];	    //���
        f /= num;								    //��ƽ��
    }
    return f;
}

void Mod_MeasureInit(Measure_t* pst_Meas)
{
    InitSem();
    InitTimeOutCheck();  
}

void Mod_MeasureDoStaticMeasure(Measure_t* pst_Meas)
{
    Mod_GreyGotoMeas(&st_Grey);                             //�̹�ȴ���ʼ����
    Mod_GasMeasureDoDiffMeasure(&st_GasMeasure);            //���⿪ʼ��ֲ��� ��ʼ���Բ���
	PostMsg((void*)e_MeasureStaticSample);	                //ֱ�ӿ�ʼ����
}

void Mod_MeasureDoDynamicMeasure(Measure_t* pst_Meas)
{
    Mod_GreyGotoIdle(&st_Grey);                             //�̹��������  
    Mod_GasMeasureDoAbsMeasure(&st_GasMeasure);             //���������ֲ��� ��ʼ���Բ���
	PostMsg((void*)e_MeasureIdle);		                    //�л�������ģʽ
}

void Mod_MeasurePoll(Measure_t* pst_Meas)
{

    INT32U s,ms;

    FP64 q0,q1,q2;
    FP64 fCO2,fCO,fHC,fNO;
    
    OS_ERR os_err;

    MeasureState_e e_Msg = (MeasureState_e)PendMeg();
    switch(e_Msg)
    {
    case e_MeasureWait:
        MEASURE_DBG(">>MEASURE DBG:   �������� ׼����ʼ����\r\n");
        
        StartTimeOutCheck();                                    //��ʼ��ʱ���      
        
        pst_Meas->e_State = e_MeasureWait;
        Mod_GreyGotoMeas(&st_Grey);                             //�̹�ȴ���ʼ����
        Mod_GasMeasureDoDiffMeasure(&st_GasMeasure);            //���⿪ʼ��ֲ��� ��ʼ���Բ���
        
        USB4000.b_WaitSync = TRUE;                              //�ȴ�ͬ��
        break;
    case e_MeasureDead: 
        MEASURE_DBG(">>MEASURE DBG:   ������ȥ ������ʱ\r\n");

        StopTimeOutCheck();                                     //ֹͣ��ʱ���
        
        pst_Meas->e_State = e_MeasureDead; 
        
        s = pst_Meas->ul_DeadTime / 1000;
        ms = pst_Meas->ul_DeadTime % 1000; 
        
        MEASURE_DBG(">>MEASURE DBG:   ����ʱ��(Ms):%d\r\n", pst_Meas->ul_DeadTime);
        OSTimeDlyHMSM(0u, 0u, s, ms,
                      OS_OPT_TIME_HMSM_STRICT,
                      &os_err);  
        
        PostMsg((void*)e_MeasureSample);
        break;

    case e_MeasureSample:
        MEASURE_DBG(">>MEASURE DBG:   ��ʼ����\r\n");
        
        pst_Meas->st_SampleHC.ul_Len = 0;
        pst_Meas->st_SampleNO.ul_Len = 0;
        //pst_Meas->st_SampleCO.ul_Len = 0;
        //pst_Meas->st_SampleCO2.ul_Len = 0;
        
        pst_Meas->st_SampleGrey[0].ul_Len = 0;
        pst_Meas->st_SampleGrey[1].ul_Len = 0;
        pst_Meas->st_SampleGrey[2].ul_Len = 0;
        pst_Meas->st_SampleGrey[3].ul_Len = 0;
        pst_Meas->st_SampleGrey[4].ul_Len = 0;
        pst_Meas->st_SampleGrey[5].ul_Len = 0;
        pst_Meas->st_SampleGrey[6].ul_Len = 0;
        pst_Meas->st_SampleGrey[7].ul_Len = 0;
        pst_Meas->st_SampleGrey[8].ul_Len = 0;
        pst_Meas->st_SampleGrey[9].ul_Len = 0;
        
        pst_Meas->e_State = e_MeasureSample;
        
        s = pst_Meas->ul_MesureTime / 1000;
        ms = pst_Meas->ul_MesureTime % 1000; 

        MEASURE_DBG(">>MEASURE DBG:   ����ʱ��(Ms):%d\r\n", pst_Meas->ul_MesureTime);
        
        USB4000.b_WaitSync = FALSE;                              //��ʼ����
        OSTaskResume(&TaskUsbHostTCB,&os_err);                   //�ָ����ײɼ� 
        
        OSTimeDlyHMSM(0u, 0u, s, ms,
                      OS_OPT_TIME_HMSM_STRICT,
                      &os_err);   

        PostMsg((void*)e_MeasureCal);
        break;
    case e_MeasureCal:
        MEASURE_DBG(">>MEASURE DBG:   ��������\r\n");
        
        pst_Meas->e_State = e_MeasureCal;
        
        Mod_GreyGotoIdle(&st_Grey);                             //�̹��������  
        Mod_GasMeasureDoAbsMeasure(&st_GasMeasure);             //���������ֲ��� ��ʼ���Բ���

        
        MEASURE_DBG(">>MEASURE DBG:================================================\r\n"); 
        
        MEASURE_DBG(">>MEASURE DBG:   ��ȡ���ٰ�����\r\n");
        do
        {
            Mod_MeasSpeedRequest(&st_MeasSpeed);                //��ȡ���ٰ������ �������ٶ� �����ٶ�
        }while(PendSem(100) != TRUE);
        MEASURE_DBG(">>MEASURE DBG:   ���ٰ��ȡ���\r\n"); 
        
        MEASURE_DBG(">>MEASURE DBG:   ��ȡCO2COƽ��Ũ��\r\n");
        //do
        //{
        //    Mod_LaserRequestGasAvg(&st_Laser);                  //������ƽ��CO2COŨ��
        //}while(PendSem(100) != TRUE);
        MEASURE_DBG(">>MEASURE DBG:   ��ȡCO2COƽ��Ũ�����\r\n");
        
        pst_Meas->lf_NO = WindowFilter (pst_Meas->st_SampleNO.af_Buff,
                                        pst_Meas->st_SampleNO.ul_Len,
                                        pst_Meas->uin_InvalidDots,
                                        pst_Meas->uin_ActiveDots);
        
        pst_Meas->lf_HC = WindowFilter (pst_Meas->st_SampleHC.af_Buff,
                                        pst_Meas->st_SampleHC.ul_Len,
                                        pst_Meas->uin_InvalidDots,
                                        pst_Meas->uin_ActiveDots);
        
#if 0        
        /* ��������Ӵ�С���� */
        SortBuff(pst_Meas->st_SampleNO.af_Buff,pst_Meas->st_SampleNO.ul_Len);
        SortBuff(pst_Meas->st_SampleHC.af_Buff,pst_Meas->st_SampleHC.ul_Len);
        for(i = 0; i < 10; i++)
        {
            SortBuff(pst_Meas->st_SampleGrey[i].af_Buff, pst_Meas->st_SampleGrey[i].ul_Len);
        }
        
        pst_Meas->lf_Grey = 0;
        
        
        /* �����ȥn1�����ֵ֮�� ʣ��ֵ��n2����Чֵ��ƽ��ֵ */
        f = 0;
        if (pst_Meas->st_SampleNO.ul_Len > pst_Meas->uin_InvalidDots)
        {
        	/* �ܵ��� ������Ч���� */
        	INT16U num = pst_Meas->st_SampleNO.ul_Len - pst_Meas->uin_InvalidDots;		//����ȥ��N1����Ч���ʣ�����Ʒ����

        	if (num  > pst_Meas->uin_ActiveDots)										//�ж�ʣ������Ƿ����N2����Ч����
        		num  = pst_Meas->uin_ActiveDots;

            for( i = 0; i <	num; i++)
            	f += pst_Meas->st_SampleNO.af_Buff[pst_Meas->uin_InvalidDots + i];	    //���
            pst_Meas->lf_NO = f / num;								                    //��ƽ��
        }

        f = 0;
        if (pst_Meas->st_SampleHC.ul_Len > pst_Meas->uin_InvalidDots)
        {
        	/* �ܵ��� ������Ч���� */
        	INT16U num = pst_Meas->st_SampleHC.ul_Len - pst_Meas->uin_InvalidDots;		//����ȥ��N1����Ч���ʣ�����Ʒ����

        	if (num  > pst_Meas->uin_ActiveDots)										//�ж�ʣ������Ƿ����N2����Ч����
        		num  = pst_Meas->uin_ActiveDots;

            for( i = 0; i <	num; i++)
            	f += pst_Meas->st_SampleHC.af_Buff[pst_Meas->uin_InvalidDots + i];	    //���
            pst_Meas->lf_HC = f / num;								                    //��ƽ��
        }
#endif      
        /* ����ȼ�շ��� ����ʵ��Ũ�� */
        q0 = pst_Meas->lf_CO / pst_Meas->lf_CO2;
        q1 = pst_Meas->lf_HC / pst_Meas->lf_CO2;
        q2 = pst_Meas->lf_NO / pst_Meas->lf_CO2;
        
        fCO2 = 42 / (2.79 + 2 * q0 + 1.21 * q1 + q2);
        fCO = fCO2 * q0;
        fHC = fCO2 * q1;
        fNO = fCO2 * q2;

        
        
        INT16U i;
        MEASURE_DBG(">>MEASURE DBG:================================================\r\n");  
        MEASURE_DBG(">>MEASURE DBG:   NO->%d��������\r\n",pst_Meas->st_SampleNO.ul_Len);
        for(i = 0; i < pst_Meas->st_SampleNO.ul_Len; i++)
            MEASURE_DBG(">>MEASURE DBG:     NO[%d]: %f\r\n", i, pst_Meas->st_SampleNO.af_Buff[i]);
        
        MEASURE_DBG(">>MEASURE DBG:   HC->%d��������\r\n",pst_Meas->st_SampleHC.ul_Len);
        for(i = 0; i < pst_Meas->st_SampleHC.ul_Len; i++)
            MEASURE_DBG(">>MEASURE DBG:     HC[%d]: %f\r\n", i, pst_Meas->st_SampleHC.af_Buff[i]);
/*        
        for (int j = 0; j < 10; j++)
        {
            MEASURE_DBG(">>MEASURE DBG:   Grey[%d]->%d��������\r\n", j, pst_Meas->st_SampleGrey[j].ul_Len);
            for(i = 0; i < pst_Meas->st_SampleGrey[j].ul_Len; i++)
                MEASURE_DBG(">>MEASURE DBG:     Grey[%d][%d]: %f\r\n", j, i, pst_Meas->st_SampleGrey[j].af_Buff[i]);        
        }    
*/
        
        MEASURE_DBG(">>MEASURE DBG:================================================\r\n"); 
        MEASURE_DBG(">>MEASURE DBG:   һ�ζ�̬�������\r\n");
        MEASURE_DBG(">>MEASURE DBG:   �����ٶ�:     %f\r\n",pst_Meas->st_Result.f_Speed);
        MEASURE_DBG(">>MEASURE DBG:   �������ٶ�:   %f\r\n",pst_Meas->st_Result.f_Acc);
        MEASURE_DBG(">>MEASURE DBG:   �����ȹ���:   %f\r\n",0.0);
        MEASURE_DBG(">>MEASURE DBG:   ƽ��CO2:      %f\r\n",pst_Meas->lf_CO2);
        MEASURE_DBG(">>MEASURE DBG:   ƽ��CO:       %f\r\n",pst_Meas->lf_CO);
        MEASURE_DBG(">>MEASURE DBG:   ƽ��NO:       %f\r\n",pst_Meas->lf_NO);
        MEASURE_DBG(">>MEASURE DBG:   ƽ��HC:       %f\r\n",pst_Meas->lf_HC);
        MEASURE_DBG(">>MEASURE DBG:   Q0:    CO/CO2 %f\r\n",q0);
        MEASURE_DBG(">>MEASURE DBG:   Q1:    HC/CO2 %f\r\n",q1);
        MEASURE_DBG(">>MEASURE DBG:   Q2:    NO/CO2 %f\r\n",q2);
        MEASURE_DBG(">>MEASURE DBG:   ����CO2:      %f\r\n",fCO2);
        MEASURE_DBG(">>MEASURE DBG:   ����CO:       %f\r\n",fCO);
        MEASURE_DBG(">>MEASURE DBG:   ����NO:       %f\r\n",fNO);
        MEASURE_DBG(">>MEASURE DBG:   ����HC:       %f\r\n",fHC);
        MEASURE_DBG(">>MEASURE DBG:================================================\r\n");

        pst_Meas->e_State = e_MeasureIdle;
        break;
    case e_MeasureTimeOut:
        /* �ָ�����ģʽ  */
        MEASURE_DBG(">>MEASURE DBG:   ��������ʱ\r\n");
        
        StopTimeOutCheck();                                     //ֹͣ��ʱ��� 
        Mod_GreyGotoIdle(&st_Grey);                             //�̹��������  
        Mod_GasMeasureDoAbsMeasure(&st_GasMeasure);           //���������ֲ��� ��ʼ���Բ���
        
        pst_Meas->e_State = e_MeasureIdle;
        break;

    case e_MeasureStaticSample:
        MEASURE_DBG(">>MEASURE DBG:   ��ʼ��̬����\r\n");
        
        pst_Meas->st_SampleHC.ul_Len = 0;
        pst_Meas->st_SampleNO.ul_Len = 0;
        
        pst_Meas->st_SampleGrey[0].ul_Len = 0;
        pst_Meas->st_SampleGrey[1].ul_Len = 0;
        pst_Meas->st_SampleGrey[2].ul_Len = 0;
        pst_Meas->st_SampleGrey[3].ul_Len = 0;
        pst_Meas->st_SampleGrey[4].ul_Len = 0;
        pst_Meas->st_SampleGrey[5].ul_Len = 0;
        pst_Meas->st_SampleGrey[6].ul_Len = 0;
        pst_Meas->st_SampleGrey[7].ul_Len = 0;
        pst_Meas->st_SampleGrey[8].ul_Len = 0;
        pst_Meas->st_SampleGrey[9].ul_Len = 0;
        
        pst_Meas->e_State = e_MeasureSample;
        
        s = pst_Meas->ul_MesureTime / 1000;
        ms = pst_Meas->ul_MesureTime % 1000; 
        OSTimeDlyHMSM(0u, 0u, s, ms,
                      OS_OPT_TIME_HMSM_STRICT,
                      &os_err);   
        
        PostMsg((void*)e_MeasureStaticCal);
        break;
     case e_MeasureStaticCal:
        MEASURE_DBG(">>MEASURE DBG:   ��̬����\r\n");
        
        MEASURE_DBG(">>MEASURE DBG:   ��ȡCO2COƽ��Ũ��\r\n");
        
        do
        {
            Mod_LaserRequestGasAvg(&st_Laser);                    //������ƽ��CO2COŨ��
        }while(PendSem(100) != TRUE);
        
        
        pst_Meas->lf_NO = WindowFilter (pst_Meas->st_SampleNO.af_Buff,
                                        pst_Meas->st_SampleNO.ul_Len,
                                        pst_Meas->uin_InvalidDots,
                                        pst_Meas->uin_ActiveDots);
        
        pst_Meas->lf_HC = WindowFilter (pst_Meas->st_SampleHC.af_Buff,
                                        pst_Meas->st_SampleHC.ul_Len,
                                        pst_Meas->uin_InvalidDots,
                                        pst_Meas->uin_ActiveDots);
        
#if 0
        /* ��������Ӵ�С���� */
        SortBuff(pst_Meas->st_SampleNO.af_Buff,pst_Meas->st_SampleNO.ul_Len);
        SortBuff(pst_Meas->st_SampleHC.af_Buff,pst_Meas->st_SampleHC.ul_Len);
        for(i = 0; i < 10; i++)
        {
            SortBuff(pst_Meas->st_SampleGrey[i].af_Buff, pst_Meas->st_SampleGrey[i].ul_Len);
        }
        
        pst_Meas->lf_Grey = 0;


        /* �����ȥn1�����ֵ֮�� ʣ��ֵ��n2����Чֵ��ƽ��ֵ */
        f = 0;
        if (pst_Meas->st_SampleNO.ul_Len > pst_Meas->uin_InvalidDots)
        {
        	/* �ܵ��� ������Ч���� */
        	INT16U num = pst_Meas->st_SampleNO.ul_Len - pst_Meas->uin_InvalidDots;		//����ȥ��N1����Ч���ʣ�����Ʒ����

        	if (num  > pst_Meas->uin_ActiveDots)										//�ж�ʣ������Ƿ����N2����Ч����
        		num  = pst_Meas->uin_ActiveDots;

            for( i = 0; i <	num; i++)
            	f += pst_Meas->st_SampleNO.af_Buff[pst_Meas->uin_InvalidDots + i];	    //���
            pst_Meas->lf_NO = f / num;								                    //��ƽ��
        }

        f = 0;
        if (pst_Meas->st_SampleHC.ul_Len > pst_Meas->uin_InvalidDots)
        {
        	/* �ܵ��� ������Ч���� */
        	INT16U num = pst_Meas->st_SampleHC.ul_Len - pst_Meas->uin_InvalidDots;		//����ȥ��N1����Ч���ʣ�����Ʒ����

        	if (num  > pst_Meas->uin_ActiveDots)										//�ж�ʣ������Ƿ����N2����Ч����
        		num  = pst_Meas->uin_ActiveDots;

            for( i = 0; i <	num; i++)
            	f += pst_Meas->st_SampleHC.af_Buff[pst_Meas->uin_InvalidDots + i];	    //���
            pst_Meas->lf_HC = f / num;								                    //��ƽ��
        }
#endif
        
        /* ����ȼ�շ��� ����ʵ��Ũ�� */

        q0 = pst_Meas->lf_CO / pst_Meas->lf_CO2;
        q1 = pst_Meas->lf_HC / pst_Meas->lf_CO2;
        q2 = pst_Meas->lf_NO / pst_Meas->lf_CO2;
        
        fCO2 = 42 / (2.79 + 2 * q0 + 1.21 * q1 + q2);
        fCO = fCO2 * q0;
        fHC = fCO2 * q1;
        fNO = fCO2 * q2;
/**/
        MEASURE_DBG(">>MEASURE DBG:================================================\r\n");  
        MEASURE_DBG(">>MEASURE DBG:   NO->%d��������\r\n",pst_Meas->st_SampleNO.ul_Len);
        for(i = 0; i < pst_Meas->st_SampleNO.ul_Len; i++)
            MEASURE_DBG(">>MEASURE DBG:     NO[%d]: %f\r\n", i, pst_Meas->st_SampleNO.af_Buff[i]);
        
        MEASURE_DBG(">>MEASURE DBG:   HC->%d��������\r\n",pst_Meas->st_SampleHC.ul_Len);
        for(i = 0; i < pst_Meas->st_SampleHC.ul_Len; i++)
            MEASURE_DBG(">>MEASURE DBG:     HC[%d]: %f\r\n", i, pst_Meas->st_SampleHC.af_Buff[i]);
        
        for (int j = 0; j < 10; j++)
        {
            MEASURE_DBG(">>MEASURE DBG:   Grey[%d]->%d��������\r\n", j, pst_Meas->st_SampleGrey[j].ul_Len);
            for(i = 0; i < pst_Meas->st_SampleGrey[j].ul_Len; i++)
                MEASURE_DBG(">>MEASURE DBG:     Grey[%d][%d]: %f\r\n", j, i, pst_Meas->st_SampleGrey[j].af_Buff[i]);        
        }


        MEASURE_DBG(">>MEASURE DBG:================================================\r\n"); 
        MEASURE_DBG(">>MEASURE DBG:   һ�ξ�̬�������\r\n");
        MEASURE_DBG(">>MEASURE DBG:   �����ٶ�:     %f\r\n",0.0);
        MEASURE_DBG(">>MEASURE DBG:   �������ٶ�:   %f\r\n",0.0);
        MEASURE_DBG(">>MEASURE DBG:   �����ȹ���:   %f\r\n",0.0);
        MEASURE_DBG(">>MEASURE DBG:   ƽ��CO2:      %f\r\n",pst_Meas->lf_CO2);
        MEASURE_DBG(">>MEASURE DBG:   ƽ��CO:       %f\r\n",pst_Meas->lf_CO);
        MEASURE_DBG(">>MEASURE DBG:   ƽ��NO:       %f\r\n",pst_Meas->lf_NO);
        MEASURE_DBG(">>MEASURE DBG:   ƽ��HC:       %f\r\n",pst_Meas->lf_HC);
        MEASURE_DBG(">>MEASURE DBG:   Q0:    CO/CO2 %f\r\n",q0);
        MEASURE_DBG(">>MEASURE DBG:   Q1:    HC/CO2 %f\r\n",q1);
        MEASURE_DBG(">>MEASURE DBG:   Q2:    NO/CO2 %f\r\n",q2);
        MEASURE_DBG(">>MEASURE DBG:   ����CO2:      %f\r\n",fCO2);
        MEASURE_DBG(">>MEASURE DBG:   ����CO:       %f\r\n",fCO);
        MEASURE_DBG(">>MEASURE DBG:   ����NO:       %f\r\n",fNO);
        MEASURE_DBG(">>MEASURE DBG:   ����HC:       %f\r\n",fHC);
        MEASURE_DBG(">>MEASURE DBG:================================================\r\n");
        
        if( pst_Meas->e_State != e_MeasureIdle)
            PostMsg((void*)e_MeasureStaticSample);	//ֱ�ӿ�ʼ����
        break; 
        
    default:
        pst_Meas->e_State = e_MeasureIdle;
        break;
    }
}
#if 0
void Mod_LaserCO2Notification(FP32 f_Concentration)
{
    if(st_Measure.st_SampleCO2.ul_Len < st_Measure.st_SampleCO2.ul_Size)
    {
        st_Measure.st_SampleCO2.af_Buff[st_Measure.st_SampleCO2.ul_Len++] = f_Concentration;
    }
}

void Mod_LaserCONotification(FP32 f_Concentration)
{
    if(st_Measure.st_SampleCO.ul_Len < st_Measure.st_SampleCO.ul_Size)
    {
        st_Measure.st_SampleCO.af_Buff[st_Measure.st_SampleCO.ul_Len++] = f_Concentration;
    }
}
#endif
void Mod_MeasureGasHCReply(FP64 lf_Concentration)
{
    if(st_Measure.e_State == e_MeasureSample) 
    {
        if(st_Measure.st_SampleHC.ul_Len < st_Measure.st_SampleHC.ul_Size)
        {
            st_Measure.st_SampleHC.af_Buff[st_Measure.st_SampleHC.ul_Len++] = lf_Concentration;
        }
    }
}

void Mod_MeasureGasNOReply(FP64 lf_Concentration)
{
    if(st_Measure.e_State == e_MeasureSample) 
    {
        if(st_Measure.st_SampleNO.ul_Len < st_Measure.st_SampleNO.ul_Size)
        {
            st_Measure.st_SampleNO.af_Buff[st_Measure.st_SampleNO.ul_Len++] = lf_Concentration;
        }
    }
}

void Mod_GreyMeasureNotification(INT8U uch_Channel, FP32 lf_Gery)
{
    if(st_Measure.e_State == e_MeasureSample) 
    {
        if(st_Measure.st_SampleGrey[uch_Channel].ul_Len < st_Measure.st_SampleGrey[uch_Channel].ul_Size)
        {
            st_Measure.st_SampleGrey[uch_Channel].af_Buff[st_Measure.st_SampleGrey[uch_Channel].ul_Len++] = lf_Gery;
        }
    }
}

void Mod_LaserReply(LaserBoard_t* pst_Laser)
{
    PostSem();
}

void Mod_MeasSpeedReply(MeasSpeed_t* pst_Meas)
{
    st_Measure.st_Result.f_Acc = pst_Meas->f_Acc_mps2;
    st_Measure.st_Result.f_Speed = pst_Meas->f_Speed_mph;
    PostSem();
}



void Bsp_GpioEventHandle(GpioEvent_t* pst_Event)
{
#if 1
    if(pst_Event->uin_GpioPin == GPIO_PIN_14 && pst_Event->b_IsRising == TRUE )         //��ͷ��ס��һ��������
    {
        if(st_Measure.e_State == e_MeasureIdle) 
        {
            PostMsg((void*)e_MeasureWait);
        }
    }
    else if(pst_Event->uin_GpioPin == GPIO_PIN_13 && pst_Event->b_IsRising == FALSE )   //��λ�뿪�ڶ���������
    {
        if(st_Measure.e_State == e_MeasureWait) 
        {
            PostMsg((void*)e_MeasureDead);
        }
    }
#else
    MEASURE_DBG("����:0x%x,����:%x\r\n", pst_Event->uin_GpioPin, pst_Event->b_IsRising);
#endif
}

void DoorCloseFnct (void *p_tmr,  void *p_arg)
{
    PostMsg((void*)e_MeasureTimeOut);
}
