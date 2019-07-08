/**************************************************************************************************************
*
*
*
*
*
*
*
**************************************************************************************************************/

#include "stm32f103_config.h"
#include "74hc595.h"

#define _74HC595_NOP_()	delay_us(1)

/************************************************************
*������void HC595_GpioInit(void)
*���ܣ�74HC595�ź����Ŷ�Ӧ��I/O�ڹ��ܳ�ʼ��
*���룺��
*�������
*����˵������
************************************************************/
void HC595_GpioInit(void)
{
	STM32_GPIOx_Init(IC74HC595_DAT_Init);
	STM32_GPIOx_Init(IC74HC595_SCK_Init);
	STM32_GPIOx_Init(IC74HC595_RCK_Init);
	IC74HC595_DAT = 1;
	IC74HC595_SCK = 1;
	IC74HC595_RCK = 1;
}

void hc595_out(uint8_t dH, uint8_t dL)
{
   uint16_t dout;
   uint8_t i;
   dout = (dH<<8) | dL;
   IC74HC595_RCK =0 ;
   IC74HC595_SCK=0;
   // ������λ���
   for(i=0; i<16; i++)
   {
        IC74HC595_SCK =0 ;
        if(dout &0x8000) 
           IC74HC595_DAT=1;
        else
           IC74HC595_DAT=0;
		_74HC595_NOP_();
        IC74HC595_SCK =1 ;
       	_74HC595_NOP_();
	    dout = dout<<1;
   }
   IC74HC595_SCK=0;
   // �������
   IC74HC595_RCK =1 ;
   _74HC595_NOP_();
   _74HC595_NOP_();
   IC74HC595_RCK =0 ;
}

void hc595_out_for_int(uint8_t dH, uint8_t dL)
{
   uint16_t dout;
   uint8_t i;
   dout = (dH<<8) | dL;
   IC74HC595_RCK =0 ;
   IC74HC595_SCK=0;
   // ������λ���
   for(i=0; i<16; i++)
   {
        IC74HC595_SCK =0 ;
        if(dout &0x8000) 
           IC74HC595_DAT=1;
        else
           IC74HC595_DAT=0;
		_74HC595_NOP_();
        IC74HC595_SCK =1 ;
       	_74HC595_NOP_();
	    dout = dout<<1;
   }
   IC74HC595_SCK=0;
   // �������
   IC74HC595_RCK =1 ;
   _74HC595_NOP_();
   _74HC595_NOP_();
   IC74HC595_RCK =0 ;
}
/******************* (C) COPYRIGHT xxx *****END OF FILE****/









