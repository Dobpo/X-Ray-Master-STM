#include "stm32f4xx_it.h"
#include "main.h"

/* Обработчик прерываний для DMA2_Stream0
 *
 */
void DMA2_Stream0_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_Stream0, DMA_IT_HTIF0))
	{
		//Send_Data_to_Ethernet(Current_PCB, &Buffer_to_send_from_SPI1, Buffer_Lenght);
		//Send_Data_to_Ethernet(Current_PCB, &Buffer_to_send_from_SPI2, Buffer_Lenght);
		//Send_Data_to_Ethernet(Current_PCB, &Buffer_to_send_from_SPI3, Buffer_Lenght);
		DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_HTIF0);
	}else {
		//Send_Data_to_Ethernet(Current_PCB, (uint32_t)&Buffer_to_send_from_SPI1 + Buffer_Lenght, Buffer_Lenght);
		//Send_Data_to_Ethernet(Current_PCB, (uint32_t)&Buffer_to_send_from_SPI2 + Buffer_Lenght, Buffer_Lenght);
		//Send_Data_to_Ethernet(Current_PCB, (uint32_t)&Buffer_to_send_from_SPI3 + Buffer_Lenght, Buffer_Lenght);
		DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
	}
}


