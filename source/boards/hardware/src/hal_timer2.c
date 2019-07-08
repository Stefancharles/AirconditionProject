/*
******************************************************************************
* @file    TIM/TimeBase/main.c 
* @author  产品中心.硬件部
* @version V1.0.0
* @date    2017.09.07
* @brief   通用定时器2
******************************************************************************
*/

//Includes
#include "stm32f103_config.h"
#include "hal_timer2.h"
#include "hal_buzzer.h"
#include "ledNixietube.h"


TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

NVIC_InitTypeDef NVIC_InitStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
uint16_t capture = 0;
const uint16_t CCR1_Val = /*24000*/18000/*6000*/;//3ms,333Hz

uint16_t T_X_MS = 0;
uint16_t T_Y_MS = 0;
uint16_t T_Z_MS = 0;
uint16_t T_NET_MS = 0;
uint16_t T_ALARM_MS = 0;

/*******************************************************************
*函数：void TIM2_Init(void)
*功能：定时器2初始化
*输入：无
*输出：无
*特殊说明：无
*******************************************************************/
void TIM2_Init(void)
{
	uint16_t PrescalerValue = 0;
	   
	/* --------------------------- System Clocks Configuration ---------------------*/
	/* TIM2 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	/* --------------------------NVIC Configuration -------------------------------*/
	/* Enable the TIM2 gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;//1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;//1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);
  
  
	/* ------------------- TIM2 Configuration:Output Compare Timing Mode ---------*/

	/* 
	SystemCoreClock = SYSCLK_FREQ_72MHz;
	PrescalerValue = (uint16_t) (SystemCoreClock / 6000000) - 1=11;
	TIM2 counter clock at SystemCoreClock/(PrescalerValue+1)=72/(11+1) MHz = 6 MHz
	CC1 update rate = TIM2 counter clock / CCR1_Val = 1KHz
	*/

	/* Compute the prescaler value */
	PrescalerValue = (uint16_t) (SystemCoreClock / 6000000) - 1;

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 65535;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	/* Prescaler configuration */
	TIM_PrescalerConfig(TIM2, PrescalerValue, TIM_PSCReloadMode_Immediate);

	/* Output Compare Timing Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC1Init(TIM2, &TIM_OCInitStructure);

	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);

	/* TIM IT enable */
	TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);

	/* TIM2 enable counter */
	TIM_Cmd(TIM2, ENABLE);

}

/**
  * @brief  This function handles TIM2 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);

		//3ms定时进程
		
		/***********************串口1接收超时进程处理************************/
#if EN_USART1_RX	//如果使能了接收中断
		if(F_USART1_RX_RECEIVING)//正在接收串口数据
		{
			USART1_RX_TIMEOUT_COUNT++;//串口超时计数
			if(USART1_RX_TIMEOUT_COUNT>3)//数据接收间隔超过3ms
			{//串口接收完成或结束
				F_USART1_RX_RECEIVING=0;
				USART1_RX_TIMEOUT_COUNT=0;
				F_USART1_RX_FINISH=1;
			}
		}
#endif
		if(F_UART4_RX_RECEIVING)//正在接收串口数据
		{
			UART4_RX_TIMEOUT_COUNT++;//串口超时计数
			if(UART4_RX_TIMEOUT_COUNT>3)//数据接收间隔超过3ms
			{//串口接收完成或结束
				F_UART4_RX_RECEIVING=0;
				UART4_RX_TIMEOUT_COUNT=0;
				F_UART4_RX_FINISH=1;
			}
		}
		
		/***********************扫描数码管进程处理************************/
		led_scan();
		
		/***********************计时时基寄存器************************/
		T_X_MS++;//LCD显示用
		T_Y_MS++;//按键扫描用
		T_Z_MS++;//主程序运行间隔
		T_NET_MS++;//网络通讯间隔
		T_ALARM_MS++;//告警计时
		if(T_X_MS>32768)
		{
			T_X_MS=0;
		}
		if(T_Y_MS>32768)
		{
			T_Y_MS=0;
		}
		if(T_Z_MS>32768)
		{
			T_Z_MS=0;
		}
		if(T_NET_MS>32768)
		{
			T_NET_MS=0;
		}
		T_ALARM_MS=(uint16_t)T_ALARM_MS;
		
		capture = (uint16_t)TIM_GetCapture1(TIM2);
		TIM_SetCompare1(TIM2, (uint16_t)(capture + CCR1_Val));
	}
}


/*****************END OF FILE****************/
