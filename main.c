#include "main.h"
#include "Periph_Conf.h"

int main(void)
{
	unsigned int rdStart;

	SystemInit();Init_pData_Bus(); Init_CKc_CKr(); Init_SPIs(); Init_DMA();

	/* Infinite loop */
	while (1)
	{
		rdStart = (GPIOD->IDR & Start);  // Ждем низкий уровень Start
		while (rdStart != 0) rdStart = (GPIOD->IDR & Start);

		rdStart = GPIOD->IDR & Start;    // Ждем Высокий уровень Start
		while (rdStart == 0) rdStart = GPIOD->IDR & Start;

		Transaction_Count = 0;
		Start_Pulse();
		while(DataIsSending);
	}
  return 0;
}
