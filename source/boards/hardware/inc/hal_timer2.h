/******************************************************************************
* @ File name --> timer.h
* @ Author    --> By Liway.Ma
* @ Version   --> V1.0
* @ Date      --> 2017/04/25
* @ Brief     --> 通用定时器2设置相关的函数
*
* @ Copyright (C) 20**
* @ All rights reserved
******************************************************************************/

#ifndef _hal_timer2_h_
#define _hal_timer2_h_

/*===========================================================================*/
#ifdef __cplusplus  /* C++支持 */
	extern "C"{
#endif

#include "stdint.h"
		
extern uint16_t T_X_MS;
extern uint16_t T_Y_MS;
extern uint16_t T_Z_MS;
extern uint16_t T_NET_MS;
extern uint16_t T_ALARM_MS;
		
void TIM2_Init(void);
		
/*===========================================================================*/
#ifdef __cplusplus  /* C++支持 */
	}
#endif
/*===========================================================================*/


#endif  /* endif hal_timer2.h */





