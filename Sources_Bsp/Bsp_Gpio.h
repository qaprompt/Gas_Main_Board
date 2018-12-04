//==================================================================================================
//| �ļ����� | Bsp_Gpio.h
//|----------|--------------------------------------------------------------------------------------
//| �ļ����� | ��ͨ�������IO�ĳ�ʼ�� �����IO��ʼ���ڶ�Ӧ��������ļ��� STM32�汾
//|----------|--------------------------------------------------------------------------------------
//| ��Ȩ���� |
//|----------|--------------------------------------------------------------------------------------
//|  �汾    |  ʱ��       |  ����     | ����
//|----------|-------------|-----------|------------------------------------------------------------
//|  V1.0    | 2018.12.02  |  wjb      | ����
//==================================================================================================

#ifndef     __BSP_GPIO_H__
#define     __BSP_GPIO_H__

/* ֻ�е�GpioId_e�еĳ�ʼ��˳��ͱ���˳��һ��ʱ����ʹ�ÿ�������ģʽ */
#define     DEF_USE_FAST_FIND_MODE               FALSE

typedef enum {
    e_IO_Relay0 = 0,
    e_IO_Relay1,
    e_IO_Relay2,
    e_IO_Relay3,
    e_IO_245OE,  
    e_IO_245DIR, 
    e_IO_Sync0,  
    e_IO_Sync1,  
    e_IO_Sync2,  
    
}GpioId_e;


typedef struct {
    void * vp_GpioPort;     /* �˿� */
    INT16U  uin_GpioPin;    /* ���� */
    BOOL  b_IsRising;       /* ���� */
    TimeSample_t st_Ts;     /* ʱ��� */
}GpioEvent_t;


BOOL Bsp_GpioInit(void);

void Bsp_GpioWirte(GpioId_e e_GpioId,BOOL b_State);

BOOL Bsp_GpioReadOut(GpioId_e e_GpioId);

BOOL Bsp_GpioReadIn(GpioId_e e_GpioId);

__weak void Bsp_GpioEventHandle(GpioEvent_t* pst_Event);

#endif
