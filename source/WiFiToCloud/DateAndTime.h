/******************************************************************************
*                                 
* @ Version
* @ Author
* @ Date
* @ Revise
*
******************************************************************************/

#ifndef _DATEANDTIME_h_
#define _DATEANDTIME_h_

/*===========================================================================*/
#ifdef __cplusplus  /* C++支持 */
	extern "C"{
#endif
	
#include "stdint.h"
		
#define RESET 	0
#define SET 	1
		
//时间结构体
typedef struct
{
	//公历年月日周时分秒
	uint16_t year;
	uint8_t  month;
	uint8_t  date;
	uint8_t  week;  
	uint8_t  hour;
	uint8_t  min;
	uint8_t  sec;                           
}DateAndTime_TypeDef;

//当前时间和日期
extern DateAndTime_TypeDef CurrentTime;

/*******************************************************************
*函数：void RTC_Init(void)
*功能：初始化RTC
*输入：无
*输出：无
*特殊说明：无
*******************************************************************/
void RTC_Init(void);

/*******************************************************************
*函数：void RTC_NVIC_Configuration(void)
*功能：RTC中断配置
*输入：无
*输出：无
*特殊说明：无
*******************************************************************/
void RTC_NVIC_Configuration(void);

/*******************************************************************
*函数：void RTC_Configuration(void)
*功能：RTC配置
*输入：无
*输出：无
*特殊说明：无
*******************************************************************/
void RTC_Configuration(void);

/*******************************************************************
*函数：void Time_Adjust(uint8_t hour, uint8_t minute, uint8_t second)
*功能：RTC时间调整设置
*输入：uint8_t hour-时, uint8_t minute-分, uint8_t second-秒
*输出：无
*特殊说明：该函数只配置时钟，不配置日期
*******************************************************************/
void Time_Adjust(uint8_t hour, uint8_t minute, uint8_t second);

/*******************************************************************
*函数：int8_t LeapYearJudge(uint16_t year)
*功能：判断是否是闰年
*输入：uint16_t year,年份
*输出：该年份是不是闰年.SET-是，RESET-不是
*特殊说明：
*		月份   1  2  3  4  5  6  7  8  9  10 11 12
*		闰年   31 29 31 30 31 30 31 31 30 31 30 31
*		非闰年 31 28 31 30 31 30 31 31 30 31 30 31
*******************************************************************/
int8_t LeapYearJudge(uint16_t year);       

/*******************************************************************
*函数：int8_t RTC_SetTime(uint16_t year, uint8_t mon, uint8_t day,
*						   uint8_t hour, uint8_t min, uint8_t sec)
*功能：设置日期和时间
*输入：uint16_t year-年, uint8_t mon-月, uint8_t day-日,
*	   uint8_t hour-时, uint8_t min-分, uint8_t sec-秒
*输出：返回RESET,成功; 返回SET,错误.
*特殊说明：
*	程序将把输入的时钟转换为秒钟，
*	以1970年1月1日为基准，
*	1970~2099年为合法年份
*******************************************************************/
int8_t RTC_SetTime(uint16_t year, uint8_t mon, uint8_t day,
					uint8_t hour, uint8_t min, uint8_t sec);
 
/*******************************************************************
*函数：uint8_t RTC_GetWeek(uint16_t year,uint8_t month,uint8_t day)
*功能：判断输入的日期是星期几
*输入：uint16_t year-年, uint8_t month-月, uint8_t day-日,
*	   公历年月日
*输出：返回星期号  
*特殊说明：
*	输入公历日期得到星期(只允许1901-2099年)
*******************************************************************/                                                                                                                                                
uint8_t RTC_GetWeek(uint16_t year,uint8_t month,uint8_t day);

/*******************************************************************
*函数：DateAndTime_TypeDef RTC_GetTime(void)
*功能：获取当前日期和时间
*输入：无
*输出：返回DateAndTime_TypeDef格式的日期和时间
*特殊说明：
			typedef struct
			{
				//公历年月日周时分秒
				uint16_t year;
				uint8_t  month;
				uint8_t  date;
				uint8_t  week;  
				uint8_t  hour;
				uint8_t  min;
				uint8_t  sec;                           
			}DateAndTime_TypeDef;
*******************************************************************/  
DateAndTime_TypeDef RTC_GetTime(void);

/*******************************************************************
*函数：void RTC_GetTimeStr(char *GetRtcStr)
*功能：获取当前日期和时间的字符串形式
*输入：char *GetRtcStr指向用于存放RTC日期和时间的缓存
*输出：无
*特殊说明：
*			生成的日期格式是yyyy-mm-dd hh:mm:ss
*******************************************************************/  
void RTC_GetTimeStr(char *GetRtcStr);

		
/*===========================================================================*/
#ifdef __cplusplus  /* C++支持 */
	}
#endif
/*===========================================================================*/


#endif  /* endif _DATEANDTIME_h_ */






