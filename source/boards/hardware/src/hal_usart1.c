/******************************************************************************
* @ File name --> usart.c
* @ Author    --> By@ Sam Chan
* @ Version   --> V1.0
* @ Date      --> 10 - 20 - 2012
* @ Brief     --> ϵͳ����������صĺ���
*
* @ Copyright (C) 20**
* @ All rights reserved
******************************************************************************/

#include <stdio.h>
#include "hal_usart1.h"


/******************************************************************************
           �������´��룬֧��printf������������Ҫѡ��use MicroLIB
******************************************************************************/
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 
// FILE is typedef�� d in stdio.h.  
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR & 0x40) == 0);	//ѭ�����ͣ�ֱ���������   
	USART1->DR = (uint8_t)ch;	//��������  
	return ch;
}

/*************************************   end   *******************************/


/******************************************************************************
* Function Name --> ��ʼ��IO ����1
* Description   --> none
* Input         --> bound��������	
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void USART1_Init(uint32_t bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
#if EN_USART1_RX	//���ʹ���˽����ж�
	
	USART1->CR1 |= 1 << 8;	//PE�ж�ʹ��
	USART1->CR1 |= 1 << 5;	//���ջ������ǿ��ж�ʹ��
	
	MY_NVIC_Init(0, 0, USART1_IRQn, NVIC_PriorityGroup_2);	//�жϷ���2��������ȼ�
	
#endif
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
		
	USART_DeInit(USART1);  //��λ����1
	
	//USART1_TX   PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9
   
	//USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10
  
	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;	//���ò����ʣ�һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;	//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	
	USART_Init(USART1, &USART_InitStructure); //��ʼ������
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
	USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 
}

/******************************************************************************
* Function Name --> ����1����һ���ֽ�����
* Description   --> nonevoid USART1_SendData(uint16_t Data)
* Input         --> bound��������	
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void USART1_SendData(uint16_t Data)
{
	while((USART1->SR & 0x40) == 0);	//�ȴ��������
	USART_SendData(USART1, Data);
}

/******************************************************************************
* Function Name --> ����1����һ������
* Description   --> none
* Input         --> *Data:�ַ���	
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void USART1_SendStr(uint8_t *Data)
{
	while(*Data!='\0')
	{
		USART1_SendData((uint16_t )(*Data));
		Data++;
	}
}

//=========================================================
 
#if EN_USART1_RX   //���ʹ���˽���

//=========================================================

//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ��� 

uint8_t USART1_RX_BUF[USART1_REC_LEN];	//���ջ���,���USART1_REC_LEN���ֽ�



uint16_t USART1_RX_COUNT=0;	//���յ�����Ч�ֽ���Ŀ�����USART1_REC_LEN�ֽ�
uint8_t F_USART1_RX_FINISH=0;//����״̬���,1:usart1������ɣ�0��usart1����δ���
uint8_t F_USART1_RX_RECEIVING=0;//���ڽ������ڽ����б��,1:�����У�0����ͣ�����
uint8_t USART1_RX_TIMEOUT_COUNT=0;	//���ڽ��ճ�ʱ���������ڳ���3msδ���յ����ݣ���Ϊ��ʱ����λ��ms

/******************************************************************************
* Function Name --> ����1�����жϷ������
* Description   --> none
* Input         --> none
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //���յ�����
	{
		F_USART1_RX_RECEIVING=1;//���ڽ��մ�������
		USART1_RX_TIMEOUT_COUNT=0;//��ʱ��������
		USART1_RX_BUF[USART1_RX_COUNT] = USART_ReceiveData(USART1);	//��ȡ���յ�������
		USART1_RX_COUNT++;
		if(USART1_RX_COUNT>USART1_REC_LEN)//���������
		{USART1_RX_COUNT = 0x0000;}	
		//end ����δ���   		 
	}	//end ���յ�����
}

//=========================================================

#endif	//endʹ�ܽ���

//=========================================================





