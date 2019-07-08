#include "stm32f103_config.h"
#include "hal_key.h"

/******************************************************
�������ƣ�void keyInit(void)
���ܣ���������IO�ڳ�ʼ��
���룺��
�������
******************************************************/
void keyInit(void)
{
	STM32_GPIOx_Init(K_PA1_IN_Init);
	STM32_GPIOx_Init(K_PA3_IN_Init);
	STM32_GPIOx_Init(K_PA4_IN_Init);	
}

/******************************************************
�������ƣ�keyType keyScanInd(void)
���ܣ���������ɨ��
���룺��
�����������ֵ
******************************************************/
keyType keyScanInd(void)
{
	static unsigned char keyPressCount=0;
	static keyType keyValueOlde=noneKey;
	keyType keyValue=noneKey;
	//���K_PA1�е����а���
	if(K_PA1!=1)//K_PA1���а�������
	{
		keyValue=s106;
	}
	//���K_PA3�е����а���
	if(K_PA3!=1)//K_PA4���а�������
	{
		keyValue=s116;
	}
	//���K_PA4�е����а���
	if(K_PA4!=1)//K_PA4���а�������
	{
		keyValue=s121;
	}
	
	if(keyValue==noneKey)//�ް�������
	{
		keyValueOlde=keyValue;
		keyPressCount=0;
		return noneKey;
	}
	//�����ǰ���ȥ��������
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
�������ƣ�keyType KEY_ScanTriggerOnce(void)
���ܣ���������ɨ�裬������Ч��������һ��
���룺��
�����������ֵ
******************************************************/
keyType KEY_ScanTriggerOnce(void)
{
	static keyType keyValueTemp=noneKey;//������ֵ�Ĵ���
	static keyType keyValueTempOlde=noneKey;//��һ�ΰ�����ֵ�Ĵ���
	
	keyValueTemp=keyScanInd();//����ɨ��
	
	if(keyValueTempOlde != keyValueTemp)
	{//�������л���������Ч����ֻ����һ��
		keyValueTempOlde=keyValueTemp;
	}
	else
	{//����һֱ���£���û���£�������Ч
		keyValueTemp=noneKey;
	}
	return keyValueTemp;
}
