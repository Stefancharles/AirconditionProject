
#include "stm32f103_config.h"
#include "stm32App.h"
#include "main.h"



int main(void)
{	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置中断分组
	delay_init();
	//delay_ms(100);    //延时100Ms待系统稳定
	USART1_Init(115200);  //初始化IO 串口1
	UART4_Init(115200);   //初始化IO 串口4
	Buzzer_Init();//蜂鸣器I/O口是PA8
	TIM2_Init();
	
	HalBuzzerBlink (HAL_BUZZER_0, 1, 50, 500);  //Blink the leds
	
	simpleAppInit();

	simpleAppProcess();
	
	return 0;
}

/*******************************************************************
*函数：void IWDG_Init(void)
*功能：独立看门狗初始化，定时为250ms
*输入：无
*输出：无
*特殊说明：无
*******************************************************************/
#ifdef USE_IWDG
void IWDG_Init(void)
{	
	uint16_t LsiFreq = 40000;//LSI时钟频率大约40kHz(在30kHz和60kHz之间)。

	/* Check if the system has resumed from IWDG reset */
	if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
	{
		/* IWDGRST flag set */
		printf("IWDGRST flag set\r\n");
		/* Clear reset flags */
		RCC_ClearFlag();
	}
	else
	{
		/* IWDGRST flag is not set */
		printf("IWDGRST flag is not set\r\n");
	}

	/* IWDG timeout equal to 250 ms (the timeout may varies due to LSI frequency
	 dispersion) */
	/* Enable write access to IWDG_PR and IWDG_RLR registers */
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

	/* IWDG counter clock: LSI/32 */
	IWDG_SetPrescaler(IWDG_Prescaler_32);

	/* Set counter reload value to obtain 250ms IWDG TimeOut.
	 Counter Reload Value = 250ms/IWDG counter clock period
						  = 250ms / (LSI/32)
						  = 0.25s / (LsiFreq/32)
						  = LsiFreq/(32 * 4)
						  = LsiFreq/128
	*/
	IWDG_SetReload(LsiFreq/128);//LSI时钟频率大约40kHz(在30kHz和60kHz之间)。

	/* Reload IWDG counter */
	IWDG_ReloadCounter();

	/* Enable IWDG (the LSI oscillator will be enabled by hardware) */
	IWDG_Enable();
}
#endif /*USE_IWDG*/
