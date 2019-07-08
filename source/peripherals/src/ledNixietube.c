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
#include "hal_timer2.h"
#include "ledNixietube.h"
static uint8_t Scom = 0;
uint8_t /*volatile*/ Ledbuf[8];
extern uint8_t tick_1ms	;
extern void hc595_out_for_int(uint8_t dH, uint8_t dL);
/*************LED*************************************/
const uint8_t ledseg_tab[]  =
{
        0xC0,/*0*/
        0xF9,/*1*/
        0xA4,/*2*/
        0xB0,/*3*/
        0x99,/*4*/
        0x92,/*5*/
        0x82,/*6*/
        0xF8,/*7*/
        0x80,/*8*/
        0x90,/*9*/
        0x88,/*A*/
        0x83,/*b*/
        0xA7,/*c*/
        0xA1,/*d*/
        0x86,/*E*/
        0x8E,/*F*/
        0x89,/*H 10*/
        0x8B,/*h 11*/
        0xC7,/*L 12*/
        0xAB,/*n 13*/
        0xC8,/*N 14*/
        0xA3,/*o 15*/
        0x8C,/*P 16*/
        0x98,/*q 17*/
        0xAF,/*r 18*/
        0x87,/*t 19 */
        0xC1,/*U 1a*/
        0x91,/*y 1b*/
        0xBF,/*- 1c*/
        0xFF, /*  1D*/
        0xFF,
        0x00, /*  1F*/
 }	  ;
const uint8_t com_tab[8]  ={1,2,4,8,0x10,0x20,0x40,0x80};

/***************************************************
函 数 名：
函数功能：led动态显示
			中断函数里调用，小心编写！！！！
参    数：无
返 回 值：无
****************************************************/
/****************************************************************************************/
//#pragma NOAREGS   //指定编译器不要对该函数使用绝对寄存器寻址方式
void led_scan(void)  
{
	uint8_t segi,dati;
	Scom++;
	if(Scom>7)
	{
		Scom=0;
	}
	Scom &= 7;
	dati=Ledbuf[Scom];
	if((dati&0x7f)>MAX_LEDCH)
		dati=MAX_LEDCH;
	segi=ledseg_tab[dati&0x7f];
	if(dati&0x80)
		segi &=SEG_H;
 	hc595_out_for_int(com_tab[Scom],segi);
}
//#pragma AREGS 
/***************************************************
函 数 名：
函数功能：设置显示内容
入口参数：uint8_t *buf, uint8_t pos,uint8_t len
		  显示内容    开始位置     长度  	
出口参数：：无
返 回 值：无
****************************************************/
void set_led_dispbuf(uint8_t *buf, uint8_t pos,uint8_t len)
{
	uint8_t	i=0;
	while( (pos<8) && (len!=0) )
	{
		Ledbuf[pos]=buf[i];
		i++;
		pos++;
		len--;
	}
}
/***************************************************
函 数 名：
函数功能：
入口参数：  	
出口参数：：无
返 回 值：无
****************************************************/
void clr_led_dispbuf(void)
{
	Ledbuf[0] =LED_SPACE;
	Ledbuf[1] =LED_SPACE;
	Ledbuf[2] =LED_SPACE;
	Ledbuf[3] =LED_SPACE;
	Ledbuf[4] =LED_SPACE;
	Ledbuf[5] =LED_SPACE;
	Ledbuf[6] =LED_SPACE;
	Ledbuf[7] =LED_SPACE;
}
/***************************************************
函 数 名：
函数功能：
入口参数：  	
出口参数：：无
返 回 值：无
****************************************************/
void Bcd2disp(uint8_t *bcd, uint8_t *buf,uint8_t len)
{
	if(len>4)
		len=4;
	while(len--)
	{
		*buf= *bcd >>4;
		buf++;
		*buf= *bcd & 0x0f;
		buf++;
		bcd++;		
	}	

}
