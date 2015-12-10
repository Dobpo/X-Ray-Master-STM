#include "stm32f4x7_eth.h"
#include "netconf.h"
#include "main.h"
#include "tcp_echoserver.h"

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
	SystemInit();

	/* configure ethernet & init ethernet*/
	ETH_BSP_Config();LwIP_Init();tcp_echoserver_init();LCD_LED_Init();

	/*config & init periphial*/
	Init_SPIs();
	Init_DMA_Streams();
	Init_EXTI_for_DMA();
	Init_EXTI_12();
	Init_CKc_CKr();

	/* Infinite loop */
	while (1)
	{
		/* check if any packet received */
		if (ETH_CheckFrameReceived())
		{
			/* process received ethernet packet */
			LwIP_Pkt_Handle();
		}
		/* handle periodic timers for LwIP */
		LwIP_Periodic_Handle(LocalTime);
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

/**
  * @brief  Updates the system local time
  * @param  None
  * @retval None
  */
void Time_Update(void)
{
  LocalTime += SYSTEMTICK_PERIOD_MS;
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
