# Embedded-Systems

Repository Embedded-Systems presents suggested solutions to tasks that had to be completed in the first semester of my Master's studies within the Embedded Systems course at the Wroclaw University of Technology in the field of Control Engineering and Robotics. The tasks were implemented on the `STM32F407G-DISC1` board equipped with the `STM32F407VG` core. The `Keil µVision 5 IDE` was used to program the board.

## Table of Contents

  - [Project Setup](#project-setup)
  - [Task *A*](#task-a) *(turn on LED)*
  - [Task *B*](#task-b) *(blink LED with SysTick flag)*
  - [Task *C*](#task-c) *(blink LED with SysTick interrupt)*
  - [Task *D*](#task-d) *(configure HSE as system clock)*
  - [Task *E*](#task-e) *(configure PLL)*
  - [Task *F*](#task-f) *(configure TIM4 with interrupt)*
  - [Task *G*](#task-g) *(configure TIM4 in PWM mode)*
  - [Bonus Task](#bonus-task) *(read WHO_AM_I of accelerometer through SPI)*

## Project Setup

The first step is to create a new project. From the `Keil µVision 5` program bar, select `Project` and then `New µVision Project...`. Then a window will appear where you can define the name of the project.

![New µVision Project \label{mylabel}](./images/img1.png "New µVision Project")

After defining the name of the project, select the device to be programmed. In this case, select `STMicroelectronics -> STM32F4 Series -> STM32F407 -> STM32F407VG -> STM32F407VGTx`

![Select Device for Target](./images/img2.png "Select Device for Target")

Then the `Manage Run-Time Environment` window appears, in which checkboxes presented in the screenshot below should be checked. In addition, in `Board Support`, select the `STM32F4-Discovery` option.

![Manage Run-Time Environment](./images/img3.png "Manage Run-Time Environment")

The next step is to set the appropriate settings for the debugger. Select `Options for Target...` from the program bar.

![Options for Target](./images/img4.png "Options for Target")

There, go to the `Debug` tab and select the option shown in the screenshot below, i.e. `ST-Link Debugger`.

![Debug](./images/img5.png "Debug")

To verify the correct connection with the board, click the `Settings` button visible to the right of the name of the selected debugger. If everything works correctly, the window should contain the following information.

![Cortex-M Target Driver Setup](./images/img6.png "Cortex-M Target Driver Setup")

To make it easier to work with the board, you can go to the `Flash Download` tab and check the `Reset and Run` option. This will automatically reset the board when loading a new program.

![Flash Download](./images/img7.png "Flash Download")

The last step of project configuration is to create the `main.c` file in which the program code will be written.

![Add New Item](./images/img8.png "Add New Item")

![C File](./images/img9.png "C File")

![Main](./images/img10.png "Main")

## Task *A*

The task is to turn on the selected LEDs available on the board. In the example shown below, all LEDs are turned on.

```c
#include "stm32f4xx.h"

int main()
{
    // Enable the clock of port D of the GPIO
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

    // Set 12-15 pins as output
    GPIOD->MODER |= GPIO_MODER_MODER12_0;
    GPIOD->MODER |= GPIO_MODER_MODER13_0;
    GPIOD->MODER |= GPIO_MODER_MODER14_0;
    GPIOD->MODER |= GPIO_MODER_MODER15_0;

    // Turn on LEDs
    GPIOD->ODR |= GPIO_ODR_OD12;
    GPIOD->ODR |= GPIO_ODR_OD13;
    GPIOD->ODR |= GPIO_ODR_OD14;
    GPIOD->ODR |= GPIO_ODR_OD15;
}
```
## Task *B*

The task is to blink selected LEDs available on the board using the system tick timer `SysTick`. The LED should be on for 1 second and then off for another second. To achieve this, you need to configure `SysTick` and check the status of the `COUNTFLAG` bit in `SysTick->CTRL` register. The documentation says that `SysTick` by default runs at the `HSI` frequency divided by 8. Thus, it is *16MHz/8 = 2MHz*. To calculate the value of the `LOAD` register, you can use the following formula:

```
SysTick->LOAD = System Clock x Delay Desired
```

Applying the above formula in our problem, we get:

```
SysTick->LOAD = 2MHz x 1s = 2 000 000
```

In the code, the obtained value should be reduced by 1, because the counting starts from zero.

```c
#include "stm32f4xx.h"

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
	
	while(1)
	{
		if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
		{
			GPIOD->ODR ^= GPIO_ODR_OD12;
			GPIOD->ODR ^= GPIO_ODR_OD13;
			GPIOD->ODR ^= GPIO_ODR_OD14;
			GPIOD->ODR ^= GPIO_ODR_OD15;
		}
	}
}
```

The screenshot below shows the logical states of the output controlling one of the four LEDs.

![Logic](./images/img11.png "Logic States")

## Task *C*

The task is analogous to the previous one, but instead of checking the `COUNTFLAG` bit, the code related to the change of the diode state should be implemented in the interrupt.

```C
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
```

## Task *D*

In this task, you configure the `HSE` *(high-speed external oscillator)* and `HSI` *(high-speed internal oscillator)* clocks. `HSI` is the default system clock on the board, so it is automatically configured at device startup. To check the configuration, you can activate the `HSE` clock, set it as the system clock, and then disable the `HSI` clock. The clock frequency of the external oscillator is *8MHz*, so the `SysTick` clock is *8MHz/8 = 1MHz*. Thus, using the example of blinking a LED from the previous tasks, the time of its high and low state should be extended to 2 seconds while maintaining the same `SysTick` configuration.

```C
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
```

![Logic](./images/img12.png "Logic States")

## Task *E*

Configure `PLL`. You can use `SystemCoreClockUpdate()` function to validate frequency of the clock. But if you want to get reliable results you have to change default value of `HSE_VALUE` constant in `system_stm32f4xx.c` file.

Defualt configuration:

```C
#if !defined  (HSE_VALUE) 
  #define HSE_VALUE    ((uint32_t)25000000) /*!< Default value of the External oscillator in Hz */
#endif /* HSE_VALUE */
```

Changed HSE_VALUE to 8MHz:
```C
#if !defined  (HSE_VALUE) 
  #define HSE_VALUE    ((uint32_t)8000000) /*!< Default value of the External oscillator in Hz */
#endif /* HSE_VALUE */
```

The step described above is not necessary but it would help verify whether the frequency of the clock is set correctly.

```C
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
```

## Task *F*

Configure `TIM4` interrupt to blink LEDs. To calculate timer output frequency and choose proper values for `ARR` and `PSC` registers you can use the following equation:

```
timer_frequency = (TIMER_CLOCK)/(PSC+1)(ARR+1)
```

For `TIM4`, the TIMER_CLOCK source is APB2. In the code presented below, the dividers were set so as to obtain the maximum frequency on APB2, i.e. 84MHz. Therefore, substituting the selected parameters into the formula, we get:

```
timer_frequency = (84MHz)/(20999+1)*(1999+1) = 2Hz
```

```C
#include "stm32f4xx.h"

void ConfigurePllClock(unsigned char frequency);
void ConfigureTIM4(void);

void TIM4_IRQHandler(void)
{
	// Clear interrupt flag
	TIM4->SR &=~TIM_SR_UIF;

	GPIOD->ODR ^= GPIO_ODR_OD12;
	GPIOD->ODR ^= GPIO_ODR_OD13;
	GPIOD->ODR ^= GPIO_ODR_OD14;
	GPIOD->ODR ^= GPIO_ODR_OD15;
}

void ConfigureTIM4(void)
{
	// Set HPRE: AHB prescaler to 1
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
	
	// Set APB1 prescaler to 4 so 168MHz/4=42MHz
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
	
	// Set APB2 prescaler to 2 so 168MHz/2=84MHz
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;
	
	// Enable TIM4
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	
	// Enable interrupt
	NVIC_EnableIRQ(TIM4_IRQn);
	
	// Set TIM4 ARR
	TIM4->ARR = 1999;
	
	// Set TIM4 prescaler
	TIM4->PSC = 20999;
	
	// Enable TIM4 interrupt
	TIM4->DIER |= TIM_DIER_UIE;
	
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
	
	ConfigureTIM4();
	
	uint32_t validateFrequency = SystemCoreClock;
	
	// Enable the clock of port D of the GPIO
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; 
	
	// Set 12-15 pins as output
	GPIOD->MODER |= GPIO_MODER_MODER12_0;
	GPIOD->MODER |= GPIO_MODER_MODER13_0;
	GPIOD->MODER |= GPIO_MODER_MODER14_0;
	GPIOD->MODER |= GPIO_MODER_MODER15_0;
	
	while(1)
	{

	}
}
```

## Task *G*

Configure `TIM4` in `PWM` mode. Set a different duty for each channel so that the LEDs light up with different brightness. To calculate the frequency of the PWM output, we use the same formula from the previous task:

```
pwm_output = (84MHz)/(83+1)*(999+1) = 10kHz
```

In order to calculate the signal duty, we can use this formula:

```
duty = ((TIMx->CCRx)/(ARR+1))*100
```

In the case of the following example, the duty for subsequent channels will be:

```
duty_channel_1 = ((15)/(100))*100 = 15%
duty_channel_2 = ((50)/(100))*100 = 50%
duty_channel_3 = ((75)/(100))*100 = 75%
duty_channel_4 = ((100)/(100))*100 = 100%
```

Below the fragment with the code, there are graphs of logical states of individual channels, confirming the calculated duty.

```C
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
```
![LogicGreen](./images/img13.png "Logic States Green")
![LogicOrange](./images/img14.png "Logic States Orange")
![LogicRed](./images/img15.png "Logic States Red")
![LogicBlue](./images/img16.png "Logic States Blue")

## Bonus Task

Read `WHO_AM_I` register from the built-in accelerometer. The board used in the exercise can be equipped with one of two accelerometers: 

- LIS3DSH - `WHO_AM_I = 0x3F`
- LIS302DL - `WHO_AM_I = 0x3B`

Depending on the value read in the register, the appropriate LED will light up. Green for LIS3DSH accelerometer, orange for LIS302DL, and red when none of them are recognized.

```C
#include "stm32f4xx.h"

#define MOTION_SENSOR_DUMMY_BYTE 0x00
#define MOTION_SENSOR_WHO_AM_I 0x0F
#define MOTION_SENSOR_READ 0x80

#define LIS3DSH_WHO_AM_I_ID 0x3F
#define LIS302DL_WHO_AM_I_ID 0x3B

void ConfigureSPI1(void);
uint32_t read_WHO_AM_I(void);

void ConfigureSPI1(void)
{	
	// Enable the clock of port A of the GPIO
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	
	// Set 5-7pins in alternate function mode
	// Reference manual RM0090 p.281
	GPIOA->MODER |= GPIO_MODER_MODER5_1;
	GPIOA->MODER |= GPIO_MODER_MODER6_1;
	GPIOA->MODER |= GPIO_MODER_MODER7_1;
	
	// Set AF5(SPI1) to 5-7 pins as alternate function
	// Reference manual RM0090 p.272, 286
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL5_0 | GPIO_AFRL_AFSEL5_2;
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL6_0 | GPIO_AFRL_AFSEL6_2;
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL7_0 | GPIO_AFRL_AFSEL7_2;
	
	// Set high speed to 5-7 pins
	// Reference manual RM0090 p.282
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR5_1;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6_1;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR7_1;

	// Enable the clock of port E of the GPIE
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
	
	// Set 3 pin in output mode
	// Reference manual RM0090 p.281
	GPIOE->MODER |= GPIO_MODER_MODER3_0;
	
	// Set high speed to 3 pin
	// Reference manual RM0090 p.282
	GPIOE->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR3_1;
	
	// Set pin 3 to high
	GPIOE->ODR |= GPIO_ODR_OD3;

	// Enable the clock of the SPI1
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	
	// Configure SPI1
	// Reference manual RM0090 p.916
	SPI1->CR1 |= SPI_CR1_BR_0 | SPI_CR1_BR_1; // F_PCLK / 16
	SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI; // NSS management
	SPI1->CR1 |= SPI_CR1_CPHA | SPI_CR1_CPOL; // Set Clock Polarity and Phase
	SPI1->CR1 |= SPI_CR1_MSTR; // Set as master
	SPI1->CR1 |= SPI_CR1_SPE; // Enable SPI
}

uint32_t read_WHO_AM_I(void)
{
    uint32_t rx_data;
	
    GPIOE->ODR &= ~GPIO_ODR_OD3;

    SPI1->DR = MOTION_SENSOR_READ | MOTION_SENSOR_WHO_AM_I;
    while(!(SPI1->SR & SPI_SR_RXNE));
    rx_data = SPI1->DR;

    SPI1->DR = MOTION_SENSOR_DUMMY_BYTE;
    while(!(SPI1->SR & SPI_SR_RXNE));
    rx_data = SPI1->DR;

    GPIOE->ODR |= GPIO_ODR_OD3; 

    return rx_data;
}

int main()
{

	uint32_t whoami = 0;
	
	// Enable the clock of port D of the GPIO
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

	// Set 12-14 pins as output
	GPIOD->MODER |= GPIO_MODER_MODER12_0;
	GPIOD->MODER |= GPIO_MODER_MODER13_0;
	GPIOD->MODER |= GPIO_MODER_MODER14_0;
	
	ConfigureSPI1();

	whoami = read_WHO_AM_I();
	
	if(whoami == LIS3DSH_WHO_AM_I_ID)
	{
		GPIOD->ODR |= GPIO_ODR_OD12;
	}
	else if(whoami == LIS302DL_WHO_AM_I_ID)
	{
		GPIOD->ODR |= GPIO_ODR_OD13;
	}
	else
	{
		GPIOD->ODR |= GPIO_ODR_OD14;
	}
	
	
	while(1)
	{

	}
}
```