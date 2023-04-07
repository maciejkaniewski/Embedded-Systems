#include "stm32f4xx.h"

void ConfigurePllClock(unsigned char frequency);
void ConfigureTIM4_PWM(void);

void ConfigureTIM4_PWM(void)
{
	// Set HPRE: AHB prescaler to 1
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
	
	// Set APB1 prescaler to 4 so 168MHz/4=42MHz
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
	
	// Set APB2 prescaler to 2 so 168MHz/2=84MHz
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;
	
	// Enable TIM4
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	
	// Configure TIM4 Channel 1 as PWM
	// Reference manual RM0090 p.637
	TIM4->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
	
	// Configure TIM4 Channel 2 as PWM
	TIM4->CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2;
	
	// Configure TIM4 Channel 3 as PWM
	TIM4->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2;
	
	// Configure TIM4 Channel 4 as PWM
	TIM4->CCMR2 |= TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2;
	
	// Enable capture/compare output for all channels
	// Reference manual RM0090 p.641
	TIM4->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;
	
	// Set TIM4 ARR
	TIM4->ARR = 99;
	
	// Set TIM4 prescaler
	TIM4->PSC = 83;

	// Set the duty cycle for each channel
	TIM4->CCR1 = 15;
	TIM4->CCR2 = 50;
	TIM4->CCR3 = 75;
	TIM4->CCR4 = 100;

	// Enable counter
	TIM4->CR1 |= TIM_CR1_CEN;
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

	// Disable PLL
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
		// Set PLLN multiplier to frequency so 2MHz * 2 * frequency on VCO output
		RCC->PLLCFGR |= ((2*frequency) << RCC_PLLCFGR_PLLN_Pos);
	
		// Set PLLP divider to 4
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLP_0;
	}
	else
	{
		// Set PLLN multiplier to frequency so 2MHz * 4 * frequency on VCO output
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
	FLASH->ACR |= FLASH_ACR_PRFTEN;
	FLASH->ACR |= FLASH_ACR_ICEN;
	FLASH->ACR |= FLASH_ACR_DCEN;
	
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
	
	ConfigureTIM4_PWM();
	
	uint32_t validateFrequency = SystemCoreClock;
	
	// Enable the clock of port D of the GPIO
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; 
	
	// Set 12-15 pins in alternate function mode
	// Reference manual RM0090 p.281
	GPIOD->MODER |= GPIO_MODER_MODER12_1;
	GPIOD->MODER |= GPIO_MODER_MODER13_1;
	GPIOD->MODER |= GPIO_MODER_MODER14_1;
	GPIOD->MODER |= GPIO_MODER_MODER15_1;
	
	// Set AF2(TIM3..5) to 12-15 pins as alternate function
	// Reference manual RM0090 p.272, 286
	GPIOD->AFR[1] |= GPIO_AFRH_AFSEL12_1;
	GPIOD->AFR[1] |= GPIO_AFRH_AFSEL13_1;
	GPIOD->AFR[1] |= GPIO_AFRH_AFSEL14_1;
	GPIOD->AFR[1] |= GPIO_AFRH_AFSEL15_1;
	
	// Set high speed to 12-15 pins (not necessarily required)
	// Reference manual RM0090 p.282
	GPIOD->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR12_1;
	GPIOD->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR13_1;
	GPIOD->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR14_1;
	GPIOD->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR15_1;
	
	while(1)
	{

	}
}