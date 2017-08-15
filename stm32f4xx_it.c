#include "stm32f4xx_it.h"

/* Обработчик прерываний для DMA2_Stream0
 * Рабочее прерывание для передачи данных
 */
void DMA2_Stream0_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_Stream0, DMA_IT_HTIF0))
	{
		for	(int i = 0; i < Buffer_Lenght/2; i++)
			Send_Data(Buffer_to_send_from_SPI1[i]);
		for	(int i = 0; i < Buffer_Lenght/2; i++)
			Send_Data(Buffer_to_send_from_SPI2[i]);
		for	(int i = 0; i < Buffer_Lenght/2; i++)
			Send_Data(Buffer_to_send_from_SPI3[i]);
		DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_HTIF0);
	}else {
		for	(int i = Buffer_Lenght/2; i < Buffer_Lenght; i++)
			Send_Data(Buffer_to_send_from_SPI1[i]);
		for	(int i = Buffer_Lenght/2; i < Buffer_Lenght; i++)
			Send_Data(Buffer_to_send_from_SPI2[i]);
		for	(int i = Buffer_Lenght/2; i < Buffer_Lenght; i++)
			Send_Data(Buffer_to_send_from_SPI3[i]);
		DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
	}
}

//Тестовое прерывание для передачи данных данных
/*
void DMA2_Stream0_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_Stream0, DMA_IT_HTIF0))
	{
			for	(int i = 0; i < Buffer_Lenght/2; i++)
				Send_Data(Buffer1[i]);
			for	(int i = 0; i < Buffer_Lenght/2; i++)
				Send_Data(Buffer2[i]);
			for	(int i = 0; i < Buffer_Lenght/2; i++)
				Send_Data(Buffer3[i]);
			DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_HTIF0);
		}else {
			for	(int i = Buffer_Lenght/2; i < Buffer_Lenght; i++)
				Send_Data(Buffer1[i]);
			for	(int i = Buffer_Lenght/2; i < Buffer_Lenght; i++)
				Send_Data(Buffer2[i]);
			for	(int i = Buffer_Lenght/2; i < Buffer_Lenght; i++)
				Send_Data(Buffer3[i]);
			DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
	}
}*/



