/**********************************
**ͷ�ļ���
**�ļ���: 74hc595.c 
**������: 
**����:   
**��������: ���� 74HC595 ������λ16λ���
**ռ����Դ: 595SI, 595SCK,595RCK
*/
#ifndef _HAL_74HC595_H_
#define _HAL_74HC595_H_

#include "stdint.h"


/************************************************************
*������void HC595_GpioInit(void)
*���ܣ�74HC595�ź����Ŷ�Ӧ��I/O�ڹ��ܳ�ʼ��
*���룺��
*�������
*����˵������
************************************************************/
void HC595_GpioInit(void);

void hc595_out(uint8_t dh, uint8_t dl);

#endif