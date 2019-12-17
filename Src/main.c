/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f1xx_hal.h"
#include "vs1003.h"
#include <stdio.h>
#include "beep.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint8_t  SW1_flag =0;
uint8_t  SW2_flag =0;
uint8_t  SW3_flag =0;
uint8_t  SW4_flag =0;




FATFS fs;
FATFS *pfs;
FIL fil;
FRESULT fres;
DWORD fre_clust;
uint32_t total, free;
char buffer[100];
uint8_t retSD;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define GPIO_TEST 0
#define LINE_OUT_TEST 1
#define LINE_IN_TEST 0
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */
int _write(int file, char *ptr, int len)
{
       HAL_UART_Transmit(&huart2,(uint8_t *)ptr, (uint16_t)len, 0xFFFFFFFF);
       return len;
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#define delay_ms HAL_Delay
void delay_us(uint32_t us){  // 72MHz
    if(us>1){
        uint32_t count=us*8-6;
        while(count--); //asm volatile("nop");
    }
    else{
        uint32_t count=2;
        while(count--); // asm volatile("nop");
    }
}

#define ADDRESS   0x27<<1
#define RS1_EN1   0x05
#define RS1_EN0   0x01
#define RS0_EN1   0x04
#define RS0_EN0   0x00
#define BackLight 0x08

// RS-Q0 / RW-Q1 / EN-Q2 / BackLight-Q3 / D4-Q4 / D5-Q5 / D6-Q6 / D7-Q7
void LCD_DATA(uint8_t data) {
  uint8_t temp=(data & 0xF0)|RS1_EN1|BackLight;
  while(HAL_I2C_Master_Transmit(&hi2c1, ADDRESS, &temp, 1, 1000)!=HAL_OK);
  temp=(data & 0xF0)|RS1_EN0|BackLight;
  while(HAL_I2C_Master_Transmit(&hi2c1, ADDRESS, &temp, 1, 1000)!=HAL_OK);
  delay_us(4);

  temp=((data << 4) & 0xF0)|RS1_EN1|BackLight;
  while(HAL_I2C_Master_Transmit(&hi2c1, ADDRESS, &temp, 1, 1000)!=HAL_OK);
  temp = ((data << 4) & 0xF0)|RS1_EN0|BackLight;
  while(HAL_I2C_Master_Transmit(&hi2c1, ADDRESS, &temp, 1, 1000)!=HAL_OK);
  delay_us(50);
}

void LCD_CMD(uint8_t cmd) {
  uint8_t temp=(cmd & 0xF0)|RS0_EN1|BackLight;
  while(HAL_I2C_Master_Transmit(&hi2c1, ADDRESS, &temp, 1, 1000)!=HAL_OK);
  temp=(cmd & 0xF0)|RS0_EN0|BackLight;
  while(HAL_I2C_Master_Transmit(&hi2c1, ADDRESS, &temp, 1, 1000)!=HAL_OK);
  delay_us(4);

  temp=((cmd << 4) & 0xF0)|RS0_EN1|BackLight;
  while(HAL_I2C_Master_Transmit(&hi2c1, ADDRESS, &temp, 1, 1000)!=HAL_OK);
  temp=((cmd << 4) & 0xF0)|RS0_EN0|BackLight;
  while(HAL_I2C_Master_Transmit(&hi2c1, ADDRESS, &temp, 1, 1000)!=HAL_OK);
  delay_us(50);
}

void LCD_CMD_4bit(uint8_t cmd) {
  uint8_t temp=((cmd << 4) & 0xF0)|RS0_EN1|BackLight;
  while(HAL_I2C_Master_Transmit(&hi2c1, ADDRESS, &temp, 1, 1000)!=HAL_OK);
  temp=((cmd << 4) & 0xF0)|RS0_EN0|BackLight;
  while(HAL_I2C_Master_Transmit(&hi2c1, ADDRESS, &temp, 1, 1000)!=HAL_OK);
  delay_us(50);
}

void LCD_INIT(void) {
  delay_ms(100);

  LCD_CMD_4bit(0x03); delay_ms(5);
  LCD_CMD_4bit(0x03); delay_us(100);
  LCD_CMD_4bit(0x03); delay_us(100);
  LCD_CMD_4bit(0x02); delay_us(100);
  LCD_CMD(0x28);  // 4 bits, 2 line, 5x8 font
  LCD_CMD(0x08);  // display off, cursor off, blink off
  LCD_CMD(0x01);  // clear display
  delay_ms(3);
  LCD_CMD(0x06);  // cursor movint direction
  LCD_CMD(0x0C);  // display on, cursor off, blink off
}

void LCD_XY(char x, char y) {
  if      (y == 0) LCD_CMD(0x80 + x);
  else if (y == 1) LCD_CMD(0xC0 + x);
  else if (y == 2) LCD_CMD(0x94 + x);
  else if (y == 3) LCD_CMD(0xD4 + x);
}

void LCD_CLEAR(void) {
  LCD_CMD(0x01);
  delay_ms(2);
}

void LCD_PUTS(char *str) {
  while (*str) LCD_DATA(*str++);
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  char str[20];
  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_SPI2_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_FATFS_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */
  LCD_INIT();

  VS1003_Init();
  VS1003_SoftReset();



  BYTE buf[32] = "Hello world ";
  uint32_t bw,br;


  if(f_mount(&fs, "", 0) == FR_OK)
   {
	  LCD_XY(0,0);
	  LCD_PUTS("1111");
   }
   /* Open file to write */
//   if(f_open(&fil, "three.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE) != FR_OK)
//     _Error_Handler(__FILE__, __LINE__);

  unsigned char filename[20] = "0:/1.mp3";
  uint16_t index = 0;
  LCD_CLEAR();
  if((retSD = f_open(&fil,filename, FA_OPEN_EXISTING | FA_READ ) == FR_OK))
  {

	  sprintf(str,"%s opend",filename);
	  LCD_XY(0,0);
	  LCD_PUTS(str);
  }else
  {
	  sprintf(str,"%s error",filename);
	  LCD_XY(0,0);
	  LCD_PUTS(str);
  }

  printf("7878");

  uint8_t pp_flag = 0;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  	  if(MP3_DREQ == 1) // 코덱이 데이터를 받을 수 있는 상태 일때
	  	  {
	  		  if(pp_flag)
	  		  {
				  f_read(&fil, buf, 32, &br);
				  if(br >=32)
				  {
					  VS1003_WriteData(&buf[0]);
				  }else
				  {

				  }
	  		  }

	  	  }
		if(SW1_flag)
		{
			SW1_flag = 0;

			index = index == 0 ? 4 :index-1;
			switch(index)
			{
			case 0:strcpy(filename, "0:/1.mp3"); break;
			case 1:strcpy(filename, "0:/2.mp3"); break;
			case 2:strcpy(filename, "0:/3.mp3"); break;
			case 3:strcpy(filename, "0:/4.mp3"); break;
			case 4:strcpy(filename, "0:/5.mp3"); break;
			}

			f_close(&fil);

			if((retSD = f_open(&fil,filename, FA_OPEN_EXISTING | FA_READ )) == FR_OK)
			{
				  sprintf(str,"%s opend",filename);
				  LCD_XY(0,0);
				  LCD_PUTS(str);
			  }else
			  {
				  sprintf(str,"%s error",filename);
				  LCD_XY(0,0);
				  LCD_PUTS(str);
			  }
			VS1003_SoftReset();
		}
		if(SW3_flag)
		{
			SW3_flag = 0;

			index = index == 4 ? 0 :index+1;
			switch(index)
			{
			case 0:strcpy(filename, "0:/1.mp3"); break;
			case 1:strcpy(filename, "0:/2.mp3"); break;
			case 2:strcpy(filename, "0:/3.mp3"); break;
			case 3:strcpy(filename, "0:/4.mp3"); break;
			case 4:strcpy(filename, "0:/5.mp3"); break;
			}

			f_close(&fil);

			if((retSD = f_open(&fil,filename, FA_OPEN_EXISTING | FA_READ )) == FR_OK)
			{
				  sprintf(str,"%s opend",filename);
				  LCD_XY(0,0);
				  LCD_PUTS(str);
			  }else
			  {
				  sprintf(str,"%s error",filename);
				  LCD_XY(0,0);
				  LCD_PUTS(str);
			  }
			VS1003_SoftReset();
		}
	  	  if(SW2_flag)
	  	  {
	  		SW2_flag = 0;
	  		if(pp_flag)
	  		{
	  			pp_flag = 0;
	  			LCD_XY(0,1);
				LCD_PUTS("paused ");
	  		}
	  		else
	  		{
	  			pp_flag = 1;
	  			LCD_XY(0,1);
				LCD_PUTS("playing  ");

	  		}
	  	  }




  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* EXTI1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);
  /* EXTI2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);
  /* EXTI3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);
  /* EXTI4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);
}

/* USER CODE BEGIN 4 */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}



void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	static uint32_t temp;
	if(GPIO_Pin == GPIO_PIN_1)
	{
		if((HAL_GetTick()-temp)>200) SW1_flag =1;
		while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1)==GPIO_PIN_RESET);
		temp = HAL_GetTick();
	}

	if(GPIO_Pin == GPIO_PIN_2)
	{
		if((HAL_GetTick()-temp)>200) SW2_flag =1;
		while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_2)==GPIO_PIN_RESET);
		temp = HAL_GetTick();
	}

	if(GPIO_Pin == GPIO_PIN_3)
	{
		if((HAL_GetTick()-temp)>200)  SW3_flag =1;
		while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3)==GPIO_PIN_RESET);
		temp = HAL_GetTick();

	}

	if(GPIO_Pin == GPIO_PIN_4)
	{
		if((HAL_GetTick()-temp)>200) SW4_flag =1;
		while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4)==GPIO_PIN_RESET);
		temp = HAL_GetTick();
	}

}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
