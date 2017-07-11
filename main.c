#include "main.h"

/* Private define ------------------------------------------------------------*/
#define SYSTEMTICK_PERIOD_MS  10

/* Private variables ---------------------------------------------------------*/
__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 10ms */
uint32_t timingdelay;
/**
  * @brief  Main program.
  */
int main(void)
{
	SystemInit(); Init_CKc_CKr(); Start_Pulse(); Stop_Pulse();
	Init_SPIs(); Init_DMA_Streams(); Init_EXTI_for_DMA();


	/* Infinite loop */
	while (1)
	{

	}
  return 0;
}

/**
  * @brief  Inserts a delay time.
  * @param  nCount: number of 10ms periods to wait for.
  * @retval None
  */
void Delay(uint32_t nCount)
{
  /* Capture the current local time */
  timingdelay = LocalTime + nCount;

  /* wait until the desired delay finish */
  while(timingdelay > LocalTime)
  {
  }
}
