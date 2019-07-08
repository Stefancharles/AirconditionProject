/******************************************************************************
* @ File name --> usart.c
* @ Author    --> By@ Sam Chan
* @ Version   --> V1.0
* @ Date      --> 10 - 20 - 2012
* @ Brief     --> 系统串口设置相关的函数
*
* @ Copyright (C) 20**
* @ All rights reserved
******************************************************************************/

#include <stdio.h>
#include "hal_usart1.h"


/******************************************************************************
           加入以下代码，支持printf函数，而不需要选择use MicroLIB
******************************************************************************/
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 
// FILE is typedef’ d in stdio.h.  
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR & 0x40) == 0);	//循环发送，直到发送完毕   
	USART1->DR = (uint8_t)ch;	//发送数据  
	return ch;
}

/*************************************   end   *******************************/


/******************************************************************************
* Function Name --> 初始化IO 串口1
* Description   --> none
* Input         --> bound：波特率	
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void USART1_Init(uint32_t bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
#if EN_USART1_RX	//如果使能了接收中断
	
	USART1->CR1 |= 1 << 8;	//PE中断使能
	USART1->CR1 |= 1 << 5;	//接收缓冲区非空中断使能
	
	MY_NVIC_Init(0, 0, USART1_IRQn, NVIC_PriorityGroup_2);	//中断分组2，最低优先级
	
#endif
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
		
	USART_DeInit(USART1);  //复位串口1
	
	//USART1_TX   PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9
   
	//USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10
  
	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;	//设置波特率，一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;	//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	
	USART_Init(USART1, &USART_InitStructure); //初始化串口
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
	USART_Cmd(USART1, ENABLE);                    //使能串口 
}

/******************************************************************************
* Function Name --> 串口1发送一个字节数据
* Description   --> nonevoid USART1_SendData(uint16_t Data)
* Input         --> bound：波特率	
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void USART1_SendData(uint16_t Data)
{
	while((USART1->SR & 0x40) == 0);	//等待发送完毕
	USART_SendData(USART1, Data);
}

/******************************************************************************
* Function Name --> 串口1发送一串数据
* Description   --> none
* Input         --> *Data:字符串	
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
 
#if EN_USART1_RX   //如果使能了接收

//=========================================================

//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误 

uint8_t USART1_RX_BUF[USART1_REC_LEN];	//接收缓冲,最大USART1_REC_LEN个字节



uint16_t USART1_RX_COUNT=0;	//接收到的有效字节数目，最大USART1_REC_LEN字节
uint8_t F_USART1_RX_FINISH=0;//接收状态标记,1:usart1接收完成，0：usart1接收未完成
uint8_t F_USART1_RX_RECEIVING=0;//串口接收正在进行中标记,1:进行中，0：暂停或结束
uint8_t USART1_RX_TIMEOUT_COUNT=0;	//串口接收超时计数。串口超过3ms未接收到数据，视为超时。单位：ms

/******************************************************************************
* Function Name --> 串口1接收中断服务程序
* Description   --> none
* Input         --> none
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收到数据
	{
		F_USART1_RX_RECEIVING=1;//正在接收串口数据
		USART1_RX_TIMEOUT_COUNT=0;//超时计数清零
		USART1_RX_BUF[USART1_RX_COUNT] = USART_ReceiveData(USART1);	//读取接收到的数据
		USART1_RX_COUNT++;
		if(USART1_RX_COUNT>USART1_REC_LEN)//缓存区溢出
		{USART1_RX_COUNT = 0x0000;}	
		//end 接收未完成   		 
	}	//end 接收到数据
}

//=========================================================

#endif	//end使能接收

//=========================================================





