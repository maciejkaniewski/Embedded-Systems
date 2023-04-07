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