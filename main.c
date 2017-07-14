#include "main.h"
#include "Periph_Conf.h"

int main(void)
{
	unsigned int rdStart;

	SystemInit();Init_pData_Bus(); Init_CKc_CKr(); Init_SPIs(); Init_DMA();

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

		Transaction_Count = 0;
		Start_Pulse();
		while(DataIsSending);
	}
  return 0;
}
