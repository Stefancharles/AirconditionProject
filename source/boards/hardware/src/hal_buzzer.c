/******************************************************************************
* @ File name --> led.c
* @ Author    --> By@ MALIWEI
* @ Version   --> V1.0
* @ Date      --> 
* @ Brief     --> 蜂鸣器控制相关函数
*
* @ Copyright (C) 20**
* @ All rights reserved
******************************************************************************/

#include "hal_buzzer.h"
#include "delay.h"
#include "stm32f10x_iwdg.h"

//全局变量
BuzzerState_TypeDef Buzzer_State;

/******************************************************************************
* Function Name --> Buzzer接口初始化
* Description   --> none
* Input         --> none
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void Buzzer_Init(void)
{
	STM32_GPIOx_Init(Buzzer0_Init);
	BUZZER0 = 0;
}

/***************************************************************************************************
 * @fn      HalBuzzerBlink
 *
 * @brief   Blink the leds
 *
 * @param   leds       - bit mask value of leds to be blinked
 *          numBlinks  - number of blinks
 *          percent    - the percentage in each period where the led
 *                       will be on
 *          period     - length of each cycle in milliseconds
 *
 * @return  None
 ***************************************************************************************************/
#define BUZZER_MULTIPLE 10
void HalBuzzerBlink (uint8_t leds, uint8_t numBlinks, uint8_t percent, uint16_t period)
{
	uint8_t led;
	uint8_t pos;
	uint16_t i=0;
	
	period=period*10;
	HalBuzzerOnOff (leds, HAL_BUZZER_MODE_ON);
	if (leds && percent && period)
	{
		if (percent < 100)
		{
			led = HAL_BUZZER_0;
			leds &= HAL_BUZZER_ALL;
			pos=0;
			while (leds)
			{
				if (leds & led)
				{
					Buzzer_State.numBlinksGlobal=numBlinks;//闪烁次数;
					Buzzer_State.percentGlobal=(period*percent)/100;//一个周期中亮的时长
					Buzzer_State.periodGlobal=period;//一个周期时长
					Buzzer_State.BuzzerTimeCount=0;
					leds ^= led;
				}
				pos++;
				led <<= 1;
			}
		}
		else
		{
			HalBuzzerOnOff (leds, HAL_BUZZER_MODE_ON);                    /* >= 100%, turn on */
		}
	}
	else
	{
		HalBuzzerOnOff (leds, HAL_BUZZER_MODE_OFF);                     /* No on time, turn off */
	}
	for(i=0; i<period; i++)
	{
		HalBuzzerBlinkProcess();
		delay_us(100);
#ifdef USE_IWDG	
		IWDG_ReloadCounter();//重载看门狗定时器
#endif /*USE_IWDG*/
	}
}

/***************************************************************************************************
 * @fn      HalBuzzerOnOff
 *
 * @brief   Turns specified Buzzer ON or OFF
 *
 * @param   leds - Buzzer bit mask
 *          mode - BUZZER_ON,BUZZER_OFF,
 *
 * @return  none
 ***************************************************************************************************/
void HalBuzzerOnOff (uint8_t leds, uint8_t mode)
{
	if (leds & HAL_BUZZER_0)
	{
		if (mode == HAL_BUZZER_MODE_ON)
		{
			HAL_TURN_ON_BUZZER0;
		}
		else
		{
			HAL_TURN_OFF_BUZZER0;
		}
	}
}

void HalBuzzerBlinkProcess ( void )
{
	if(Buzzer_State.numBlinksGlobal>0)
	{
		if(Buzzer_State.BuzzerTimeCount<Buzzer_State.periodGlobal)
		{
			if(Buzzer_State.BuzzerTimeCount++<Buzzer_State.percentGlobal)
			{
				HAL_TOGGLE_BUZZER0;
			}
			else
			{
				HAL_TURN_OFF_BUZZER0;
			}
		}
		else
		{
			Buzzer_State.BuzzerTimeCount=0;
			Buzzer_State.numBlinksGlobal--;
		}
	}
}

