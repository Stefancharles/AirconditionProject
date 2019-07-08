/******************************************************************************
* @ File name --> sys.c
* @ Author    --> By@ Sam Chan
* @ Version   --> V1.0
* @ Date      --> 10 - 20 - 2012
* @ Brief     --> ϵͳ������صĺ���
*
* @ Copyright (C) 20**
* @ All rights reserved
*******************************************************************************
*
*                                  File Update
* @ Version   --> V1.0.1
* @ Author    --> By@ Sam Chan
* @ Date      --> 03 - 09 - 2014
* @ Revise    --> ����GPIO���ó�ʼ����������������ֲʱֱ�Ӻ궨�弴��
*
* @ Version   --> V1.0.2
* @ Author    --> By@ Sam Chan
* @ Date      --> 05 - 10 - 2014
* @ Revise    --> ���Ӷ�C++����֧��
*
******************************************************************************/

#include "sys.h"

/******************************************************************************
* Function Name --> STM32 GPIO��������ʼ��
* Description   --> ���մ˺����βε�˳����к궨�弴��
* Input         --> none
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void STM32_GPIOx_Init(
                        uint32_t RCC_APB2Periph,		/* GPIO��Ӧʱ�� */
						GPIO_TypeDef* GPIOx,			/* ����Ӧ��GPIO�� */
						uint16_t GPIO_Pinx,				/* GPIO��Ӧ�Ĺܽź� */                       
						GPIOSpeed_TypeDef GPIO_SPEED,	/* GPIO��ת�ٶ� */
						GPIOMode_TypeDef GPIO_MODE		/* GPIO����ģʽ */
					   )
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//����������
	assert_param(IS_RCC_APB2_PERIPH(RCC_APB2Periph));
	assert_param(IS_GPIO_PIN(GPIO_Pinx));
	assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
	assert_param(IS_GPIO_SPEED(GPIO_SPEED));
	assert_param(IS_GPIO_MODE(GPIO_MODE)); 

	RCC_APB2PeriphClockCmd(RCC_APB2Periph, ENABLE);	//��������ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pinx;
	GPIO_InitStructure.GPIO_Speed = GPIO_SPEED;
	GPIO_InitStructure.GPIO_Mode = GPIO_MODE;

	GPIO_Init(GPIOx, &GPIO_InitStructure);	//��ʼ��GPIO��ؽṹ��
}
/******************************************************************************
* Function Name --> ��ȡоƬ��������
* Description   --> none
* Input         --> none
* Output        --> *STMCapa�������ַ���ʾ����
* Reaturn       --> none 
******************************************************************************/
void STM32_Flash_Capacity(uint8_t *STMCapa)
{
	uint16_t capa;
	
	capa = *((uint16_t*)0x1ffff7e0);	//��ȡ���������Ĵ�������16λ��Ч
	
	capa = ((capa >> 12) * 4096 + ((capa >> 8) & 0x0f) * 256 + ((capa >> 4) & 0x0f) * 16 + (capa & 0x0f));	//ת����ʮ����
	
	STMCapa[0] = 0x4d;	//M
	STMCapa[1] = 0x43;	//C
	STMCapa[2] = 0x55;	//U
	STMCapa[3] = 0x43;	//C
	STMCapa[4] = 0x61;	//a
	STMCapa[5] = 0x70;	//p
	STMCapa[6] = 0x3a;	//:
	
	if((capa / 1000) != 0)	STMCapa[7] = capa / 1000 + 48;	//ǧλ��Ϊ0ʱ��ʾ
	else	STMCapa[7] = 0x20;
	
	STMCapa[8] = capa % 1000 / 100 + 48;	//��λ
	STMCapa[9] = capa % 100 /10 + 48;		//ʮλ
	STMCapa[10] = capa % 10 + 48;			//��λ
	STMCapa[11] = 0x4b;	//K
	STMCapa[12] = 0x62;	//b
}
/******************************************************************************
* Function Name --> ��ȡCPUID
* Description   --> none
* Input         --> none
* Output        --> �洢CPUID���棨�ַ��������
* Reaturn       --> none 
******************************************************************************/
void STM32_CPUID(uint8_t *IDbuff)
{
	uint32_t CPUID;
	CPUID = *((uint32_t*)0xe000ed00);
	sprintf((char*)IDbuff, "CPU ID:%08X", CPUID);
}
/******************************************************************************
* Function Name --> ����������ƫ�Ƶ�ַ
* Description   --> none
* Input         --> NVIC_VectTab����ַ
*                      SRAM_BASE���ڲ�SRAM��ַ(��NVIC_VectTab_RAM)
*                     FLASH_BASE���ڲ�FLASH��ַ(��NVIC_VectTab_FLASH)
*                   Offset��ƫ����	
* Output        --> none
* Reaturn       --> none 
******************************************************************************/ 
void MY_NVIC_SetVectorTable(uint32_t NVIC_VectTab,uint32_t Offset)	 
{
	//����������
	assert_param(IS_NVIC_VECTTAB(NVIC_VectTab));
	assert_param(IS_NVIC_OFFSET(Offset));
	 	   	 
	SCB->VTOR = NVIC_VectTab | (Offset & (uint32_t)0x1fffff80);//����NVIC��������ƫ�ƼĴ���
	//���ڱ�ʶ����������CODE��������RAM��
}
/******************************************************************************
* Function Name --> �����жϷ���
* Description   --> 0 ~ 4�飬������5��
* Input         --> NVIC_PriorityGroup: �жϷ���
* Output        --> none
* Reaturn       --> none 
******************************************************************************/ 
void MY_NVIC_PriorityGroup_Config(uint32_t NVIC_PriorityGroup)
{
	//����������
	assert_param(IS_NVIC_PRIORITY_GROUP(NVIC_PriorityGroup));

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup);	//�����жϷ���
	
//	uint32_t temp,temp1;
//	
//	temp1 = (~NVIC_PriorityGroup) & 0x00000007;//ȡ����λ
//	temp1 <<= 8;
//	temp = SCB->AIRCR;  //��ȡ��ǰ������
//	temp &= 0x0000f8ff; //�����ǰ����
//	temp |= 0x05fa0000; //д��Կ��
//	temp |= temp1;	   
//	SCB->AIRCR = temp;  //���÷���
}
/******************************************************************************
* Function Name --> �����жϷ������ȼ�
* Description   --> 1���黮��:
*                      ��0��0λ��ռ���ȼ���4λ��Ӧ���ȼ�
*                      ��1��1λ��ռ���ȼ���3λ��Ӧ���ȼ�
*                      ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
*                      ��3��3λ��ռ���ȼ���1λ��Ӧ���ȼ�
*                      ��4��4λ��ռ���ȼ���0λ��Ӧ���ȼ�
*                   2���������ȼ���������Ӧ���ȼ�����ԭ��������ֵԽС���ȼ�Խ��
* Input         --> NVIC_PreemptionPriority���������ȼ�
*                   NVIC_Subpriority����Ӧ���ȼ�
*                   NVIC_Channel���жϱ��
*                   NVIC_Group: �жϷ���
* Output        --> none
* Reaturn       --> none 
******************************************************************************/ 
void MY_NVIC_Init(uint8_t NVIC_PreemptionPriority,uint8_t NVIC_Subpriority,uint8_t NVIC_Channel,uint32_t NVIC_Group)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	//����������
	assert_param(IS_NVIC_PREEMPTION_PRIORITY(NVIC_PreemptionPriority));  
	assert_param(IS_NVIC_SUB_PRIORITY(NVIC_Subpriority));
	
	MY_NVIC_PriorityGroup_Config(NVIC_Group);	//�����жϷ���	
	
	NVIC_InitStructure.NVIC_IRQChannel = NVIC_Channel;								//�����ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_PreemptionPriority;	//�������ȼ�  	 	 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_Subpriority;				//��Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;									//ʹ���ж�

	NVIC_Init(&NVIC_InitStructure);	//��ʼ���ж�
	
//	uint32_t temp;	
//	uint8_t IPRADDR=NVIC_Channel/4;  //ÿ��ֻ�ܴ�4��,�õ����ַ 
//	uint8_t IPROFFSET=NVIC_Channel%4;//�����ڵ�ƫ��
//	
//	IPROFFSET = IPROFFSET*8 + 4;    //�õ�ƫ�Ƶ�ȷ��λ��
//	
//	MY_NVIC_PriorityGroup_Config(NVIC_Group);//���÷���
//	
//	temp  = NVIC_PreemptionPriority << (4 - NVIC_Group);	//�������ȼ�	  
//	temp |= NVIC_Subpriority & (0x0f >> NVIC_Group);	//��Ӧ���ȼ�
//	temp &= 0xf;//ȡ����λ

//	if(NVIC_Channel < 32)	NVIC->ISER[0] |= 1 << NVIC_Channel;//ʹ���ж�λ(Ҫ����Ļ�,�෴������OK)
//	else	NVIC->ISER[1]| |= 1 << (NVIC_Channel - 32); 
//	
//	NVIC->IPR[IPRADDR] |= temp << IPROFFSET;//������Ӧ���ȼ����������ȼ�  
}
/******************************************************************************
* Function Name --> THUMBָ�֧�ֻ������
* Description   --> �������·���ʵ��ִ�л��ָ��WFI
* Input         --> none
* Output        --> none
* Reaturn       --> none 
******************************************************************************/ 
__asm void WFI_SET(void)
{
	WFI;    
}
/******************************************************************************
* Function Name --> ����ʱ�ӼĴ�����λ
* Description   --> ����������ִ���������踴λ!�����������𴮿ڲ�����
* Input         --> none
* Output        --> none
* Reaturn       --> none 
******************************************************************************/ 
void MY_RCC_DeInit(void)
{										  					   
	RCC->APB1RSTR = 0x00000000;	//��λ����			 
	RCC->APB2RSTR = 0x00000000; 
	  
  	RCC->AHBENR   = 0x00000014;	//˯��ģʽ�����SRAMʱ��ʹ��.�����ر�.	  
  	RCC->APB2ENR  = 0x00000000;	//����ʱ�ӹر�.			   
  	RCC->APB1ENR  = 0x00000000;   
	RCC->CR      |= 0x00000001;	//ʹ���ڲ�����ʱ��HSION	 															 
	RCC->CFGR    &= 0xf8ff0000;	//��λSW[1:0],HPRE[3:0],PPRE1[2:0],PPRE2[2:0],ADCPRE[1:0],MCO[2:0]					 
	RCC->CR      &= 0xfef6ffff;	//��λHSEON,CSSON,PLLON
	RCC->CR      &= 0xfffbffff;	//��λHSEBYP	   	  
	RCC->CFGR    &= 0xff80ffff;	//��λPLLSRC, PLLXTPRE, PLLMUL[3:0] and USBPRE 
	RCC->CIR      = 0x00000000;	//�ر������ж�
	
	//����������				  
#ifdef  VECT_TAB_RAM
	MY_NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);	//�ڲ�SRAM��ʼ��ַ
#else   
	MY_NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);	//�ڲ�FLASH��ʼ��ַ
#endif
}
/******************************************************************************
* Function Name --> ����оƬ�������ģʽ
* Description   --> ����WKUP���Ż��ѣ�PA.0��
* Input         --> none
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void SYS_Standby(void)
{
	SCB->SCR |= 1 << 2;			//ʹ��sleep deep��SYS->CTRL��
	RCC->APB1ENR |= 1 << 28;	//��Դ�ӿ�ʱ�ӿ���
	PWR->CSR |= 1 << 8;			//����WKUP�������ڻ���
	PWR->CR |= 1 << 2;			//���WAKE-UP��־
	PWR->CR |= 1 << 1;			//PDDS��λ��������˯��
	WFI_SET();					//ִ��WFIָ��
}
/******************************************************************************
* Function Name --> ϵͳ��λ
* Description   --> none
* Input         --> none
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void SYS_SoftReset(void)
{
	SCB->AIRCR = 0x05fa0000 | 0x00000004;  //������ο���Cortex-M3Ȩ��ָ��(����).pdf����285ҳ����
}
/******************************************************************************
* Function Name --> JTAGģʽ����
* Description   --> none
* Input         --> mode��ģʽ��������ϸ�궨���sys.h�ж��壩
*                   000��JTAG-DP + SW-DP����λ״̬��
*                   001��JTAG-DP + SW-DP������JNTRST���ţ��ͷ�JRST���ţ�
*                   010��JTAG-DP�ӿڽ�ֹ��SW-DP�ӿ�����
*                   100��JTAG-DP�ӿں�SW-DP�ӿڶ���ֹ
*                   xxx������ֵ����ֹ
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void STM_JTAG_Set(uint32_t mode)
{
	RCC->APB2ENR |= 1 << 0;		//ʹ�ܸ���ʱ��
	AFIO->MAPR &= 0xf8ffffff;	//���SWJ_CFG[2:0]λ������26:24��
	AFIO->MAPR |= (mode << 24);	//����JTAGģʽ
}
/******************************************************************************
* Function Name --> ϵͳʱ�ӳ�ʼ��
* Description   --> none
* Input         --> pll����Ƶ����ȡֵ��Χ��2 ~ 16
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void STM_Clock_Init(uint8_t pll)
{
	uint8_t tmp = 0;
	
	MY_RCC_DeInit();	//��λ���������������ҽ��ⲿ�жϺ�����ʱ��ȫ���ر�
	
	RCC->CR |= 0x00010000;  //�ⲿ����ʱ��ʹ��HSEON
	
	while(!(RCC->CR >> 17));//�ȴ��ⲿʱ�Ӿ���
	
	RCC->CFGR = 0X00000400; //APB1=DIV2;APB2=DIV1;AHB=DIV1;
	pll -= 2;//����2����λ
	RCC->CFGR |= pll << 18;   //����PLLֵ 2~16
	RCC->CFGR |= 1 << 16;	  //PLLSRC ON 
	FLASH->ACR |= 0x32;	  //FLASH 2����ʱ����

	RCC->CR |= 0x01000000;  //PLLON
	
	while(!(RCC->CR >> 25));//�ȴ�PLL����
	
	RCC->CFGR |= 0x00000002;//PLL��Ϊϵͳʱ�ӣ������ܿ���PLL����Ϊ�趨PLLʱ��PLL���λ�����ڹر�״̬�½���
	
	while(tmp != 0x02)     //�ȴ�PLL��Ϊϵͳʱ�����óɹ�
	{   
		tmp  = RCC->CFGR >> 2;
		tmp &= 0x03;
	}    
}
/******************************************************************************
* Function Name --> BCD��תΪHEX
* Description   --> none
* Input         --> BCD_Data��Ҫת����BCD����
* Output        --> none
* Reaturn       --> HEX��
******************************************************************************/
uint8_t BCD_to_HEX(uint8_t BCD_Data)
{
	return((BCD_Data / 10) << 4 | (BCD_Data % 10));
}
/******************************************************************************
* Function Name --> HEX��תΪBCD
* Description   --> none
* Input         --> HEX_Data��Ҫת����BCD����
* Output        --> none
* Reaturn       --> BCD��
******************************************************************************/
uint8_t HEX_to_BCD(uint8_t HEX_Data)
{
	return((HEX_Data >> 4) * 10 + (HEX_Data & 0x0f));
}
/******************************************************************************
* Function Name --> 10������תΪ16����
* Description   --> none
* Input         --> DX_Data��Ҫת����10��������
* Output        --> none
* Reaturn       --> 16����
******************************************************************************/
uint16_t DX_to_HX(uint16_t DX_Data)
{
	return(((DX_Data/1000)<<12) | ((DX_Data%1000/100)<<8) | ((DX_Data%100/10)<<4) | (DX_Data%10));
}
/******************************************************************************
* Function Name --> 16������תΪ10����
* Description   --> none
* Input         --> HX_Data��Ҫת����16��������
* Output        --> none
* Reaturn       --> 10����
******************************************************************************/
uint16_t HX_to_DX(uint16_t HX_Data)
{
	return((HX_Data>>12)*1000+((HX_Data&0x0f00)>>8)*100+((HX_Data&0x00f0)>>4)*10+(HX_Data&0x000f));
}	

/******************************************************************************
* Function Name --> ��ʼ�������б�
* Description   --> none
* Input         --> *LIST���б�ָ��
* Output        --> none
* Reaturn       --> none
******************************************************************************/
//void Sqlist_Init(Sqlist *LIST)
//{
//	LIST->elem = (uint16_t*)malloc(MaxSize * sizeof(ElemType));
//	//����һ������ΪMaxSize * sizeof(ElemType)��С���ڴ�ռ�
//	if(!LIST->elem)	return;	//û�����������б�ֱ���˳�
//	//����ɹ�
//	LIST->length = 0;	//�б���û����
//	LIST->listsize = MaxSize;	//�����ݱ�ռ���ڴ��СΪMaxSize����sizeof(ElemType)Ϊ��λ��
//}
/******************************************************************************
* Function Name --> ��λ�����б�
* Description   --> none
* Input         --> none
* Output        --> none
* Reaturn       --> none
******************************************************************************/
//void Sqlist_DeInit(void)
//{
//	Sqlist *list;
//
//	list->elem = 0;	//�׵�ַ����
//	list->length = 0;	//��������
//	list->listsize = 0;	//�б��СΪ0
//}
/******************************************************************************
* Function Name --> ��һ����̬�������б����һ��Ԫ��
* Description   --> none
* Input         --> *L���б�ָ��
*                    i���б��е�i��λ�ò���Ԫ��
*                    item���ڵ�i��λ���������Ԫ��
* Output        --> none
* Reaturn       --> none
******************************************************************************/
//void InsertElem(Sqlist *L,uint16_t i,ElemType item)
//{	/* ��˳���б�*L�ĵ�i��λ�ò���Ԫ��item */
//	ElemType *base, *insertPtr, *p;
//
//	if(i < 1 || i > L->length + 1)	return;	//�Ƿ�����
//	if(L->length >= L->listsize)	//�������б����һ��λ�ò���Ԫ��
//	{	//׷���ڴ�ռ�
//		base = (ElemType*)realloc(L->elem,(L->listsize + 10) * sizeof(ElemType));
//		L->elem = base;	//�����ڴ����ַ
//		L->listsize = L->listsize + 100;	//�洢�ռ�����100����Ԫ
//	}
//	insertPtr = &(L->Elem[i - 1]);	//insertPtrΪ����λ��
//	for(p = &(L->elem[L->length - 1]);p >= insertPtr;p--)
//		*(p + 1) = *p;	//��i - 1�Ժ��Ԫ��˳�������һ��Ԫ��λ��
//	*insertPtr = item;	//�ڵ�i��λ���ϲ���Ԫ��item
//	L->length++;	//����1
//}


/*****************************************************************************************
*�������ƣ�uint8_t dataTransfer(uint8_t *Input, uint8_t *Output, uint8_t  targetFormat)
*���룺uint8_t *Input, uint8_t *Output, uint8_t  targetFormat
*				uint8_t *Input, ���������
*				uint8_t *Output, ���������
*				uint8_t  targetFormat, ���Ŀ������Output�ĸ�ʽ��ȡֵΪ �� 
*																		BCD_FORMAT-BCD��ʽ
*																		DECIMAL_FORMAT-ʮ���Ƹ�ʽ
*�����SUCCESS��FAILURE
*���ܣ�����ת��
*****************************************************************************************/
uint8_t dataTransfer(uint8_t *Input, uint8_t *Output, uint8_t  targetFormat, uint8_t len)
{
	uint8_t i;
	switch(targetFormat)
	{
		case(BCD_FORMAT)://ת��ΪBCD��
				for(i=0; i < len; i++)
				{
					*Output=((*Input%10)&0x0F)+(((*Input/10)&0x0F)<<4);
					Input++;
					Output++;
				}
			break;
		case(DECIMAL_FORMAT)://ת��Ϊʮ������
				for(i=0; i < len; i++)
				{
					*Output=(*Input&0x0F)+(((*Input&0xF0)>>4)*10);
					Input++;
					Output++;
				}			
			break;
		default: break;
	}
	return SUCCESS;
}




