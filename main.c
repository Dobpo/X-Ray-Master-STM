#include "main.h"
#include "Periph_Conf.h"

int main(void)
{
	unsigned int rdStart;

	SystemInit();Init_pData_Bus(); Init_CKc_CKr(); Init_SPIs(); Init_DMA();

	/* Infinite loop */
	while (1)
	{
		GPIOE->BSRRH = RDY;       //  DataRdy = 0  (��������� �������)
		GPIOD->BSRRL = Led_Red;   // �������� ������� ���������

		rdStart = (GPIOD->IDR & Start);  // ���� ������ ������� Start
		while (rdStart != 0) rdStart = (GPIOD->IDR & Start);

		GPIOD->BSRRH = Led_Red;          // ��������� ������� ���������
		GPIOD->BSRRL = Led_Orange;       // �������� ��������� ���������

		rdStart = GPIOD->IDR & Start;    // ���� ������� ������� Start
		while (rdStart == 0) rdStart = GPIOD->IDR & Start;

		GPIOD->BSRRH = Led_Orange;  // ��������� ��������� ���������

		Transaction_Count = 0;
		Start_Pulse();
		while(DataIsSending);
	}
  return 0;
}
