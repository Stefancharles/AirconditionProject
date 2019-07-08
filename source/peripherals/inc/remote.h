#ifndef _REMOTE_H_
#define _REMOTE_H_

#include "sys.h"   
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK MiniSTM32开发板
//红外遥控解码驱动 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/3/12
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
  
#define RDATA 	PAin(0)	 	//红外数据输入脚

//红外遥控识别码(ID),每款遥控器的该值基本都不一样,但也有一样的.
//我们选用的遥控器识别码为0
#define REMOTE_ID 0      		   

extern uint8_t RmtCnt;			//按键按下的次数

/*******************************************************************
*函数：void Remote_Init(void)
*功能：红外遥控初始化,设置PA0作为TIM5的CH1的输入捕获
*输入：无
*输出：无
*特殊说明：无
*******************************************************************/
void Remote_Init(void);

/*******************************************************************
*函数：void TIM5_IRQHandler(void)
*功能：定时器5中断服务程序
*输入：无
*输出：无
*特殊说明：无
*******************************************************************/
void TIM5_IRQHandler(void);

/*******************************************************************
*函数：uint8_t Remote_Scan(void)
*功能：处理红外键盘发出的键值
*输入：无
*输出：0,没有任何按键按下，其他,按下的按键键值.
*特殊说明：无
*******************************************************************/
uint8_t Remote_Scan(void);

 /*********************************************************************
*函数：uint8_t IR_RemoteScan(void)
*功能：处理红外键盘发出的键值，滤除重复码
*输入：无
*输出：0,没有任何按键按下，其他,按下的按键键值.
*特殊说明：当遥控器按键按下时，长按不放，遥控器将发出重复码，这里滤除重复码
**********************************************************************/
uint8_t IR_RemoteScan(void);

#endif /*_REMOTE_H_*/















