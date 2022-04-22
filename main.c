#include <stdio.h>
#include "stm32f4xx.h"

// APB1 (bit 17)	: 	USART2
// alternate func	:	AFRL bit 7
// USART_BRR
// USART_CR1 		:	bit 13, 3


void init_USART2(void);
void write_USART2(int ch);
void delayMS(int delay);

int __io_putchar(int ch)
{
	write_USART2(ch);
	return ch;
}

void init_interrupt(void)
{
	RCC->AHB1ENR |= (1<<2);			// Enable Port C
	RCC->APB2ENR |= (1<<14);		// Enable SYSCFG Clock

	GPIOC->MODER |= 0x00000000;		// set Port(s) C to input

	SYSCFG->EXTICR[0] |= (1<<1);	// select port C for EXTI0 in configuration register 1
	SYSCFG->EXTICR[2] |= (1<<5);	// select port C for EXTI9 in configuration register 3

	EXTI->IMR |= (1<<0);			// unmask EXTI0
	EXTI->IMR |= (1<<9);			// unmask EXTI9


	// select rising edge trigger
	EXTI->RTSR |= (1<<0);
	EXTI->RTSR |= (1<<9);

	NVIC_EnableIRQ(EXTI0_IRQn);
	NVIC_EnableIRQ(EXTI9_5_IRQn);

	/*
	 * Board User-Button B1
	 *
	SYSCFG->EXTICR[3] |= (1<<5);	// select port C for EXTI13 in configuration register 4
	EXTI->IMR |= (1<<13);			// unmask EXTI13
	EXTI->FTSR |= (1<<13);			// select falling edge trigger
	NVIC_EnableIRQ(EXTI15_10_IRQn);
	*/
}

void init_USART2(void)
{
	// enable clock for USART
	RCC->APB1ENR |= (1<<17);

	// enable clock for GPIO_A to use alternate function to enable USART
	RCC->AHB1ENR |= (1<<0);

	// 0 => alternate function low; 1 => alternate function high
	GPIOA->AFR[0] = 0x0700; // (AFRL2) 0111 0000 0000
	GPIOA->MODER |= (1<<5);	// Set PA2 to alternate function (AFRL2 correspond to PA2)

	USART2->BRR = 0x0683; 	// Set Baudrate to 9600 @16MhZ
	USART2->CR1 |= (1<<3); 	// Enable TE
	USART2->CR1 |= (1<<13); // Enable UE
}

void write_USART2(int ch)
{
	// wait while TX buffer is empty
	while(!(USART2->SR & 0x0080)) {}

	USART2->DR = (ch & 0xFF);
}

void EXTI0_IRQHandler(void)
{
	char text[] = "d3b3ecc2-ced7-461a-ac96-04f6d99d9d34\n";
	printf(text);
	EXTI->PR |= (1<<0);			// clear interrupt
}

void EXTI9_5_IRQHandler(void)
{
	char text[] = "2e11f26e-d155-42d7-be96-d8dec1e6c69e\n";
	printf(text);
	EXTI->PR |= (1<<9);
}

/*
 * Board User-Button B1
 *
void EXTI15_10_IRQHandler(void)
{
	char text[] = "d3b3ecc2-ced7-461a-ac96-04f6d99d9d34\n";
	printf(text);
	EXTI->PR |= (1<<13);			// clear interrupt
}
*/

int main(void)
{
	init_USART2();
	init_interrupt();

	while(1)
	{
		// printf("§$&\r\n");
	}

	return 0;
}

