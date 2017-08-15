#include "main.h"
#include "Periph_Conf.h"

int main(void)
{
	unsigned int rdStart;

	SystemInit();Init_pData_Bus(); Init_CKc_CKr(); Init_SPIs(); Init_DMA();

	//���� �� ���������� �������� �������
	//Fill_Test_Data(1);

	/* Infinite loop */
	while (1)
	{
		rdStart = (GPIOD->IDR & Start);  // ���� ������ ������� Start
		while (rdStart != 0) rdStart = (GPIOD->IDR & Start);

		rdStart = GPIOD->IDR & Start;    // ���� ������� ������� Start
		while (rdStart == 0) rdStart = GPIOD->IDR & Start;

		Transaction_Count = 0;
		Start_Pulse();
		while(DataIsSending);
	}
  return 0;
}
