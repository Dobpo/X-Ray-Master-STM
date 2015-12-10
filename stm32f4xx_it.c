/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-October-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "main.h"
#include "tcp_echoserver.h"

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  /* Update the LocalTime by adding SYSTEMTICK_PERIOD_MS each SysTick interrupt */
  Time_Update();
}

/** @TODO
  * @brief  This function handles External line 10 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(void)//EXTI15_10_IRQHandler(void)
{
  if(EXTI_GetITStatus(ETH_LINK_EXTI_LINE) != RESET)
  {
    Eth_Link_ITHandler(DP83848_PHY_ADDRESS);
    /* Clear interrupt pending bit */
    EXTI_ClearITPendingBit(EXTI_Line0);//ETH_LINK_EXTI_LINE);
  }
}

//Внешнее прерывание для триггов АЦП
void EXTI15_10_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line12) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line12);

		if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_12))
		{
			STM_EVAL_LEDOn(LED3);
			Start_Pulse();
		}else {
			STM_EVAL_LEDOff(LED3);
			Stop_Pulse();
		}
	}
}

/* Обработчик прерываний для DMA2_Stream0
 *
 */
void DMA2_Stream0_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_Stream0, DMA_IT_HTIF0))
	{
		Send_Data_to_Ethernet(Current_PCB, &Buffer_to_send_from_SPI1, Buffer_Lenght);
		DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_HTIF0);
	}else {
		Send_Data_to_Ethernet(Current_PCB, (uint32_t)&Buffer_to_send_from_SPI1 + Buffer_Lenght, Buffer_Lenght);
		DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
	}
}
/* Обработчик прерываний для DMA1_Stream2
 *
 */
void DMA1_Stream2_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_Stream2, DMA_IT_HTIF2))
	{
		Send_Data_to_Ethernet(Current_PCB, &Buffer_to_send_from_SPI3, Buffer_Lenght);
		DMA_ClearITPendingBit(DMA1_Stream2, DMA_IT_HTIF2);
	}else {
		Send_Data_to_Ethernet(Current_PCB, (uint32_t)&Buffer_to_send_from_SPI3 + Buffer_Lenght, Buffer_Lenght);
		DMA_ClearITPendingBit(DMA1_Stream2, DMA_IT_TCIF2);
	}
}

/* Обработчик прерываний для DMA1_Stream3
 *
 */
void DMA1_Stream3_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_Stream3, DMA_IT_HTIF3))
	{
		Send_Data_to_Ethernet(Current_PCB, &Buffer_to_send_from_SPI2, Buffer_Lenght);
		DMA_ClearITPendingBit(DMA1_Stream3, DMA_IT_HTIF3);
	}else {
		Send_Data_to_Ethernet(Current_PCB, (uint32_t)&Buffer_to_send_from_SPI2 + Buffer_Lenght, Buffer_Lenght);
		DMA_ClearITPendingBit(DMA1_Stream3, DMA_IT_TCIF3);
	}
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
