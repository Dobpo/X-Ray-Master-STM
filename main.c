#include "main.h"
#include "Periph_Conf.h"

int main(void)
{
	SystemInit(); Init_CKc_CKr(); Start_Pulse(); Stop_Pulse();
	Init_SPIs(); Init_DMA_Streams(); Init_EXTI_for_DMA();Init_pData_Bus();

	/* Infinite loop */
	while (1)
	{
		GPIOE->BSRRH = RDY;       //  DataRdy = 0  (Начальный уровень)
		GPIOD->BSRRL = Led_Red;   // Включаем красный светодиод

		rdStart = (GPIOD->IDR & Start);  // Ждем низкий уровень Start
		while (rdStart != 0) rdStart = (GPIOD->IDR & Start);

		GPIOD->BSRRH = Led_Red;          // Выключаем красный светодиод
		GPIOD->BSRRL = Led_Orange;       // Включаем оранжевый светодиод

		rdStart = GPIOD->IDR & Start;    // Ждем Высокий уровень Start
		while (rdStart == 0) rdStart = GPIOD->IDR & Start;

		GPIOD->BSRRH = Led_Orange;  // Выключаем оранжевый светодиод
		GPIOD->BSRRL = Led_Green;   // Включаем  зеленый светодиод

		Start_Pulse();
	}
  return 0;
}
