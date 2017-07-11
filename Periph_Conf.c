#include "Periph_Conf.h"

/* Отправка одного слова данных
 * */
void Send_Data(uint16_t data){
	GPIOE->BSRRH = OutMask;    // Чистим линии данных
	Data = (data << 4) | RDY;

	rdStart = GPIOD->IDR & BusY;    // Ждем низкий уровень BusY
	while (rdStart != 0) rdStart = GPIOD->IDR & BusY;
	GPIOE->BSRRL = Data; // Устанавливаем данные на линии
	GPIOE->BSRRH = RDY;        //  DataRdy = 0 (Защелкиваем данные)
}

/* Инициализация SPIx (1,2,3) для приема данных
 * с Slave контроллеров. SPI настроен в режиме Slave.
 *
 */
void Init_SPIs(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPI_InitStruct;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2 | RCC_APB1Periph_SPI3, ENABLE);

	/** SPIs GPIO Configuration
	 *          SPI1   SPI2   SPI3
		 SCK:  | PB3  | PB10 | PC10
		 MOSI: | PB5  | PB15 | PC12
	*/
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_10
			| GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin alternate function SPI1:*/
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);
	/*Configure GPIO pin alternate function SPI2:*/
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);
	/*Configure GPIO pin alternate function SPI3:*/
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SPI3);


	SPI_I2S_DeInit(SPI1);
	SPI_I2S_DeInit(SPI2);
	SPI_I2S_DeInit(SPI3);
	SPI_StructInit(&SPI_InitStruct);
	SPI_InitStruct.SPI_Mode = SPI_Mode_Slave;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_16b;
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(SPI1, &SPI_InitStruct);
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_Init(SPI2, &SPI_InitStruct);
	SPI_Init(SPI3, &SPI_InitStruct);


	SPI_Cmd(SPI1, ENABLE);
	SPI_Cmd(SPI2, ENABLE);
	SPI_Cmd(SPI3, ENABLE);

	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);
	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, ENABLE);
	SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Rx, ENABLE);
}

/* Инициализация DMA для чтения данных из SPIs
 * в буфферы.
 *
 */
void Init_DMA_Streams(void)
{
	DMA_InitTypeDef DMA_InitStruct;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_DMA1, ENABLE);

	DMA_DeInit(DMA2_Stream0);
	DMA_StructInit(&DMA_InitStruct);
	DMA_InitStruct.DMA_Channel = DMA_Channel_3;
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)SPI1_DR_Address;
    DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t) & Buffer_to_send_from_SPI1;
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStruct.DMA_BufferSize = Buffer_Lenght;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;
	DMA_Init(DMA2_Stream0, &DMA_InitStruct);
	DMA_InitStruct.DMA_Channel = DMA_Channel_0;
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)SPI3_DR_Address;
	DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t) & Buffer_to_send_from_SPI3;
	DMA_Init(DMA1_Stream2, &DMA_InitStruct);
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)SPI2_DR_Address;
	DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t) & Buffer_to_send_from_SPI2;
	DMA_Init(DMA1_Stream3, &DMA_InitStruct);

	DMA_Cmd(DMA2_Stream0, ENABLE);
	DMA_Cmd(DMA1_Stream2, ENABLE);
	DMA_Cmd(DMA1_Stream3, ENABLE);


	DMA_ITConfig(DMA2_Stream0, DMA_IT_HT | DMA_IT_TC, ENABLE);
	//DMA_ITConfig(DMA1_Stream2, DMA_IT_HT | DMA_IT_TC, ENABLE);
	//DMA_ITConfig(DMA1_Stream3, DMA_IT_HT | DMA_IT_TC, ENABLE);
}

/* Инициализация обр. прерываний для половинного и полного
 * заполнения буфферов через DMA.
 *
 */
void Init_EXTI_for_DMA(void)
{
	NVIC_InitTypeDef Nvic_Initstruct;

	Nvic_Initstruct.NVIC_IRQChannel = DMA2_Stream0_IRQn;
 	Nvic_Initstruct.NVIC_IRQChannelPreemptionPriority = 0x01;
 	Nvic_Initstruct.NVIC_IRQChannelSubPriority = 0x01;
 	Nvic_Initstruct.NVIC_IRQChannelCmd = ENABLE;
 	NVIC_Init(&Nvic_Initstruct);
 	/*Nvic_Initstruct.NVIC_IRQChannel = DMA1_Stream2_IRQn;
 	NVIC_Init(&Nvic_Initstruct);
 	Nvic_Initstruct.NVIC_IRQChannel = DMA1_Stream3_IRQn;
 	NVIC_Init(&Nvic_Initstruct);*/
}

/* Инициализация TIM10 для генерации CKc (main clock).
 * Инициализация TIM11 для генерации CKr.
 * Режим PWM, Port B, Pin 8 - CKc, Pin 9 - CKr.
 * Для запуска "Start_Pulse()".
 */
void Init_CKc_CKr(void){
	GPIO_InitTypeDef Gpio_InitStruct;
	TIM_TimeBaseInitTypeDef Tim_Initstruct;
	TIM_OCInitTypeDef Tim_OCInitStruct;

	uint32_t Period_CKc, Duty_CKc, Period_CKr, Duty_CKr;

	//Период задается в тактах контроллера по формуле :
	//Период = (Частота контр. / делитель частоты на шине) / желаемая частота периода.
	//Внимание на делитель частоты перед таймером.
	Period_CKc = (SystemCoreClock/4) / 1000000;
	Duty_CKc = (5 * Period_CKc / 10);
	Period_CKr = ((Period_CKc + 1) * 1100) - 1;
	Duty_CKr = ((Period_CKc + 1) * 1032) - 1;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11, ENABLE);

	GPIO_StructInit(&Gpio_InitStruct);
	Gpio_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	Gpio_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	Gpio_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &Gpio_InitStruct);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_TIM10);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_TIM11);

	//Configuration of CKc.
	TIM_TimeBaseStructInit(&Tim_Initstruct);
	Tim_Initstruct.TIM_Period = Period_CKc;
	Tim_Initstruct.TIM_Prescaler = 0x03;
	TIM_TimeBaseInit(TIM10, &Tim_Initstruct);

	TIM_OCStructInit(&Tim_OCInitStruct);
	Tim_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	Tim_OCInitStruct.TIM_OutputState = ENABLE;
	Tim_OCInitStruct.TIM_Pulse = Duty_CKc;
	TIM_OC1Init(TIM10, &Tim_OCInitStruct);

	//Configuration of CKr.
	TIM_TimeBaseStructInit(&Tim_Initstruct);
	Tim_Initstruct.TIM_Period = Period_CKr;
	Tim_Initstruct.TIM_Prescaler = 0x03;
	TIM_TimeBaseInit(TIM11, &Tim_Initstruct);

	TIM_OCStructInit(&Tim_OCInitStruct);
	Tim_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	Tim_OCInitStruct.TIM_OutputState = ENABLE;
	Tim_OCInitStruct.TIM_Pulse = Duty_CKr;
	TIM_OC1Init(TIM11, &Tim_OCInitStruct);
}

/* Запуск тактирования CKc & CKr.
 */
void Start_Pulse(void){
	TIM10->CNT = 0;
	TIM11->CNT = 9;

	TIM10->CR1 |= TIM_CR1_CEN;
	TIM11->CR1 |= TIM_CR1_CEN;
}

/* Остановка тактирования CKc & CKr.
 */
void Stop_Pulse(void){
	TIM10->CR1 &= (uint16_t)~TIM_CR1_CEN;
	TIM11->CR1 &= (uint16_t)~TIM_CR1_CEN;
}

/* Иниц. портов PE и PD для использования в виде паралельной шины данных
 * и упр. сигналов + светодиоды для kitDiscovery.
 * */
void Init_pData_Bus(void){
	GPIO_InitTypeDef Gpio_InitStruct;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE, ENABLE);

	//Светодиоды
	GPIO_StructInit(&Gpio_InitStruct);
	Gpio_InitStruct.GPIO_Pin  = LedMask;
	Gpio_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOD, &Gpio_InitStruct);

	//Сигналы Start и BusY
	GPIO_StructInit(&Gpio_InitStruct);
	Gpio_InitStruct.GPIO_Pin  = BusY | Start;
	Gpio_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD, &Gpio_InitStruct);

	//Настройка параллельной шины (Data [0...11] и DataRdy, выходы)
	GPIO_StructInit(&Gpio_InitStruct);
	Gpio_InitStruct.GPIO_Pin  = OutMask;
	Gpio_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	Gpio_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOE, &Gpio_InitStruct);
}

















