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


#define MAX_AT_RX_LEN	256//�����ճ���
#define MAX_AT_TX_LEN	256//128//����ͳ���
#define AT_RX_BUF 		UART4_RX_BUF//AT���ڻ��棬����һ������
#define F_AT_RX_FINISH	F_UART4_RX_FINISH//AT�������ݽ�����ϱ�־��ȡֵ0��1
#define AT_RX_COUNT		UART4_RX_COUNT////AT���ڽ��յ������ݸ���

/*******************************************************************
*������void ClrAtRxBuf(void)
*���ܣ����AT���ڽ��ջ�����ؼĴ���
*���룺��
*�������
*����˵������
*******************************************************************/
void ClrAtRxBuf(void);

/*******************************************************************
*������void SendAtCmd(uint8_t *string)
*���ܣ�����ATָ���ַ���
*���룺
		uint8_t *string �����͵�ATָ��
		uint8_t len ����
*�������
*����˵������
*******************************************************************/
void SendAtCmd(uint8_t *string, uint8_t len);

/*******************************************************************
*������void SendStrLen(uint8_t *string, uint8_t len)
*���ܣ������ַ�����ATָ���
*���룺
		uint8_t *string �����͵��ַ���
		uint8_t len ����

*�������
*����˵������
*******************************************************************/
void SendStrLen(uint8_t *string, uint8_t len);


#endif /*_HAL_WiFiToCloud_h_*/
