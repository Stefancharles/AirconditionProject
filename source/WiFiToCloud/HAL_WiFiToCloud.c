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
/**********************************************************
	Modify History: qiuzhb make, 2016-8-21
**********************************************************/
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "delay.h"
#include "hal_uart4.h"
#include "WiFiToCloud.h"
#include "HAL_WiFiToCloud.h"


/*******************************************************************
*函数：void ClrAtRxBuf(void)
*功能：清空AT串口接收缓存相关寄存器
*输入：无
*输出：无
*特殊说明：无
*******************************************************************/
void ClrAtRxBuf(void)
{
	memset(AT_RX_BUF, 0x00, MAX_AT_RX_LEN);//清空缓存
	AT_RX_COUNT = 0;
	F_AT_RX_FINISH = 0;
}
/*******************************************************************
*函数：void SendAtCmd(uint8_t *string)
*功能：发送AT指令字符串
*输入：
		uint8_t *string 待发送的AT指令
		uint8_t len 长度
*输出：无
*特殊说明：无
*******************************************************************/
void SendAtCmd(uint8_t *string, uint8_t len)
{
	UART4_SendStrlen(string, len);
	UART4_SendStrlen((uint8_t *)"\r\n", 2);
}

/*******************************************************************
*函数：void SendStrLen(uint8_t *string, uint8_t len)
*功能：发送字符串到AT指令串口
*输入：
		uint8_t *string 待发送的字符串
		uint8_t len 长度

*输出：无
*特殊说明：无
*******************************************************************/
void SendStrLen(uint8_t *string, uint8_t len)
{
	UART4_SendStrlen(string, len);
}
