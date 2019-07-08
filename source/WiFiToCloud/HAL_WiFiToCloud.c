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
*������void ClrAtRxBuf(void)
*���ܣ����AT���ڽ��ջ�����ؼĴ���
*���룺��
*�������
*����˵������
*******************************************************************/
void ClrAtRxBuf(void)
{
	memset(AT_RX_BUF, 0x00, MAX_AT_RX_LEN);//��ջ���
	AT_RX_COUNT = 0;
	F_AT_RX_FINISH = 0;
}
/*******************************************************************
*������void SendAtCmd(uint8_t *string)
*���ܣ�����ATָ���ַ���
*���룺
		uint8_t *string �����͵�ATָ��
		uint8_t len ����
*�������
*����˵������
*******************************************************************/
void SendAtCmd(uint8_t *string, uint8_t len)
{
	UART4_SendStrlen(string, len);
	UART4_SendStrlen((uint8_t *)"\r\n", 2);
}

/*******************************************************************
*������void SendStrLen(uint8_t *string, uint8_t len)
*���ܣ������ַ�����ATָ���
*���룺
		uint8_t *string �����͵��ַ���
		uint8_t len ����

*�������
*����˵������
*******************************************************************/
void SendStrLen(uint8_t *string, uint8_t len)
{
	UART4_SendStrlen(string, len);
}
