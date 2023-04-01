#include "stm32f4xx.h"

void SysTick_Handler(void)
{
	GPIOD->ODR ^= GPIO_ODR_OD12;
	GPIOD->ODR ^= GPIO_ODR_OD13;
	GPIOD->ODR ^= GPIO_ODR_OD14;
	GPIOD->ODR ^= GPIO_ODR_OD15;
}

int main()
{
	// Enable the clock of port D of the GPIO
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; 
	
	// Set 12-15 pins as output
	GPIOD->MODER |= GPIO_MODER_MODER12_0;
	GPIOD->MODER |= GPIO_MODER_MODER13_0;
	GPIOD->MODER |= GPIO_MODER_MODER14_0;
	GPIOD->MODER |= GPIO_MODER_MODER15_0;
	
	// Set SysTick LOAD
	SysTick->LOAD = 2000000 - 1;
	
	// Clear current value
	SysTick->VAL = 0;
	
	// Enable SysTick
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	
	// Enable SysTick interrupt
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;

	while(1)
	{

	}
}