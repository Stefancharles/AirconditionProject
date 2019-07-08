#ifndef _HAL_KEY_H_
#define _HAL_KEY_H_


enum keyValueType
{
	s101=0,s102,s103,s104,s105,s106,s107,s108,s109,s110,s111,s112,s113,
	s114,s115,s116,s117,s118,s119,s120,s121,s122,s123,s124,s125,noneKey
};

typedef enum keyValueType keyType;

/******************************************************
函数名称：void keyInit(void)
功能：独立按键IO口初始化
输入：无
输出：无
******************************************************/
void keyInit(void);

/******************************************************
函数名称：enum keyValueType keyScanInd(void)
功能：独立按键扫描
输入：无
输出：按键键值
******************************************************/
keyType keyScanInd(void);

/******************************************************
函数名称：keyType KEY_ScanTriggerOnce(void)
功能：独立按键扫描，按下有效，仅触发一次
输入：无
输出：按键键值
******************************************************/
keyType KEY_ScanTriggerOnce(void);

#endif /*_HAL_KEY_H_*/
