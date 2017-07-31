#ifndef __PERIPH_CONF_H
#define __PERIPH_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx.h"

 // ������� ������ + ���������� -------------------------------------------------
 #define OutMask 0x0000FFF4  // �������� ����� ������������ ���� (PE2, PE4..PE15)
 #define RDY 	 0x00000004  // �������� ������ "������ ������" (PE2)
 #define Start   0x00000100  // ������� ������ "����� ����� � ��������"
 #define BusY    0x00000400  // ������� ������ "����������� ���� ��������"
 #define OutData 0x0000FFF0  // �������� ����� ������������ ���� (PE2, PE4..PE15)

 #define Led_Green  0x00001000      // ������� ���������
 #define Led_Orange 0x00002000      // ������� ���������
 #define Led_Red    0x00004000      // ������� ���������
 #define Led_Blue   0x00008000      // ����� ���������
 #define LedMask  Led_Green | Led_Orange | Led_Red | Led_Blue;

#define Buffer_Lenght 768

#define SPI1_DR_Address 0x4001300C
#define SPI2_DR_Address 0x4000380C
#define SPI3_DR_Address 0x40003C0C

uint16_t Data;
volatile unsigned int DataIsSending;
volatile unsigned int Transaction_Count;

uint16_t Buffer_to_send_from_SPI1[Buffer_Lenght];
uint16_t Buffer_to_send_from_SPI2[Buffer_Lenght];
uint16_t Buffer_to_send_from_SPI3[Buffer_Lenght];

//Main clock
void Init_CKc_CKr(void);
void Start_Pulse(void);
void Stop_Pulse(void);
//SPI
void Init_SPIs(void);
void Init_DMA(void);
//EXTI
void Init_EXTI_12(void);
//Data bus
void Init_pData_Bus(void);
void Send_Data(uint16_t data);

#ifdef __cplusplus
}
#endif

#endif
