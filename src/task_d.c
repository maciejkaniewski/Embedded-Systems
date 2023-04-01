#include "stm32f4xx.h"

void SysTick_Handler(void)
{
	GPIOD->ODR ^= GPIO_ODR_OD12;
}

int main()
{
	// Enable the clock of port D of the GPIO
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	
	// Set pin 12 as output
	GPIOD->MODER |= GPIO_MODER_MODER12_0;
	
	// Enable HSE clock
	RCC->CR |= RCC_CR_HSEON_Msk;
	while(!(RCC->CR & RCC_CR_HSERDY_Msk));
	
	// Set HSE as system clock
	RCC->CFGR |= RCC_CFGR_SW_HSE;
	while(!(RCC->CFGR & RCC_CFGR_SWS_HSE));
	
	// Disable HSI clock
	RCC->CR &= ~RCC_CR_HSION_Msk;
	while(RCC->CR & RCC_CR_HSIRDY_Msk);
	
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