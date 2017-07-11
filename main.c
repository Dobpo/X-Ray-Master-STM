#include "main.h"
#include "Periph_Conf.h"

int main(void)
{
	SystemInit(); Init_CKc_CKr(); Start_Pulse(); Stop_Pulse();
	Init_SPIs(); Init_DMA_Streams(); Init_EXTI_for_DMA();Init_pData_Bus();

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
		GPIOD->BSRRL = Led_Green;   // ��������  ������� ���������

		Start_Pulse();
	}
  return 0;
}
