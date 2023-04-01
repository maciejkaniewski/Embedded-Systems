#include "stm32f4xx.h"

void ConfigurePllClock(unsigned char frequency);

volatile uint32_t systickCounter = 0;

void SysTick_Handler(void)
{
	systickCounter++;
	
	if(systickCounter == 1000)
	{
		GPIOD->ODR ^= GPIO_ODR_OD12;
		GPIOD->ODR ^= GPIO_ODR_OD13;
		GPIOD->ODR ^= GPIO_ODR_OD14;
		GPIOD->ODR ^= GPIO_ODR_OD15;
		systickCounter = 0;
	}
}

void ConfigurePllClock(unsigned char frequency)
{
	
	// Enable HSI clock
	RCC->CR |= RCC_CR_HSION;
	while(!(RCC->CR & RCC_CR_HSIRDY));
	
	// Set HSI as system clock
	RCC->CFGR &= ~RCC_CFGR_SW;
	while(RCC->CFGR & RCC_CFGR_SWS_HSI);
	
	// Enable HSE clock
	RCC->CR |= RCC_CR_HSEON;    				
	while(!(RCC->CR & RCC_CR_HSERDY));

	//Disable PLL
	RCC->CR &= ~RCC_CR_PLLON;
	
	// Configure PLL 
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;   // PLL source is HSE (8 MHz crystal)

	// Set PLLM divider to 4 so 8MHz/4 = 2MHz on VCO input
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM); // Clear PLLM
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_2; // Set divider to 4
		
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLN); // Clear PLLN 
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLP); // Clear PLLM - 00 is divider by 2
	
	if(frequency > 50)
	{
		// Set PLLN multiplier to frequency so 2MHz * frequency on VCO output
		RCC->PLLCFGR |= (frequency << RCC_PLLCFGR_PLLN_Pos);
	}
	else if(frequency > 25)
	{
		// Set PLLN multiplier to frequency so 2MHz * frequency on VCO output
		RCC->PLLCFGR |= ((2*frequency) << RCC_PLLCFGR_PLLN_Pos);
	
		// Set PLLP divider to 4
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLP_0;
	}
	else
	{
		// Set PLLN multiplier to frequency so 2MHz * frequency on VCO output
		RCC->PLLCFGR |= ((4*frequency) << RCC_PLLCFGR_PLLN_Pos);
	
		// Set PLLP divider to 8
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLP_0;
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLP_1;
	}

	// Enable PLL 
	RCC->CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY));

	// Configure flash latency 
	FLASH->ACR &= ~(FLASH_ACR_LATENCY);
	
	if(frequency > 150)
	{
		FLASH->ACR |= FLASH_ACR_LATENCY_5WS;
	}
	else if(frequency > 120)
	{
		FLASH->ACR |= FLASH_ACR_LATENCY_4WS;
	}
	else if(frequency > 90)
	{
		FLASH->ACR |= FLASH_ACR_LATENCY_3WS;
	}
	else if(frequency > 60)
	{
		FLASH->ACR |= FLASH_ACR_LATENCY_2WS;
	}
	else if(frequency > 30)
	{
		FLASH->ACR |= FLASH_ACR_LATENCY_1WS;
	}
	else
	{
		FLASH->ACR |= FLASH_ACR_LATENCY_0WS;
	}
	
	// Select PLL as system clock source 
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while(!(RCC->CFGR & RCC_CFGR_SWS_PLL));
	
	// Disable HSI clock
	RCC->CR &= ~RCC_CR_HSION;
}


int main()
{
	
	ConfigurePllClock(168);
	
	SystemCoreClockUpdate();
	
	uint32_t validateFrequency = SystemCoreClock;
	
	// Enable the clock of port D of the GPIO
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; 
	
	// Set 12-15 pins as output
	GPIOD->MODER |= GPIO_MODER_MODER12_0;
	GPIOD->MODER |= GPIO_MODER_MODER13_0;
	GPIOD->MODER |= GPIO_MODER_MODER14_0;
	GPIOD->MODER |= GPIO_MODER_MODER15_0;
	
	// Configure SysTick
	SysTick->LOAD = SystemCoreClock/1000 - 1; 
	
	// Clear current value
	SysTick->VAL = 0;
	
	// Enable SysTick
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	
	// Enable SysTick interrupt
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	
	// Set SysTick source
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
	
	while(1)
	{

	}
}