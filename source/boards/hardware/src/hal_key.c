#include "stm32f103_config.h"
#include "hal_key.h"

/******************************************************
函数名称：void keyInit(void)
功能：独立按键IO口初始化
输入：无
输出：无
******************************************************/
void keyInit(void)
{
	STM32_GPIOx_Init(K_PA1_IN_Init);
	STM32_GPIOx_Init(K_PA3_IN_Init);
	STM32_GPIOx_Init(K_PA4_IN_Init);	
}

/******************************************************
函数名称：keyType keyScanInd(void)
功能：独立按键扫描
输入：无
输出：按键键值
******************************************************/
keyType keyScanInd(void)
{
	static unsigned char keyPressCount=0;
	static keyType keyValueOlde=noneKey;
	keyType keyValue=noneKey;
	//检测K_PA1行的所有按键
	if(K_PA1!=1)//K_PA1行有按键按下
	{
		keyValue=s106;
	}
	//检测K_PA3行的所有按键
	if(K_PA3!=1)//K_PA4行有按键按下
	{
		keyValue=s116;
	}
	//检测K_PA4行的所有按键
	if(K_PA4!=1)//K_PA4行有按键按下
	{
		keyValue=s121;
	}
	
	if(keyValue==noneKey)//无按键按下
	{
		keyValueOlde=keyValue;
		keyPressCount=0;
		return noneKey;
	}
	//以下是按键去抖动处理
	if(keyValueOlde!=keyValue)
	{
		keyValueOlde=keyValue;
		keyPressCount=0;
		return noneKey;
	}
	else
	{
		if(keyPressCount>5)
		{
			return keyValue;
		}
		keyPressCount++;
		return noneKey;
	}
}

/******************************************************
函数名称：keyType KEY_ScanTriggerOnce(void)
功能：独立按键扫描，按下有效，仅触发一次
输入：无
输出：按键键值
******************************************************/
keyType KEY_ScanTriggerOnce(void)
{
	static keyType keyValueTemp=noneKey;//按键键值寄存器
	static keyType keyValueTempOlde=noneKey;//上一次按键键值寄存器
	
	keyValueTemp=keyScanInd();//按键扫描
	
	if(keyValueTempOlde != keyValueTemp)
	{//按键有切换，按键有效，且只触发一次
		keyValueTempOlde=keyValueTemp;
	}
	else
	{//按键一直按下，或没按下，按键无效
		keyValueTemp=noneKey;
	}
	return keyValueTemp;
}
