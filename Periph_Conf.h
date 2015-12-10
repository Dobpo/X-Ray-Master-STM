#ifndef __PERIPH_CONF_H
#define __PERIPH_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx.h"
#include "tcp_echoserver.h"
#include "stm32f4_discovery.h"

#define Buffer_Lenght 768

#define SPI1_DR_Address 0x4001300C
#define SPI2_DR_Address 0x4000380C
#define SPI3_DR_Address 0x40003C0C

__IO uint16_t Buffer_to_send_from_SPI1[Buffer_Lenght];
__IO uint16_t Buffer_to_send_from_SPI2[Buffer_Lenght];
__IO uint16_t Buffer_to_send_from_SPI3[Buffer_Lenght];

//Main clock
void Init_CKc_CKr(void);
void Start_Pulse(void);
void Stop_Pulse(void);
//SPI
void Init_SPIs(void);
void Init_DMA_Streams(void);
void Init_EXTI_for_DMA(void);
//EXTI
void Init_EXTI_12(void);
//Functions
void Send_Data_to_Ethernet(struct tcp_pcb *pPCB, uint32_t * pBuffer, uint32_t buffer_size);
void LCD_LED_Init(void);
#ifdef __cplusplus
}
#endif

#endif
