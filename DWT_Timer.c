#include <DWT_Timer.h>

uint32_t DWT_Init(void)
{
    /* Disable TRC */
    CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk; // ~0x01000000;
    /* Enable TRC */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // 0x01000000;

    /* Disable clock cycle counter */
    DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; //~0x00000001;
    /* Enable  clock cycle counter */
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk; // 0x00000001;

    /* Reset the clock cycle counter value */
    DWT->CYCCNT = 0;

    if (DWT->CYCCNT)
    {
        return 0; /*clock cycle counter started*/
    }
    else
    {
        return 1; /*clock cycle counter not started*/
    }
}

void DWT_delay_us(volatile uint32_t micsec)
{
    uint32_t initial_tick = DWT->CYCCNT;
    uint32_t ticks = (HAL_RCC_GetHCLKFreq() / 1000000);
    micsec *= ticks;
    while ((DWT->CYCCNT - initial_tick) < micsec - ticks)
        ;
}

void DWT_delay_ms(volatile uint32_t milsec)
{
    uint32_t initial_tick = DWT->CYCCNT;
    uint32_t ticks = (HAL_RCC_GetHCLKFreq() / 1000);
    milsec *= ticks;
    while ((DWT->CYCCNT - initial_tick) < milsec - ticks)
        ;
}

uint32_t DWT_getTick_us()
{
    volatile uint32_t time_stamp = DWT->CYCCNT;
    uint32_t ticks = (HAL_RCC_GetHCLKFreq() / 1000000);
    time_stamp /= ticks;
    return time_stamp;
}
