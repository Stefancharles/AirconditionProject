/******************************************************************************
* @ File name --> led.h
* @ Author    --> By@ maliwei
* @ Version   --> V1.0
* @ Date      --> 
* @ Brief     --> ������������غ���
*
* @ Copyright (C) 20**
* @ All rights reserved
******************************************************************************/

#ifndef _hal_buzzer_h_
#define _hal_buzzer_h_

/*===========================================================================*/
#ifdef __cplusplus  /* C++֧�� */
	extern "C"{
#endif
/*===========================================================================*/

/******************************************************************************
                             �ⲿ����ͷ�ļ�
                 Ӧ�õ���ͬ������ͷ�ļ����������޸ļ���                        
******************************************************************************/

#include "sys.h"

/******************************************************************************
                                   ����ӿ�
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

//����
typedef struct
{//��������ʱ�䵥λ��MS
	uint8_t numBlinksGlobal;//��˸����
	uint16_t percentGlobal;//һ������������ʱ��
	uint16_t periodGlobal;//һ������ʱ��
	uint16_t BuzzerTimeCount;//ÿ�������У��Ѿ��߹���ʱ���ʱ
} BuzzerState_TypeDef;

//�ⲿ����

/******************************************************************************
                                �ⲿ���ܺ���                      
******************************************************************************/

void Buzzer_Init(void);	//��ʼ��Buzzer�ӿ�
void HalBuzzerBlink (uint8_t leds, uint8_t numBlinks, uint8_t percent, uint16_t period);
void HalBuzzerOnOff (uint8_t leds, uint8_t mode);
void HalBuzzerBlinkProcess ( void );

/*===========================================================================*/
#ifdef __cplusplus  /* C++֧�� */
	}
#endif
/*===========================================================================*/


#endif





