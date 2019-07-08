/******************************************************************************
* @ File name --> led.h
* @ Author    --> By@ maliwei
* @ Version   --> V1.0
* @ Date      --> 
* @ Brief     --> 蜂鸣器控制相关函数
*
* @ Copyright (C) 20**
* @ All rights reserved
******************************************************************************/

#ifndef _hal_buzzer_h_
#define _hal_buzzer_h_

/*===========================================================================*/
#ifdef __cplusplus  /* C++支持 */
	extern "C"{
#endif
/*===========================================================================*/

/******************************************************************************
                             外部函数头文件
                 应用到不同的外设头文件请在这里修改即可                        
******************************************************************************/

#include "sys.h"

/******************************************************************************
                                   定义接口
******************************************************************************/

#define Buzzer0_Init				RCC_APB2Periph_GPIOA,GPIOA,GPIO_Pin_8,GPIO_Speed_2MHz,GPIO_Mode_Out_PP
#define BUZZER0						PAout(8)

/* BUZZERS - The BUZZER number is the same as the bit position */
#define HAL_BUZZER_0				0x01
#define HAL_BUZZER_ALL				(HAL_BUZZER_0)

/* Modes */
#define HAL_BUZZER_MODE_OFF			0x00
#define HAL_BUZZER_MODE_ON			0x01

/* Defaults */
#define HAL_BUZZER_DEFAULT_MAX_BUZZERS		1
#define HAL_BUZZER_DEFAULT_DUTY_CYCLE		5
#define HAL_BUZZER_DEFAULT_FLASH_COUNT		50
#define HAL_BUZZER_DEFAULT_FLASH_TIME		1000

//ON/OFF
#define HAL_TURN_ON_BUZZER0 	(BUZZER0=1)
#define HAL_TURN_OFF_BUZZER0	(BUZZER0=0)
#define HAL_TOGGLE_BUZZER0		(BUZZER0=!BUZZER0)

//类型
typedef struct
{//以下所有时间单位：MS
	uint8_t numBlinksGlobal;//闪烁次数
	uint16_t percentGlobal;//一个周期中亮的时长
	uint16_t periodGlobal;//一个周期时长
	uint16_t BuzzerTimeCount;//每个周期中，已经走过的时间计时
} BuzzerState_TypeDef;

//外部变量

/******************************************************************************
                                外部功能函数                      
******************************************************************************/

void Buzzer_Init(void);	//初始化Buzzer接口
void HalBuzzerBlink (uint8_t leds, uint8_t numBlinks, uint8_t percent, uint16_t period);
void HalBuzzerOnOff (uint8_t leds, uint8_t mode);
void HalBuzzerBlinkProcess ( void );

/*===========================================================================*/
#ifdef __cplusplus  /* C++支持 */
	}
#endif
/*===========================================================================*/


#endif





