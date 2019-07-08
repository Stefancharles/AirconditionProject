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
*
******************************************************************************/
#ifndef _HAL_WiFiToCloud_h_
#define _HAL_WiFiToCloud_h_

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "delay.h"
#include "hal_uart4.h"
#include "WiFiToCloud.h"


#define MAX_AT_RX_LEN	256//最大接收长度
#define MAX_AT_TX_LEN	256//128//最大发送长度
#define AT_RX_BUF 		UART4_RX_BUF//AT串口缓存，它是一个数组
#define F_AT_RX_FINISH	F_UART4_RX_FINISH//AT串口数据接收完毕标志，取值0或1
#define AT_RX_COUNT		UART4_RX_COUNT////AT串口接收到的数据个数

/*******************************************************************
*函数：void ClrAtRxBuf(void)
*功能：清空AT串口接收缓存相关寄存器
*输入：无
*输出：无
*特殊说明：无
*******************************************************************/
void ClrAtRxBuf(void);

/*******************************************************************
*函数：void SendAtCmd(uint8_t *string)
*功能：发送AT指令字符串
*输入：
		uint8_t *string 待发送的AT指令
		uint8_t len 长度
*输出：无
*特殊说明：无
*******************************************************************/
void SendAtCmd(uint8_t *string, uint8_t len);

/*******************************************************************
*函数：void SendStrLen(uint8_t *string, uint8_t len)
*功能：发送字符串到AT指令串口
*输入：
		uint8_t *string 待发送的字符串
		uint8_t len 长度

*输出：无
*特殊说明：无
*******************************************************************/
void SendStrLen(uint8_t *string, uint8_t len);


#endif /*_HAL_WiFiToCloud_h_*/
