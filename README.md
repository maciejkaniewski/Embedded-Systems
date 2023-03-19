# Embedded-Systems

Repository Embedded-Systems presents suggested solutions to tasks that had to be completed in the first semester of my Master's studies within the Embedded-Systems course at the Wroclaw University of Technology in the field of Control Engineering and Robotics. The tasks were implemented on the `STM32F407G-DISC1` board equipped with the `STM32F407VG` core. The `Keil µVision 5 IDE` was used to program the board.

## Table of Contents

  - [Project Setup](#project-setup)
  - [Task *A*](#task-a)
  - [Task *B*](#task-b)
  - [Task *C*](#task-c)

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

The task is analogous to the previous one, but instead of checking the `COUNTFLAG` register, the code related to the change of the diode state should be implemented in the interrupt.

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
	
	// Set 12-15 pin as output
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