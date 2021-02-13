/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include <string.h>
#include <stdio.h>

#define TRUE 1
#define FALSE 0

void UART2_Init(void);
void Error_handler(void);

UART_HandleTypeDef huart2;


/*
 * main.c calls HAL_Init(), then it calls HAL_MspInit() (in msp.c)
 * main.c calls UART2_Init()-->HAL_UART_Init(), then it calls HAL_UART_MspInit() (in msp.c)
 * UART interrupts: HAL_UART_Receive_IT --> USART2_IRQHandler (it.c) --> HAL_UART_RxCpltCallback
 */

int main(void){

	HAL_Init();
	UART2_Init();

	RCC_OscInitTypeDef 	osc_init;
	RCC_ClkInitTypeDef	clk_init;
	char msg[100];

	memset(&osc_init, 0, sizeof(osc_init));
	osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	osc_init.HSEState 		= RCC_HSE_BYPASS;
	if( HAL_RCC_OscConfig(&osc_init) != HAL_OK ){
		Error_handler();
	}
	clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK \
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
	clk_init.AHBCLKDivider = RCC_SYSCLK_DIV2;
	clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
	clk_init.APB2CLKDivider = RCC_HCLK_DIV2;

	if(HAL_RCC_ClockConfig(&clk_init, FLASH_ACR_LATENCY_0WS) != HAL_OK){
		Error_handler();
	}

	// save energy
	__HAL_RCC_HSI_DISABLE();

	/* Initialize the Cortex System Timer
	 * HAL_RCC_GetHCLKFreq() = 4Mhz (in this case)*/
	/* SYSTICK = 1 / HAL_RCC_GetHCLKFreq(), in this case 1 / 4Mhz = 0.25 micro-seconds*/
	/* TicksNumb = number of SYSTICK for a defined delay.
	 * In this case, delay = 1ms, that is, TicksNumb = 1ms / 0.25micro-seconds = 4Mhz / 1000*/
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000 );
	/* Can be divided by 1 ou 8 (see Timer map) */
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	UART2_Init();

	// send the system clock to computer/logic analyzer
	memset(msg,0, sizeof(msg));
	sprintf( msg, "SYSCLK: %ld\r\n", HAL_RCC_GetSysClockFreq());
	if(HAL_UART_Transmit(&huart2, (uint8_t*) msg, (uint16_t) strlen(msg), HAL_MAX_DELAY) != HAL_OK){
		Error_handler();
	}

	memset(msg,0, sizeof(msg));
	sprintf( msg, "HCLCLK: %ld\r\n", HAL_RCC_GetHCLKFreq());
	if(HAL_UART_Transmit(&huart2, (uint8_t*) msg, (uint16_t) strlen(msg), HAL_MAX_DELAY) != HAL_OK){
		Error_handler();
	}

	memset(msg,0, sizeof(msg));
	sprintf( msg, "PCLK1: %ld\r\n", HAL_RCC_GetPCLK1Freq());
	if(HAL_UART_Transmit(&huart2, (uint8_t*) msg, (uint16_t) strlen(msg), HAL_MAX_DELAY) != HAL_OK){
		Error_handler();
	}

	memset(msg,0, sizeof(msg));
	sprintf( msg, "PCLK2: %ld\r\n", HAL_RCC_GetPCLK2Freq());
	if(HAL_UART_Transmit(&huart2, (uint8_t*) msg, (uint16_t) strlen(msg), HAL_MAX_DELAY) != HAL_OK){
		Error_handler();
	}

	while(1);

	return 0;

}

void UART2_Init(void){
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;

	if(HAL_UART_Init(&huart2) != HAL_OK){
		// there is a problem
		Error_handler();
	}
}

void Error_handler(void){

	while(1);
}





