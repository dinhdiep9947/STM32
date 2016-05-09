/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* USER CODE BEGIN Includes */
#include "ssd1306.h"
#include "display.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c2;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
#define enter GPIO_PIN_12
#define opt 	GPIO_PIN_11
#define menu 	GPIO_PIN_7



/* Control */
_Bool time_tmp = 0, digit = 0, channel = 0, tmp_time_sample = 0, active_valve = 0;
static uint16_t  page = 1, scr = 1, back = 0; 
static uint16_t   time_dv, time_chuc, save_time = 1;
volatile uint32_t j = 0, i = 0, k = 0;       /* Using it for timer */
int16_t dem = 30, tmp_push = 0, mode = 0, tmp_channel = 0, time_uart = 30, van = 0;
char wifi[20], sensor[20];


/* UART */
uint8_t Rx_indx;
char buffer[20];
uint8_t Rx_data = '\0';
char  Rx_Buffer[20], tmp[20];
uint8_t Transfer_cplt;

/* Variables for Caculation */
float pH = 6.5, DO = 3.6, Temp = 38;
uint32_t ID = 1124;
char payload[30];


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C2_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM3_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_SYSTICK_Callback(void);
void send_data(void);
void quet_mode(void);
void auto_measure(void);
void action_valve(uint16_t _van);

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C2_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_TIM3_Init();

  /* USER CODE BEGIN 2 */
	
	__HAL_UART_FLUSH_DRREGISTER(&huart1);
	HAL_UART_Receive_DMA(&huart1, &Rx_data, 1);
	
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);			// Enable
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);			// GPIO0_ Flash: 0
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);		// RST
	HAL_Delay(100);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);

	if (SSD1306_Init() == 0)
	{
		while(1)
		{
			action_valve(1);
			HAL_Delay(50);
		}
	}
	SSD1306_flip();
	if (mode == 1)
		display(page, scr);
	
//	if (Transfer_cplt == 1)
//		{
//			sprintf(buffer, "%s", Rx_Buffer);
//			Transfer_cplt = 0;
//		}
//		quet_mode();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */
	if (Transfer_cplt == 1)
		{
			sprintf(buffer, "%s", Rx_Buffer);
			Transfer_cplt = 0;
		}
		quet_mode();

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* I2C2 init function */
void MX_I2C2_Init(void)
{

  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  HAL_I2C_Init(&hi2c2);

}

/* TIM2 init function */
void MX_TIM2_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 8000;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 99;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_Base_Init(&htim2);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig);

}

/* TIM3 init function */
void MX_TIM3_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 8000;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 60000;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_Base_Init(&htim3);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig);

}

/* USART1 init function */
void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart1);

}

/** 
  * Enable DMA controller clock
  */
void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : PA7 PA12 */
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PC5 PC7 PC8 PC9 
                           PC10 PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9 
                          |GPIO_PIN_10|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PC11 */
  GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10|GPIO_PIN_12, GPIO_PIN_SET);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

void quet_mode(void)
{
	if (mode == 0)
		{
			screen_check(dem, buffer, "Connected");
			if (dem == 0)
			{
				HAL_TIM_Base_Start_IT(&htim3);
				active_valve = 1;
				mode = 2;
				dem = 10;
			}
		}
		else if (mode == 1)
		{
			if (back == 4) 													
			{
				if (tmp_time_sample == 1)
					Time_Sample(time_chuc, time_dv, 3);
				else
					Time_Sample(time_chuc, time_dv, digit);
			}
			else if (back == 1)
			{
				if (tmp_push == 2)					
				{
					Active_Calib(dem);
					if (dem < 0)
					{
						dem = 10;
						tmp_push = 0;
					}
				}
				else
				{
					screen_PH();
				}
			}
			else if (back == 2)
			{
				if (tmp_push == 2)					
				{
					Active_Calib(dem);
					if (dem < 0)
					{
						dem = 10;
						tmp_push = 0;
					}
				}
				else
				{
					screen_DO();
				}
			
			}
			else if (back == 3)
			{
				if (tmp_push == 2)					
				{
					Active_Calib(dem);
					if (dem < 0)
					{
						dem = 10;
						tmp_push = 0;
					}
				}
				else
				{
					screen_Cond();
				}
			}
			else if (back == 0)
			{
				display(page, scr);
			}
			else if (back == 5)
			{
				Set_Channel(channel);	
			}
		}
		else if (mode == 2)
		{
			auto_measure();
		}
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	
	/* If press Menu */
	if (GPIO_Pin == menu)
	{
		HAL_TIM_Base_Stop_IT(&htim3);
		mode = 1;
		scr = 1;
		time_uart = 60;
		
		/* If press Menu in Page PH, DO, COND  */
		if (back == 0)		
		{
			if (page > 1)				
				page = 1;
		}
		else 
			back = 0;
	}
	
	/* If press Opt */
	if (GPIO_Pin == opt)
	{
		if (mode == 1)
		{
			if (back == 0)
			{
				scr++;
				if (page == 1)
				{
					if (scr > 3)			scr = 1;
				}
				else if (page == 2)
				{
					if (scr > 3)			scr = 1;
				}
				else if (page == 3)
				{
					if (scr > 2)			scr = 1;
				}
			}
			else if (back == 4)									/* If press OPT in Time_Sample */
			{
				HAL_TIM_Base_Start_IT(&htim2);
				tmp_time_sample = 0;
			}
			else if (back == 5)
			{	
				channel =! channel;
			}
		}
	}
	
	/* If press Enter */
	if (GPIO_Pin == enter)
	{
		if (mode == 1)
		{
			if (page == 1) 							/* Menu page */
			{
				if (scr == 1)					/* Calibration */		
				{
					page = 2;
				}
				else if (scr == 2)		/* Set-up */			
				{
					page = 3;
					scr = 1;
				}
				else if (scr == 3)		/* BACK */
				{
					mode = 0;
					active_valve = 1;
					HAL_TIM_Base_Start_IT(&htim3);
				}
			}
			else if (page == 2)					/* Calibration page */
			{
				if (back == 0)
				{
					if (scr == 1)					/* PH */				
					{
						back = 1;
					}
					else if (scr == 2)		/* DO */
					{
						back = 2;
					}
					else if (scr == 3)		/* COND */
					{
						back = 3;
					}
				}
			}
			else if (page == 3)							/* Set-up page */
			{
				if (back == 0)
				{
					if (scr == 2)				/*  */
					{
						time_chuc = save_time/10;
						time_dv = save_time%10;
						back = 4;
						digit = 0;	
						tmp_time_sample = 1;						
					}
					else if (scr == 1)
					{
						back = 5;		
					}
				}
				else if (back == 4)
				{
					save_time = time_chuc*10 + time_dv;
					tmp_time_sample = 1;
				}
			}
			
		/* Press Enter if you want to calibrate */
			/* tmp_push = 1 : the first time, tmp_push = 2 : the second time */
		if ((back == 1) || (back == 2) || (back == 3))  
			{
				tmp_push++;
			}
			
			/* Press Enter if you want to save time sample */
			if ((back == 4) && (time_tmp == 1))
			{
				//time_tmp = 0;
				save_time = time_chuc*10 + time_dv;
				digit = 3;
				//Time_Sample(save_time/10, save_time%10, 3);
			}
		}
	}
}

void HAL_SYSTICK_Callback(void)
{
	if (mode == 0)
	{
		i++;
		if(i > 1000)
		{
			dem--;
			i = 0;
		}
	}
	if ((back == 1) || (back == 2) || (back == 3))
	{
		if (tmp_push == 2)
		{
			i++;
			if (i > 1000)
			{
				dem--;
				i = 0;
			}
		}
	}
	if (mode == 2)
	{
		if (active_valve == 1)
		{
			i++;
			if (i > 1000)
			{
				time_uart--;
				i = 0;
			}
		}
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM2)
	{
		if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11) == 0)
		{
			j++;
			if ( j == 10)		
				{
					digit =! digit;
					j = 0;
				}
		}
		else
		{
			j = 0;
			if (digit == 0)									/* Hang don vi */
				{
					time_dv++;
					if (time_dv > 9)			time_dv = 0;
				}
				else if (digit == 1)														/* Hang chuc */
				{
					time_chuc++;
					if (time_chuc > 9)		time_chuc = 0;
				}
			HAL_TIM_Base_Stop_IT(&htim2);	
		}
	}
	if (htim->Instance == TIM3)
	{
		k++;
		if (k == save_time)
		{
			active_valve = 1;
			van++;
			if (van > 4)
				van = 1;
			k = 0;
		}
	}
		
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	__HAL_UART_FLUSH_DRREGISTER(&huart1);
	if (huart->Instance == USART1)
	{
		if (Rx_indx == 0)			{for (i = 0; i <20; i++ )		Rx_Buffer[i]=0;}
		if (Rx_data == '\n')
		{
			Rx_indx = 0;
			Transfer_cplt = 1;
		}
		else
		{
			Rx_Buffer[Rx_indx] = Rx_data;
			Rx_indx++;
		}
	}
}
void send_data(void)
{
	sprintf(payload,"%d,%2.2f,%2.2f,%2.2f", van, pH, DO, Temp);
	HAL_UART_Transmit(&huart1, (uint8_t *)payload, sizeof(payload), 5000);
}

void auto_measure(void)
{
	if (active_valve == 1)
	{
		if ( time_uart > 0)
		{
			Caculation(van, pH, DO, Temp, 0, 1);
			sprintf(tmp,"%d", time_uart);
			SSD1306_GotoXY(86, 0);
			SSD1306_Puts(tmp, &TM_Font_7x10, SSD1306_COLOR_WHITE);
			SSD1306_UpdateScreen();
			action_valve(van);
		}
		else
		{
			action_valve(0);
			pH = rand() % 10 + 1;
			Temp = rand() % 100;
			DO = rand() % 100;
			Caculation(van, pH, DO, Temp, 1, 1);
			time_uart = 30;
			send_data();
			active_valve = 0;
		}
	}
}

void action_valve(uint16_t _van)
{
	if (_van == 1)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_SET);
	}
	else if (_van == 2)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_SET);
	}
	else if (_van == 3)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_SET);
	}
	else if (_van == 4)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET);
	}
	else if (_van == 0)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_SET);
	}
}


/* USER CODE END 4 */

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
