/******************************************************************************
*
*
*
*
*
*
*
*
*
******************************************************************************/

#include "hal_uart4.h"
#include "stm32f10x_rcc.h"

/******************************************************************************
* Function Name --> 初始化IO 串口4
* Description   --> none
* Input         --> bound：波特率	
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void UART4_Init(uint32_t bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
#ifdef EN_UART4_RX	//如果使能了接收中断
	
	UART4->CR1 |= 1 << 8;	//PE中断使能
	UART4->CR1 |= 1 << 5;	//接收缓冲区非空中断使能
	
	MY_NVIC_Init(3, 2, UART4_IRQn, NVIC_PriorityGroup_2);	//中断分组2
	
#endif
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);	//使能UART4，GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	USART_DeInit(UART4);  //复位串口4
	
	//UART4_TX   PC.10 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOC, &GPIO_InitStructure); //初始化PC10
   
	//UART4_RX	  PC.11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);  //初始化PC11

	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;	//设置波特率，一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;	//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	
	USART_Init(UART4, &USART_InitStructure); //初始化串口
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//开启中断
	USART_Cmd(UART4, ENABLE);                    //使能串口 
}

/******************************************************************************
* Function Name --> 串口4发送一个字节数据
* Description   --> nonevoid USART1_SendData(uint16_t Data)
* Input         --> bound：波特率	
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void UART4_SendData(uint16_t Data)
{
	while((UART4->SR & 0x40) == 0);	//等待发送完毕
	USART_SendData(UART4, Data);
}

/******************************************************************************
* Function Name --> 串口4发送一串数据
* Description   --> none
* Input         --> *Data:字符串	
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void UART4_SendStr(uint8_t *Data)
{
	while(*Data!='\0')
	{
		UART4_SendData((uint16_t )(*Data));
		Data++;
	}
}

/******************************************************************************
* Function Name --> 串口4发送一串数据，指定数据长度
* Description   --> none
* Input         --> *Data:字符串，len长度	
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void UART4_SendStrlen(uint8_t *Data, uint8_t len)
{
	while(len--)
	{
		UART4_SendData((uint16_t )(*Data));
		Data++;
	}
}

//=========================================================
 
#ifdef EN_UART4_RX   //如果使能了接收

//=========================================================

//串口4中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误 

uint8_t UART4_RX_BUF[UART4_REC_LEN];	//接收缓冲,最大UART4_REC_LEN个字节

//接收状态
uint16_t UART4_RX_COUNT=0;	//接收到的有效字节数目，最大USART1_REC_LEN字节
uint8_t F_UART4_RX_FINISH=0;//接收状态标记,1:UART4接收完成，0：UART4接收未完成
uint8_t F_UART4_RX_RECEIVING=0;//串口接收正在进行中标记,1:进行中，0：暂停或结束
uint8_t UART4_RX_TIMEOUT_COUNT=0;	//串口接收超时计数。串口超过3ms未接收到数据，视为超时。单位：ms

/******************************************************************************
* Function Name --> 串口4接收中断服务程序
* Description   --> none
* Input         --> none
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void UART4_IRQHandler(void)
{
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //接收到数据
	{
		F_UART4_RX_RECEIVING=1;//正在接收串口数据
		UART4_RX_TIMEOUT_COUNT=0;//超时计数清零
		UART4_RX_BUF[UART4_RX_COUNT] = USART_ReceiveData(UART4);	//读取接收到的数据
		UART4_RX_COUNT++;
		if(UART4_RX_COUNT>UART4_REC_LEN)//缓存区溢出
		{UART4_RX_COUNT = 0x0000;}	
		//end 接收未完成   		 		 
	}	//end 接收到数据
}

//=========================================================

#endif	//end使能接收

//=========================================================





