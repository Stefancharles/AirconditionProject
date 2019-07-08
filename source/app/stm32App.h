#ifndef _STM32APP_H_
#define _STM32APP_H_

#include "stdint.h"

/*******************************************************************
*函数：void simpleAppInit(void)
*功能：应用初始化
*输入：无
*输出：无
*特殊说明：无
*******************************************************************/
void simpleAppInit(void);

/*******************************************************************
*函数：void simpleAppProcess(void)
*功能：打印机应用
*输入：无
*输出：无
*特殊说明：无
*******************************************************************/
void simpleAppProcess(void);

/*******************************************************************
*函数：uint8_t ConnectCloudProcess(void)
*		连接云平台进程
*功能：和云平台建立有效连接
*输入：无
*输出：返回连接结果，成功-0，失败-1
*特殊说明：无
*******************************************************************/
uint8_t ConnectCloudProcess(void);

/*******************************************************************
*函数：uint8_t CloudProcess(uint8_t NoWiFiToCloud)
*		云服务进程
*功能：和云平台的通信进程，对接收到到云平台数据进行解析，反馈云平台请求，
*	  并执行相应的指令。每隔3s发送相关数据到云平台。
*输入：uint8_t NoWiFiToCloud，系统联网情况，0-有联网，1-未联网
*输出：返回联网情况，有联网-0，无联网-1
*特殊说明：无
*******************************************************************/
uint8_t CloudProcess(uint8_t NoWiFiToCloud);

/*************************************************************************************************
*函数：POTENTIOMETER_Control(uint8_t* upValue, uint8_t* downValue, uint8_t upCh, uint8_t downCh)
*功能：根据对应的ADC通道，采集到的电位器的电压信号数字量大小，来确定上下限温度值
*输入：
*	uint8_t* upValue, 指向上限值寄存器地址
*	uint8_t* downValue, 指向下限值寄存器地址
*	uint8_t upCh, 设置上限值的ADC通道
*	uint8_t downCh, 设置下限值得ADC通道
*输出：无
*特别说明：硬件中电位器电压信号范围0~1.6V，温度控制范围18~30设施度度，具体和硬件电路的电压信号大小相关。
*************************************************************************************************/
#ifdef POTENTIOMETER
void POTENTIOMETER_Control(int8_t *upValue, int8_t *downValue, uint8_t upCh, uint8_t downCh);
#endif

/*******************************************************************
*函数：void LCD_DisplayProcess(void)
*功能：LCD显示进程
*输入：无
*输出：无
*特殊说明：无
*******************************************************************/
void LCD_DisplayProcess(void);

/*******************************************************************
*函数：void LCD_ClrDispAirConRun(void)
*功能：清空LCD显示空调运行状态提示信息
*输入：无
*输出：无
*特殊说明：无
*******************************************************************/
void LCD_ClrDispAirConRun(void);

/*******************************************************************
*函数：void NTC_temperatureProcess(void)
*功能：采集温度部分进程
*输入：无
*输出：无
*特殊说明：无
*******************************************************************/
void NTC_temperatureProcess(void);

/*******************************************************************
*函数：void Air_conditionProcess(void)
*功能：温度控制进程
*输入：无
*输出：无
*特殊说明：无
*******************************************************************/
void Air_conditionProcess(void);

/*******************************************************************
*函数：void KEY_HandleProcess(void)
*功能：按键事件处理进程
*输入：无
*输出：无
*特殊说明：无
*******************************************************************/
void KEY_HandleProcess(void);

/*******************************************************************
*函数：void IR_HandleProcess(uint8_t irData)
*功能：红外遥控按键键值处理进程
*输入：uint8_t irData，红外键值
*输出：无
*特殊说明：无
*******************************************************************/
void IR_HandleProcess(uint8_t irData);

/*******************************************************************
*函数：int8_t SE_SendSensor3(char *ApiTag0, uint32_t Value0, 
							char *ApiTag1, uint32_t Value1, 
							char *ApiTag2, uint32_t Value2,char *TimeStr)
*功能：发送传感数据到服务器
*输入：
	char *ApiTag0 实体对象标签名称
	uint32_t sensor0, 传感数值
	char *ApiTag1 实体对象标签名称
	uint32_t sensor1, 传感数值
	char *ApiTag2 实体对象标签名称
	uint32_t sensor2, 传感数值
	char *TimeStr，采集数据时的时间，须是yyyy-mm-dd hh:mm:ss格式
*输出：
		return = 0 ,sucess
		return < 0 ,error
*特殊说明：
*******************************************************************/
 int8_t SE_SendSensor3(char *ApiTag0, uint32_t Value0, 
						char *ApiTag1, uint32_t Value1, 
						char *ApiTag2, uint32_t Value2,char *TimeStr);
						
/*******************************************************************
*函数：int8_t SE_SendSensor2(char *ApiTag0, uint32_t Value0, 
							char *ApiTag1, uint32_t Value1,char *TimeStr)
*功能：发送传感数据到服务器
*输入：
	char *ApiTag0 实体对象标签名称
	uint32_t sensor0, 传感数值
	char *ApiTag1 实体对象标签名称
	uint32_t sensor1, 传感数值
	char *TimeStr，采集数据时的时间，须是yyyy-mm-dd hh:mm:ss格式
*输出：
		return = 0 ,sucess
		return < 0 ,error
*特殊说明：
*******************************************************************/
 int8_t SE_SendSensor2(char *ApiTag0, uint32_t Value0, 
						char *ApiTag1, uint32_t Value1,char *TimeStr);
						
/*******************************************************************
*函数：char *USER_GetJsonValue(char *cJson, char *Tag)
*功能：json为字符串序列，将json格式中的目标对象Tag对应的值字符串提取出来
*输入：
		char *cJson json字符串
		char *Tag 要操作的对象标签
*输出：返回数值的字符串形式的启始地址
*特殊说明：用户可以在此基础上改造和扩展该函数，这里只是个简单的DEMO
*******************************************************************/
char *USER_GetJsonValue(char *cJson, char *Tag);

/*******************************************************************
*函数：char *USER_GetTempJsonValue(char *cJson, char *Tag)
*功能：json为字符串序列，将json格式中的目标对象Tag对应的值字符串转换为数值
*输入：
		char *cJson json字符串
		char *Tag 要操作的对象标签
*输出：返回数值
*特殊说明：用户可以在此基础上改造和扩展该函数，这里只是个简单的DEMO
*******************************************************************/
uint8_t USER_GetTempJsonValue(char *cJson, char *Tag);

/*******************************************************************
*函数：void USER_DataAnalysisProcess(char *RxBuf)
*功能：解析服务器数据
*输入：char *RxBuf 服务器下发数据
*输出：
*特殊说明：用户可以在此基础上改造和扩展该函数，这里只是个简单的DEMO
*******************************************************************/
void USER_DataAnalysisProcess(char *RxBuf);

/*******************************************************************
*函数：void PowerOffDisplay(uint8_t power)
*功能：电源开关机显示界面处理
*输入：uint8_t power， 0是关机，1是开机
*输出：无
*特殊说明：无
*******************************************************************/
void PowerOffDisplay(uint8_t power);

/*******************************************************************
*函数：void AlarmProcess(uint8_t alarm)
*功能：告警进程处理
*输入：uint8_t alarm，取值0或1,1的时候执行告警，0的时候不执行
*输出：无
*特殊说明：无
*******************************************************************/
void AlarmProcess(uint8_t alarm);

#endif	/*_STM32APP_H_*/
