#ifndef _pt100_h_
#define _pt100_h_

#include "stdint.h"
#include "pcf8591.h"

/********************************************************
*函数名称：int8_t getPt100Temperature(uint8_t ch)
*功能：测算出当前温度
*输入：uint8_t ch，通道号
*输出：输出温度值，有符号，单位：℃
*特殊说明：度范围0℃~101℃
**********************************************************/
int8_t getPt100Temperature(uint8_t ch);

#endif //_pt100_h_
