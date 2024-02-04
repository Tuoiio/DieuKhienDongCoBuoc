#ifndef _MAIN_H_
#define _MAIN_H_

#include <stm32f10x.h>
#include "Delay_SysTick.h"
#include "Delay_Timer.h"
#include "LCD20X4.h"

#define Button_Mode  	GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)
#define Button_OK			GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2)
#define Button_Up 			GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)
#define Button_Down		GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)

#define Pin_Enable			GPIO_Pin_9		// Dong co chay khi chan Enable = 0
#define Pin_Direct_Motor GPIO_Pin_7			// 1: Chay thuan,    0: Chay nghich
#define Pin_PWM			GPIO_Pin_8

void GPIO_Config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	/* Config port B, pin12, 13, 14, 15 Output */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* Config port B, pin 6,7,8 Output */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* Config port A, pin 8,9,10 Ouput */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* Config port A, pin 1,2,3,4 Input Pull Up */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_8);
}

void Timer_Config(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	
	TIM_TimeBaseInitStruct.TIM_Prescaler = 72 - 1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 10 - 1;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
	
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 14;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void PWM_Config(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72;
	TIM_TimeBaseInitStructure.TIM_Period = 0xFFFF - 1;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM1, ENABLE);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	TIM_Cmd(TIM1, ENABLE);
}

#endif
