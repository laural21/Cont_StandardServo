#include "stm32f7xx.h"                  // Device header
#include "stm32f7xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"                // Keil.STM32F746G-Discovery::Board Support:Graphic LCD
#include "Board_Touch.h"                // ::Board Support:Touchscreen
#include <stdlib.h>

static void SystemClock_Config(void);
extern GLCD_FONT GLCD_Font_16x24;
/*
Duty-cycle: 6% for fast clockwise rotation, 8% for fast counter-clockwise rotation.
(1.3 ms ON, 20 ms OFF, 1.7 ms ON, 20 ms OFF)

1. Choose pins
2. Activate and configure GPIO's and timers
3. Set appropriate PWM signal
*/

int main(void)
{
	short x, y;
	TOUCH_STATE state;
	
	SystemClock_Config();
	
	//Activate GPIOF, GPIOH
	RCC->AHB1ENR |= (1 << 5) | (1 << 7);

	//Activate TIM12 for Continuous Rotation Servo and TIM13 for Standard Servo
	RCC->APB2ENR |= (1 << 6) | (1 << 7);

	//AF mode
	GPIOH->MODER |= (2 << 12);
	GPIOF->MODER |= (2 << 16);
	
	//AF9 for TIM12 and TIM13
	GPIOH->AFR[0] |= (9 << 24);
	GPIOF->AFR[1] |= (9 << 0);

	TIM12->CCMR1 |= (6 << 4);
	TIM13->CCMR1 |= (6 << 4);
 
	//Connect to output
	TIM12->CCER |= (1 << 0);
	TIM13->CCER |= (1 << 0);

	//Set prescaler and ARR values
	TIM12->PSC = 71;
	TIM12->ARR = 65099;
	
	TIM13->PSC = 74;
	TIM13->ARR = 64524;
	
	//There is a constant difference between ARR and CCR, of 60,000, so the initial CCR values are:
	TIM12->CCR1 = 5099;
	TIM13->CCR1 = 4524;

	//Start counters
	TIM12->CR1 |= (1 << 0);
	TIM13->CR1 |= (1 << 0);

	GLCD_Initialize();
	GLCD_SetBackgroundColor(GLCD_COLOR_BLUE);
	GLCD_SetForegroundColor(GLCD_COLOR_YELLOW);
	GLCD_SetFont(&GLCD_Font_16x24);
	GLCD_ClearScreen();
	
	Touch_Initialize();
	
	x = GLCD_SIZE_X / 2;
	y = GLCD_SIZE_Y / 2;
	
	GLCD_DrawPixel(x, y);
	while(1)
	{
		Touch_GetState(&state);
		
		if (state.pressed == 1)
		{
			GLCD_SetForegroundColor(GLCD_COLOR_BLUE);
			GLCD_DrawPixel(x, y);
			
			x = state.x;
			y = state.y;
			GLCD_SetForegroundColor(GLCD_COLOR_YELLOW);
			GLCD_DrawPixel(x, y);
			
			//Position changes freqency
			TIM12->CCR1 = (x+1)*5099/640;
			TIM12->ARR = (x+1)*5099/640+60000;
			TIM13->CCR1 = (y+1)*4525/480;
			TIM13->ARR = (y+1)*4525/480+60000;
		}
	}
	return 0;
}

void SysTick_Handler (void)
{
    HAL_IncTick();
}

static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;

  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }

  /* Activate the OverDrive to reach the 216 MHz Frequency */
  ret = HAL_PWREx_EnableOverDrive();
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
}