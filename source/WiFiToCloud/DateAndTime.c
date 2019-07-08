/********************************************************************************
  * @file    
  * @author  
  * @version
  *******************************************************************************/ 

/* Includes ------------------------------------------------------------------*/

#include "stdint.h"
#include "stdio.h"
#include "stm32f10x.h"
#include "DateAndTime.h"

//月份数据表                                                                       
const uint8_t Day_CorrectTable[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //月修正数据表  
//平年的月份日期表
const uint8_t Month_Table[12]={31,28,31,30,31,30,31,31,30,31,30,31};
//当前时间和日期
DateAndTime_TypeDef CurrentTime;


/*******************************************************************
*函数：void RTC_Init(void)
*功能：初始化RTC
*输入：无
*输出：无
*特殊说明：无
*******************************************************************/
void RTC_Init(void)
{
  /* NVIC configuration */
  RTC_NVIC_Configuration();

  if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
    /* Backup data register value is not correct or not yet programmed (when
       the first time the program is executed) */

    printf("RTC not yet configured...\r\n");

    /* RTC Configuration */
    RTC_Configuration();

    printf("RTC configured...\r\n");

    /* Adjust time by values entered by the user on the hyperterminal */
    RTC_SetTime(2018, 8, 31, 9, 30, 00);

    BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
  }
  else
  {
    /* Check if the Power On Reset flag is set */
    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
    {
      printf("Power On Reset occurred...\r\n");
    }
    /* Check if the Pin Reset flag is set */
    else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
    {
      printf("External Reset occurred...\r\n");
    }

    printf("No need to configure RTC...\r\n");
    /* Wait for RTC registers synchronization */
    RTC_WaitForSynchro();

    /* Enable the RTC Second */
    RTC_ITConfig(RTC_IT_SEC, ENABLE);
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
  }

#ifdef RTCClockOutput_Enable
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Disable the Tamper Pin */
  BKP_TamperPinCmd(DISABLE); /* To output RTCCLK/64 on Tamper pin, the tamper
                                 functionality must be disabled */

  /* Enable RTC Clock Output on Tamper Pin */
  BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);
#endif

  /* Clear reset flags */
  RCC_ClearFlag();
}

/*******************************************************************
*函数：void RTC_NVIC_Configuration(void)
*功能：RTC中断配置
*输入：无
*输出：无
*特殊说明：无
*******************************************************************/
void RTC_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************
*函数：void RTC_Configuration(void)
*功能：RTC配置
*输入：无
*输出：无
*特殊说明：无
*******************************************************************/
void RTC_Configuration(void)
{
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset Backup Domain */
  BKP_DeInit();

  /* Enable LSE */
  RCC_LSEConfig(RCC_LSE_ON);
  /* Wait till LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {}

  /* Select LSE as RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  /* Enable RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Enable the RTC Second */
  RTC_ITConfig(RTC_IT_SEC, ENABLE);

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Set RTC prescaler: set RTC period to 1sec */
  RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}

/*******************************************************************
*函数：void Time_Adjust(uint8_t hour, uint8_t minute, uint8_t second)
*功能：RTC时间调整设置
*输入：uint8_t hour-时, uint8_t minute-分, uint8_t second-秒
*输出：无
*特殊说明：该函数只配置时钟，不配置日期
*******************************************************************/
void Time_Adjust(uint8_t hour, uint8_t minute, uint8_t second)
{
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
  /* Change the current time */
  RTC_SetCounter((hour*3600 + minute*60 + second));
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

}

/*******************************************************************
*函数：void RTC_IRQHandler(void)
*功能：RTC中断
*输入：无
*输出：无
*特殊说明：中断处理
*******************************************************************/
void RTC_IRQHandler(void)
{
	
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
	{
		/* Clear the RTC Second interrupt */
		RTC_ClearITPendingBit(RTC_IT_SEC);
		
//		static uint32_t TimeVar;
//		TimeVar = RTC_GetCounter();
//		/* Reset RTC Counter when Time is 23:59:59 */
//		if (TimeVar == 0x0001517f)
//		{
//			Time_Adjust(0, 0, 0);
//		}
//		/* Wait until last write operation on RTC registers has finished */
//		RTC_WaitForLastTask();
	}
}

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
int8_t LeapYearJudge(uint16_t year)
{                    
	if(year%4==0) //必须能被4整除
	{
		if(year%100==0)
		{
			if(year%400==0)
				return SET;//如果以00结尾,还要能被400整除          
			else 
				return RESET;  
		}
		else 
		{
			return SET;  
		}
	}
	else 
	{
		return RESET;
	}
}            

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
					uint8_t hour, uint8_t min, uint8_t sec)
{
	uint16_t t;
	uint32_t SecCount=0;
	if(year<2000||year>2099)
		return SET;//year范围1970-2099，此处设置范围为2000-2099       
	for(t=1970;t<year;t++) //把所有年份的秒钟相加
	{
		if(LeapYearJudge(t))
			SecCount+=31622400;//闰年的秒钟数
		else
			SecCount+=31536000;                    //平年的秒钟数
	}
	mon-=1;
	for(t=0;t<mon;t++)         //把前面月份的秒钟数相加
	{
		SecCount+=(uint32_t)Month_Table[t]*86400;//月份秒钟数相加
		if(LeapYearJudge(year)&&t==1)
			SecCount+=86400;//闰年2月份增加一天的秒钟数        
	}
	SecCount+=(uint32_t)(day-1)*86400;//把前面日期的秒钟数相加
	SecCount+=(uint32_t)hour*3600;//小时秒钟数
	SecCount+=(uint32_t)min*60;      //分钟秒钟数
	SecCount+=sec;//最后的秒钟加上去

	//设置时钟
	
	/* 
	//下面三步是必须的!
	RCC->APB1ENR|=1<<28;//使能电源时钟
	RCC->APB1ENR|=1<<27;//使能备份时钟
	PWR->CR|=1<<8;    //取消备份区写保护
	*/
	PWR_BackupAccessCmd(ENABLE);
	RTC_WaitForLastTask();
	RTC_SetCounter(SecCount);
	RTC_WaitForLastTask();
	return RESET;     
}
 
/*******************************************************************
*函数：uint8_t RTC_GetWeek(uint16_t year,uint8_t month,uint8_t day)
*功能：判断输入的日期是星期几
*输入：uint16_t year-年, uint8_t month-月, uint8_t day-日,
*	   公历年月日
*输出：返回星期号  
*特殊说明：
*	输入公历日期得到星期(只允许1901-2099年)
*******************************************************************/                                                                                                                                                
uint8_t RTC_GetWeek(uint16_t year,uint8_t month,uint8_t day)
{    
	uint16_t temp2;
	uint8_t yearH,yearL;

	yearH=year/100;     yearL=year%100;
	// 如果为21世纪,年份数加100 
	if (yearH>19)yearL+=100;
	// 所过闰年数只算1900年之后的 
	temp2=yearL+yearL/4;
	temp2=temp2%7;
	temp2=temp2+day+Day_CorrectTable[month-1];
	if (yearL%4==0&&month<3)temp2--;
	return(temp2%7);
}

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
DateAndTime_TypeDef RTC_GetTime(void)
{
	
	static uint16_t DayCnt=0;
	uint32_t TimeCount=0;
	uint32_t temp=0;
	uint16_t temp1=0;

	TimeCount=RTC_GetCounter();

	/*TimeCount=RTC->CNTH;//得到计数器中的值(秒钟数)
	TimeCount<<=16;
	TimeCount+=RTC->CNTL;    */          

	temp=TimeCount/86400;   //得到天数(秒钟数对应的)
	if(DayCnt!=temp)//超过一天了
	{
		DayCnt=temp;
		temp1=1970;  //从1970年开始
		while(temp>=365)
		{                         
			if(LeapYearJudge(temp1))//是闰年
			{
				if(temp>=366)
				{	
					temp-=366;//闰年的秒钟数
				}
				else 
				{
					temp1++;
					break;
				} 
			}
			else //平年
			{
				temp-=365;      
			} 
			temp1++; 
		}  
		CurrentTime.year=temp1;//得到年份
		temp1=0;
		while(temp>=28)//超过了一个月
		{
			if(LeapYearJudge(CurrentTime.year)&&temp1==1)//当年是不是闰年/2月份
			{
				if(temp>=29)
					temp-=29;//闰年的秒钟数
				else 
					break;
			}
			else
			{
				if(temp>=Month_Table[temp1])//平年
					temp-=Month_Table[temp1];
				else 
					break;
			}
			temp1++; 
		}
		CurrentTime.month=temp1+1;//得到月份
		CurrentTime.date=temp+1;  //得到日期
	}
	temp=TimeCount%86400;     //得到秒钟数      
	CurrentTime.hour=temp/3600;     //小时
	CurrentTime.min=(temp%3600)/60; //分钟     
	CurrentTime.sec=(temp%3600)%60; //秒钟
	CurrentTime.week=RTC_GetWeek(CurrentTime.year,CurrentTime.month,CurrentTime.date);//获取星期  
	return CurrentTime;
}    

/*******************************************************************
*函数：void RTC_GetTimeStr(char *GetRtcStr)
*功能：获取当前日期和时间的字符串形式
*输入：char *GetRtcStr指向用于存放RTC日期和时间的缓存
*输出：无
*特殊说明：
*			生成的日期格式是yyyy-mm-dd hh:mm:ss
*******************************************************************/  
void RTC_GetTimeStr(char *GetRtcStr)
{
	DateAndTime_TypeDef nowaday;
	nowaday=RTC_GetTime();
	sprintf(GetRtcStr,"%d-%d%d-%d%d %d%d:%d%d:%d%d",
						nowaday.year,nowaday.month/10,nowaday.month%10,nowaday.date/10,nowaday.date%10,
						nowaday.hour/10,nowaday.hour%10,nowaday.min/10,nowaday.min%10,nowaday.sec/10,nowaday.sec%10);
//	printf("%s\r\n",GetRtcStr);
}	




