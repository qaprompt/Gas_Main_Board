//==================================================================================================
//| �ļ����� | Bsp_Gpio.c
//|----------|--------------------------------------------------------------------------------------
//| �ļ����� | ��ͨ�������IO�ĳ�ʼ�� �����IO��ʼ���ڶ�Ӧ��������ļ��� STM32�汾
//|----------|--------------------------------------------------------------------------------------
//| ��Ȩ���� |
//|----------|--------------------------------------------------------------------------------------
//|  �汾    |  ʱ��       |  ����     | ����
//|----------|-------------|-----------|------------------------------------------------------------
//|  V1.0    | 2018.12.02  |  wjb      | ����
//==================================================================================================
#include "Bsp.h"

typedef struct {
    GpioId_e  ul_ID;                    /*  ʹ�ñ��    */
    INT8U*    ppch_Name;                /*  �����ַ���  */
    BOOL      b_InState;                /*  ����״̬    */
    BOOL      b_OutState;               /*  ���״̬    */

    /* �˿����ò��� ����ݲ�ͬ��оƬ���Ҹ��� */
    GPIO_TypeDef*  ul_Port;
    INT32U  ul_Pin;
    INT32U  ul_Mode;
    INT32U  ul_Pull;
    INT32U  ul_Speed;

}GpioConfig_t;

GpioConfig_t ast_GpioConfig[] = {
/*|-------ʹ�ñ��-------|--�����ַ���--|--����--|--���--|--�˿�--|------����------|--------ģʽ-------|----������----|-------�ٶ�-------|*/
    {e_IO_Relay0,          "�̵���0",   FALSE,   FALSE,   GPIOH,  GPIO_PIN_15,    GPIO_MODE_OUTPUT_PP, GPIO_PULLUP,  GPIO_SPEED_HIGH},
    {e_IO_Relay1,          "�̵���1",   FALSE,   FALSE,   GPIOH,  GPIO_PIN_13,    GPIO_MODE_OUTPUT_PP, GPIO_PULLUP,  GPIO_SPEED_HIGH},
    {e_IO_Relay2,          "�̵���2",   FALSE,   FALSE,   GPIOH,  GPIO_PIN_11,    GPIO_MODE_OUTPUT_PP, GPIO_PULLUP,  GPIO_SPEED_HIGH},
    {e_IO_Relay3,          "�̵���3",   FALSE,   FALSE,   GPIOH,  GPIO_PIN_14,    GPIO_MODE_OUTPUT_PP, GPIO_PULLUP,  GPIO_SPEED_HIGH},
    
    {e_IO_245OE,         "74HC245 OE",  FALSE,   FALSE,   GPIOF,  GPIO_PIN_11,    GPIO_MODE_OUTPUT_PP, GPIO_PULLUP,  GPIO_SPEED_HIGH},
    {e_IO_245DIR,        "74HC245 DIR", FALSE,   TRUE,    GPIOF,  GPIO_PIN_15,    GPIO_MODE_OUTPUT_PP, GPIO_PULLUP,  GPIO_SPEED_HIGH},
    {e_IO_Sync0,         "ͬ���ź�0",   FALSE,   FALSE,   GPIOF,  GPIO_PIN_12,    GPIO_MODE_IT_RISING_FALLING, GPIO_PULLUP,  GPIO_SPEED_HIGH},
    {e_IO_Sync1,         "��������",    FALSE,   FALSE,   GPIOF,  GPIO_PIN_13,     GPIO_MODE_IT_RISING_FALLING, GPIO_PULLUP,  GPIO_SPEED_HIGH},
    {e_IO_Sync2,         "������ʼ",    FALSE,   FALSE,   GPIOF,  GPIO_PIN_14,     GPIO_MODE_IT_RISING_FALLING, GPIO_PULLUP,  GPIO_SPEED_HIGH},
};

static INT32U  ul_UsePinNum = sizeof(ast_GpioConfig)/sizeof(GpioConfig_t);     /*����ʹ�õ�������*/

BOOL Bsp_GpioInit(void)
{
    INT32U  i;

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOI_CLK_ENABLE();
    
    for(i = 0; i < ul_UsePinNum; i++)
    {
        GPIO_InitTypeDef  gpio_init;

        gpio_init.Pin   = ast_GpioConfig[i].ul_Pin;
        gpio_init.Mode  = ast_GpioConfig[i].ul_Mode;
        gpio_init.Pull  = ast_GpioConfig[i].ul_Pull;
        gpio_init.Speed = ast_GpioConfig[i].ul_Speed;
        HAL_GPIO_Init(ast_GpioConfig[i].ul_Port, &gpio_init);
        
        if(ast_GpioConfig[i].ul_Mode == GPIO_MODE_OUTPUT_PP)
        {
            HAL_GPIO_WritePin((GPIO_TypeDef*)ast_GpioConfig[i].ul_Port,ast_GpioConfig[i].ul_Pin,
                              (GPIO_PinState)ast_GpioConfig[i].b_OutState);        
        }
    }
    
    /* EXTI interrupt init*/
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
    
    return TRUE;
}

void Bsp_GpioWirte(GpioId_e e_GpioId,BOOL b_State)
{
    GpioConfig_t* pst_Gpio;
#if (DEF_USE_FAST_FIND_MODE == TRUE)
    /* ��������ģʽ */
    pst_Gpio = &ast_GpioConfig[e_GpioId];
#else
    /* ��������ģʽ */
    INT32U  i;
    for (i = 0; i < ul_UsePinNum; i++)
    {
        if (ast_GpioConfig[i].ul_ID == e_GpioId)
        {
            pst_Gpio = (GpioConfig_t*)&ast_GpioConfig[i];
            break;
        }
    }
#endif
    pst_Gpio->b_OutState = b_State;
    
    HAL_GPIO_WritePin((GPIO_TypeDef*)pst_Gpio->ul_Port, pst_Gpio->ul_Pin,
                      (GPIO_PinState)pst_Gpio->b_OutState);
}

BOOL Bsp_GpioReadOut(GpioId_e e_GpioId)
{
    GpioConfig_t* pst_Gpio;
#if (DEF_USE_FAST_FIND_MODE == TRUE)
    /* ��������ģʽ */
    pst_Gpio = &ast_GpioConfig[e_GpioId];
#else
    /* ��������ģʽ */
    INT32U  i;
    for (i = 0; i < ul_UsePinNum; i++)
    {
        if (ast_GpioConfig[i].ul_ID == e_GpioId)
        {
            pst_Gpio = (GpioConfig_t*)&ast_GpioConfig[i];
            break;
        }
    }
#endif
    return pst_Gpio->b_OutState;
}

BOOL Bsp_GpioReadIn(GpioId_e e_GpioId)
{
    GpioConfig_t* pst_Gpio;
#if (DEF_USE_FAST_FIND_MODE == TRUE)
    /* ��������ģʽ */
    pst_Gpio = &ast_GpioConfig[e_GpioId];
#else
    /* ��������ģʽ */
    INT32U  i;
    for (i = 0; i < ul_UsePinNum; i++)
    {
        if (ast_GpioConfig[i].ul_ID == e_GpioId)
        {
            pst_Gpio = (GpioConfig_t*)&ast_GpioConfig[i];
            break;
        }
    }
#endif
    pst_Gpio->b_InState = HAL_GPIO_ReadPin((GPIO_TypeDef*)pst_Gpio->ul_Port, pst_Gpio->ul_Pin);
    return pst_Gpio->b_InState;
}


void EXTI15_10_IRQHandler(void)
{
#ifdef  OS_SUPPORT
    CPU_SR_ALLOC();

    CPU_CRITICAL_ENTER();
    OSIntEnter();
    CPU_CRITICAL_EXIT();
#endif
    
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
    
#ifdef  OS_SUPPORT
    OSIntExit();
#endif
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    //16���ж���
    static GpioEvent_t st_Event[16];
    static INT8U uch_Index = 0;
    
    Bsp_DelayUs(100);
    
    BOOL b_IsRising = HAL_GPIO_ReadPin(GPIOF,GPIO_Pin);     //��ȡһ�ε�ƽ
    
    Bsp_GetTimeSample(&st_Event[uch_Index].st_Ts);          //��ȡʱ���
    st_Event[uch_Index].uin_GpioPin = GPIO_Pin;
    st_Event[uch_Index].vp_GpioPort = GPIOD;
    st_Event[uch_Index].b_IsRising = b_IsRising;
    
    if(HAL_GPIO_ReadPin(GPIOF,GPIO_Pin) != b_IsRising)      //��ȡ�ڶ��ε�ƽ��Ϊ����
        return;
    
    /* ����������ͨ���ŷ�����Ϣ ��˫���ض��ʺ� */
    Bsp_GpioEventHandle(&st_Event[uch_Index]);
    if(++uch_Index >= 16)
        uch_Index = 0;

}

__weak void Bsp_GpioEventHandle(GpioEvent_t* pst_Event)
{

}




