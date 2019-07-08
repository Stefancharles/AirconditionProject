/************************************************************************************
*
*
*
*
*
************************************************************************************/

//头文件
#include <math.h>
#include "stm32App.h"
#include "chinese_font.h"
#include "stm32f103_config.h"
#include "ledNixietube.h"
#include "lcd12864.h"
#include "pcf8563.h"
#include "pcf8591.h"
#include "74hc595.h"
#include "NTCthermistor.h"
#include "remote.h"
#include "WiFiToCloud.h"
#include "main.h"

#ifdef PT100
#include "pt100.h"
#endif /*PT100*/


//温控箱相关参数
#define MAX_TEMP (99)  //热敏电阻最大测温值
#define MIN_TEMP (0)//(-20) //热敏电阻最小测温值

#ifdef PT100

#define ERROR_MAX_TEMP2 60//温度2最大可测量温度（根据需要设置）
#define ERROR_MIN_TEMP2 0//温度2最小可测量温度（根据需要设置）

#endif /*PT100*/

#define MAX_CTRL_TEMP 30 //最大可控温度，兼报警温度值

#ifdef POTENTIOMETER

//电位器相关参数
#define VREF_3_3_V 3.28	//参考电压
#define MAX_VR 2.2 //最大电位器电压
#define RESOLUTION_RATIO 10	//分辨率
#define RESOLUTION_MIN_TEMP (MAX_CTRL_TEMP-(uint8_t)(((MAX_VR/VREF_3_3_V)*256)/RESOLUTION_RATIO)) //最低温度(30-(uint8_t)(((2.2/3.2)*256)/RESOLUTION_RATIO))

#endif /* POTENTIOMETER */



int8_t upTemp=28;//28;//温度上限值
int8_t downTemp=20;//20;//温度下限值
uint8_t work=0;//工作状态标志寄存器
uint8_t oldWork=0;//工作状态标志寄存器
uint8_t F_Error = 0; //系统错误标记，0-正常，非0-异常
uint8_t systemPowerOnOff=1;

uint8_t RTC_BCD_Buf[7];//RTC时钟缓存,BCD码格式
uint8_t RTC_Buf[7];//RTC时钟缓存，十六进制格式
uint8_t buf_led[8]={2,0,1,4,0,5,2,0};
int8_t temperature1;//存放温度1数值，由热敏电阻采集的温度，单位：℃

#ifdef PT100
int8_t temperature2;//存放温度2数值，由PT100采集的温度，单位：℃
#endif /*PT100*/

uint8_t secondTemp;	//缓存RTC的秒，特殊说明：编码格式--BCD码

uint8_t RTC_SecondOld;//缓存RTC的秒，以前的数据.
uint8_t RTC_Second;	//缓存RTC的秒，现在的数据.
uint8_t RTC_TestBuf[7];//RTC时钟缓存，特殊说明：编码格式--BCD码

uint8_t alarm=0;//告警标志，0-正常，1-报警

char RTC_TimeTemp[32];//RTC时钟缓存

uint16_t T_ALARM_MS_OLD = 0;

#define P_FAN		RYLAY1//控制风扇寄存器
#define P_LAMP		RYLAY2//控制加热灯寄存器
#define P_LAMP_STA	GPIO_ReadOutputDataBit(RYLAY2_PIN_PORT, RYLAY2_PIN)
#define	ON			1	//开启
#define	OFF			0	//关闭

/*******************************************************************
*函数：void simpleAppInit(void)
*功能：应用初始化
*输入：无
*输出：无
*特殊说明：无
*******************************************************************/
void simpleAppInit(void)
{
	uint8_t temp=0;
	
	set_led_dispbuf(buf_led,0,8); //设置数码管初始显示
	
	printf("空调控制系统-V03-180327\r\n");
	LCD_Init();//初始化显示屏
	LCD_Clr();
	
	IIC_Init();//IIC总线初始化
	IIC1_Init();//IIC1总线初始化
	keyInit();//按键初始化
	
	//继电器初始化
	STM32_GPIOx_Init(RYLAY1_Init);
	STM32_GPIOx_Init(RYLAY2_Init);	
	
	HC595_GpioInit();//初始化74HC5959驱动I/O口
	
	/****************************************************************************
	*系统初始化部分
	*****************************************************************************/
	P_FAN = OFF;//控制风扇寄存器
	P_LAMP = OFF;//控制加热灯寄存器
	
	temp=10;
	while(temp--)
	{
		if(PCF8563_Init() != 1)//初始化RTC
		delay_ms(10);//等待RTC初始化完毕
	}
	//LCD12864初始化显示内容
	LCD_Init();
	LCD_DispFullImg((uint8_t *)newLandEduLogo);	//显示新大陆logo
	LCD_WriteChineseString( 6, 16, (uint8_t *)KongTiaoKongZhiXiTong, 6);
	delay_ms(1000);
	LCD_Clr();//清屏	
	LCD_WriteChineseString( 2, 24, (uint8_t *)newLandEdu, 5);
	LCD_WriteEnglishString( 4, 20, (uint8_t *)"Newland Edu");
	delay_ms(1000);
	LCD_Clr();//清屏
	memset(buf_led,MAX_LEDCH,sizeof(buf_led));//设置数码管初始显示内容
	set_led_dispbuf(buf_led,0,8); //数码管显示初始化
	
	Remote_Init();//红外遥控接收模块初始化
	
#ifdef USE_IWDG
	IWDG_Init();
#endif

	//时间参数初始化
	T_X_MS = 0;
	T_Y_MS = 0;
	T_Z_MS = 0;
	T_NET_MS = 0;
}

/*******************************************************************
*函数：void simpleAppProcess(void)
*功能：应用进程
*输入：无
*输出：无
*特殊说明：无
*******************************************************************/
void simpleAppProcess(void)
{
	uint8_t temp=0;	
	uint8_t NoWiFiToCloud=0;//无云网络服务，0-有服务，1-无服务；
ConnectToCloud:
	NoWiFiToCloud=ConnectCloudProcess();
	while(1)
	{
#ifdef USE_IWDG	
		IWDG_ReloadCounter();//重载看门狗定时器
#endif /*USE_IWDG*/
		
		//网络服务
		if(CloudProcess(NoWiFiToCloud) != 0)
		{
			printf("重连云平台！\r\n");
			LCD_Clr();
			work=0;
			goto ConnectToCloud;//重连服务器
		}
		
#ifndef POTENTIOMETER	
		temp=IR_RemoteScan();
		if(temp)//扫描红外管
		{
			printf("红外遥控器键值码 = %d(十进制)\r\n",temp);
			HalBuzzerBlink (HAL_BUZZER_0, 1, 50, 100);
		}
		IR_HandleProcess(temp);
#endif /*POTENTIOMETER*/
		if(systemPowerOnOff != 1)
		{//关机状态
			continue;
		}

		if(T_X_MS >= 166) //500MS
		{
			LCD_DisplayProcess();//LCD显示进程
			T_X_MS = 0;
		}
		if(T_Y_MS >= 33) //100MS
		{
			NTC_temperatureProcess();//采集温度进程
			if(work==0)
			{
				Air_conditionProcess();//温度调节控制进程
			}
			else
			{
				LCD_ClrDispAirConRun();
			}
			T_Y_MS = 0;
		}
		if(T_Z_MS>=2)
		{
#ifndef POTENTIOMETER
			KEY_HandleProcess();//按键事件处理进程
#endif /*POTENTIOMETER*/
			T_Z_MS=0;	
		}
#if EN_USART1_RX	//以下串口1预留功能代码
		if(F_USART1_RX_RECEIVING)
		{
			USART1_RX_BUF[USART1_RX_COUNT]='\0';
			USART1_SendStr(USART1_RX_BUF);
			F_USART1_RX_RECEIVING=0;
			USART1_RX_COUNT=0;
		}
#endif
		if(work==0)
		{
			AlarmProcess(alarm);//告警进程
		}
	}
}

/*******************************************************************
*函数：uint8_t ConnectCloudProcess(void)
*		连接云平台进程
*功能：和云平台建立有效连接
*输入：无
*输出：返回连接结果，0-有服务，1-无服务；
*特殊说明：无
*******************************************************************/
uint8_t ConnectCloudProcess(void)
{
	uint8_t TryCount=0;
	int8_t temp=0;	
	uint8_t NoWiFiToCloud=0;//无云网络服务，0-有服务，1-无服务；
	
	//初始化显示内容
	LCD_WriteEnglishString(0,0,(unsigned char *)"Connect To");
	LCD_WriteEnglishString(2,0,(unsigned char *)"Server ...");
	delay_ms(2000);//延时1000Ms待系统WiFi通讯模块启动完毕
	LCD_WriteEnglishString(2,0,(unsigned char *)"Server ......");
	printf("连接云平台...\r\n");
	//连接服务器
	for(TryCount=0; TryCount<3; TryCount++)
	{
		temp=ConnectToServer((char *)MY_DEVICE_ID, (char *)MY_SECRET_KEY);
		if(temp != 0)
		{
			NoWiFiToCloud=1;
			printf("Connect To Server ERROR=%d\r\n",temp);
		}
		else
		{
			NoWiFiToCloud=0;
			printf("Connect To Server Success !\r\n");
			break;
		}
	}
	ClrAtRxBuf();//清空AT缓存
	LCD_Clr();
	return NoWiFiToCloud;
}

/*******************************************************************
*函数：uint8_t CloudProcess(uint8_t NoWiFiToCloud)
*		云服务进程
*功能：和云平台的通信进程，对接收到到云平台数据进行解析，反馈云平台请求，
*	  并执行相应的指令。每隔3s发送相关数据到云平台。
*输入：uint8_t NoWiFiToCloud，系统联网情况，0-有联网，1-未联网
*输出：返回联网情况，有联网-0，无联网-1
*特殊说明：无
*******************************************************************/
uint8_t CloudProcess(uint8_t NoWiFiToCloud)
{
	uint8_t IpData[128];
	
	int8_t error=0;
	char RTC_TimeTemp[32];
	static int8_t erroCount=0;
	static uint8_t ReConnectCloudTimeCount=0;//重连服务器计时
	/***********************网络通信进程处理*************************/
	if(F_AT_RX_FINISH)//串口4接收到数据
	{// 接收到数据包
		ESP8266_GetIpData((uint8_t *)AT_RX_BUF, (char *)IpData);
		USER_DataAnalysisProcess((char *)IpData);
		memset(IpData, 0x00, 128);
		ClrAtRxBuf();
	}
	if(T_NET_MS >= 1000)//3S发送一次数据
	{
		T_NET_MS=0;
		//获取时间，并将日期、时间的BCD码转换为日期格式yyyy-mm-dd hh:mm:ss的字符串形式
		//RTC_GetTimeStr((char *)RTC_TimeTemp);//RTC_BCD_Buf
		sprintf(RTC_TimeTemp,"%d%d%d%d-%d%d-%d%d ",
							RTC_BCD_Buf[0]>>4,RTC_BCD_Buf[0]&0x0F,RTC_BCD_Buf[1]>>4,RTC_BCD_Buf[1]&0x0F,
							RTC_BCD_Buf[2]>>4,RTC_BCD_Buf[2]&0x0F,RTC_BCD_Buf[3]>>4,RTC_BCD_Buf[3]&0x0F);
		sprintf(&RTC_TimeTemp[11],"%d%d:%d%d:%d%d",
							RTC_BCD_Buf[4]>>4,RTC_BCD_Buf[4]&0x0F,RTC_BCD_Buf[5]>>4,RTC_BCD_Buf[5]&0x0F,
							RTC_BCD_Buf[6]>>4,RTC_BCD_Buf[6]&0x0F);
		//printf("当前%s\r\n",RTC_TimeTemp);
		error=SE_SendSensor3((char *)"upperLimit", upTemp, 
							 (char *)"lowerLimit", downTemp, 
							 (char *)"currentTemp", (uint8_t)temperature1, (char *)RTC_TimeTemp);
		ClrAtRxBuf();
		error=SE_SendSensor2((char *)"alarm", alarm, 
							 (char *)"power", systemPowerOnOff,(char *)RTC_TimeTemp);
		printf("alarm=%d\r\n",alarm);
		ClrAtRxBuf();
		printf("SE_SendSensor3 Error No. %d\r\n",error);
		//判断是否和服务器失去连接累计
		if(error != 0)
		{
			erroCount++;
		}
		else
		{
			erroCount=0;
		}
		if(erroCount > 6)//通讯失败次数超过6次视为设备离线
		{
			erroCount=0;
			if(NoWiFiToCloud != 1)//之前有连接成功服务器
			{
				return 1;//重新连接服务器
			}
		}
		if(NoWiFiToCloud)
		{//断网情况下
			ReConnectCloudTimeCount++;
			if(ReConnectCloudTimeCount>40)//每隔120s重新连接一次服务器。
			{
				ReConnectCloudTimeCount=0;
				return 1;//重新连接服务器
			}
		}
	}
	return 0;//网络连接正常
}

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
void POTENTIOMETER_Control(int8_t *upValue, int8_t *downValue, uint8_t upCh, uint8_t downCh)
{
	uint8_t upADC,downADC;
	
	PCF8591_ReadChannel(&upADC, upCh);//读取上限通道的ADC
	PCF8591_ReadChannel(&downADC, downCh);//读取下限通道的ADC
	*upValue = (upADC/RESOLUTION_RATIO) + RESOLUTION_MIN_TEMP;
	*downValue = (downADC/RESOLUTION_RATIO) + RESOLUTION_MIN_TEMP;
}
#endif /*POTENTIOMETER*/

/*******************************************************************
*函数：void ERROR_Message(uint8_t erro)
*功能：错误信息显示处理
*输入：uint8_t erro,错误值信息，取值1或2,1是阀值冲突，2是格式错误
*输出：无
*特殊说明：无
*******************************************************************/
void ERROR_Message(uint8_t erro)
{
	uint8_t count;
	if(erro != 0)
	{//系统错误处理
		if(erro == 1)
		{
			LCD_Clr();//清屏
			LCD_WriteEnglishString( 2, 0,(uint8_t *)"Threshold conflict, Please reset!");	
		}
		else if(erro == 2)
		{
			LCD_Clr();//清屏
			LCD_WriteEnglishString( 2, 0,(uint8_t *)"Time format error, Please reset!");
		}
		count=200;
		while(count--)
		{
			delay_ms(10);
#ifdef USE_IWDG	
			IWDG_ReloadCounter();//重载看门狗定时器
#endif /*USE_IWDG*/
		}
		LCD_Clr();//清屏
		LCD_WriteChineseString( 0, 0, (uint8_t *)shangXianZhi, 4);//显示上限值：
		LCD_WriteChineseString( 2, 0, (uint8_t *)xiaXianZhi, 4);//显示下限值：
		LCD_WriteChinese( 0, 80, (uint8_t *)spch_sheShiDu);//显示"℃"
		LCD_WriteChinese( 2, 80, (uint8_t *)spch_sheShiDu);
	}	
}

/*******************************************************************
*函数：void WORKSTATION_Message(uint8_t workTemp, 
*							int8_t upTempReg, 
*							int8_t downTempReg, 
*							int8_t temperature1Temp,
*							int8_t temperature2Temp)
*功能：系统工作信息显示，用于处理显示界面的显示内容处理
*输入：uint8_t workTemp,系统工作状态
*	int8_t upTempReg, 温度警戒上限
*	int8_t downTempReg, 温度警戒下限
*	int8_t temperature1Temp, 温度1-NTC
*	int8_t temperature2Temp,温度2-PT100
*输出：无
*特殊说明：无
*******************************************************************/
#ifdef PT100
void WORKSTATION_Message(uint8_t workTemp, 
							int8_t upTempReg, 
							int8_t downTempReg, 
							int8_t temperature1Temp,
							int8_t temperature2Temp)
#else
void WORKSTATION_Message(uint8_t workTemp, 
							int8_t upTempReg, 
							int8_t downTempReg, 
							int8_t temperature1Temp)
#endif /*PT100*/
{
	switch(workTemp)
	{
		case(0):
				LCD_WriteChineseString( 0, 0, (uint8_t *)shangXianZhi, 4);//显示上限值：
				LCD_WriteChineseString( 2, 0, (uint8_t *)xiaXianZhi, 4);//显示下限值：
				LCD_WriteChineseString( 4, 0, (uint8_t *)WenDu, 2);//显示温度1
#ifndef PT100
				LCD_WriteEnglish( 4, 32, '1');
				LCD_WriteEnglish( 4, 32, ':');
#else
				LCD_WriteEnglish( 4, 32, '1');
				LCD_WriteEnglish( 4, 40, ':');
		
				LCD_WriteChineseString( 6, 0, (uint8_t *)WenDu, 2);//显示温度2
				LCD_WriteEnglish( 6, 32, '2');
				LCD_WriteEnglish( 6, 40, ':');
				LCD_WriteChinese( 6, 72, (uint8_t *)spch_sheShiDu);//显示"℃"
#endif /*PT100*/
				LCD_WriteChinese( 0, 80, (uint8_t *)spch_sheShiDu);//显示"℃"
				LCD_WriteChinese( 2, 80, (uint8_t *)spch_sheShiDu);
				LCD_WriteChinese( 4, 72, (uint8_t *)spch_sheShiDu);
				
				
				LCD_WriteEnglish(4,56,((uint8_t)fabs((float)temperature1Temp))/10+'0');//显示箱内温度1
				LCD_WriteEnglish(4,64,((uint8_t)fabs((float)temperature1Temp))%10+'0');
				if(temperature1Temp >= 0)
				{//正数
					LCD_WriteEnglish(4,48,' ');
				}
				else
				{//负数
					LCD_WriteEnglish(4,48,'-');
				}
#ifdef PT100
				if((temperature2Temp < ERROR_MAX_TEMP2) && (temperature2Temp > ERROR_MIN_TEMP2))
				{//正常温度以内的数值
					LCD_WriteEnglish(6,56,((uint8_t)fabs((float)temperature2Temp))/10+'0');//显示箱内温度2
					LCD_WriteEnglish(6,64,((uint8_t)fabs((float)temperature2Temp))%10+'0');
					if(temperature2Temp >= 0)
					{//正数
						LCD_WriteEnglish(6,48,' ');
					}
					else
					{//负数
						LCD_WriteEnglish(6,48,'-');
					}
				}
				else
				{//超出正常温度
					LCD_WriteEnglish(6,56,'-');//显示箱内温度2
					LCD_WriteEnglish(6,64,'-');
					LCD_WriteEnglish(6,48,'-');
				}
#endif /*PT100*/
			break;//数码管第五位不显示,LCD显示"智能温控箱"
		case(1):
				LCD_WriteEnglishString( 4, 0, (uint8_t *)"               ");
				LCD_WriteEnglishString( 6, 0, (uint8_t *)"               ");
				LCD_WriteChineseString( 6, 24, (uint8_t *)sheZhiShangXianZhi, 5);
			break;//LCD显示"设置上限值"
		case(2):
				LCD_WriteEnglishString( 4, 0, (uint8_t *)"               ");
				LCD_WriteEnglishString( 6, 0, (uint8_t *)"               ");
				LCD_WriteChineseString( 6, 24, (uint8_t *)sheZhiXiaXianZhi, 5);
			break;//LCD显示"设置下限值"
		case(3):
				LCD_WriteEnglishString( 4, 0, (uint8_t *)"               ");
				LCD_WriteEnglishString( 6, 0, (uint8_t *)"               ");
				LCD_WriteChineseString( 6, 24, (uint8_t *)sheZhi, 2);
				LCD_WriteChineseString( 6, 24+32, (uint8_t *)RTC_Nian, 1);
			break;//LCD显示"设置年"
		case(4):
				LCD_WriteEnglishString( 4, 0, (uint8_t *)"               ");
				LCD_WriteEnglishString( 6, 0, (uint8_t *)"               ");
				LCD_WriteChineseString( 6, 24, (uint8_t *)sheZhi, 2);
				LCD_WriteChineseString( 6, 24+32, (uint8_t *)RTC_Yue, 1);
			break;//LCD显示"设置月"
		case(5):
				LCD_WriteEnglishString( 4, 0, (uint8_t *)"               ");
				LCD_WriteEnglishString( 6, 0, (uint8_t *)"               ");
				LCD_WriteChineseString( 6, 24, (uint8_t *)sheZhi, 2);
				LCD_WriteChineseString( 6, 24+32, (uint8_t *)RTC_Ri, 1);
			break;//LCD显示"设置日"
		case(6):
				LCD_WriteEnglishString( 4, 0, (uint8_t *)"               ");
				LCD_WriteEnglishString( 6, 0, (uint8_t *)"               ");
				LCD_WriteChineseString( 6, 24, (uint8_t *)sheZhi, 2);
				LCD_WriteChineseString( 6, 24+32, (uint8_t *)RTC_Shi, 1);
			break;//LCD显示"设置时"
		case(7):
				LCD_WriteEnglishString( 4, 0, (uint8_t *)"               ");
				LCD_WriteEnglishString( 6, 0, (uint8_t *)"               ");
				LCD_WriteChineseString( 6, 24, (uint8_t *)sheZhi, 2);
				LCD_WriteChineseString( 6, 24+32, (uint8_t *)RTC_Fen, 1);
			break;//LCD显示"设置分"
		case(8):
				LCD_WriteEnglishString( 4, 0, (uint8_t *)"               ");
				LCD_WriteEnglishString( 6, 0, (uint8_t *)"               ");
				LCD_WriteChineseString( 6, 24, (uint8_t *)sheZhi, 2);
				LCD_WriteChineseString( 6, 24+32, (uint8_t *)RTC_Miao, 1);
			break;//LCD显示"设置秒"
		default:
				LCD_Clr();//清屏
			break;//默认状态
	}
	LCD_WriteEnglish(0,64,((uint8_t)fabs((float)upTempReg))/10+'0');//显示上限值数值
	LCD_WriteEnglish(0,72,((uint8_t)fabs((float)upTempReg))%10+'0');
	if(upTempReg >= 0)
	{
		LCD_WriteEnglish(0,56,' ');
	}
	else
	{
		LCD_WriteEnglish(0,56,'-');
	}
	LCD_WriteEnglish(2,64,((uint8_t)fabs((float)downTempReg))/10+'0');//显示下限值数值
	LCD_WriteEnglish(2,72,((uint8_t)fabs((float)downTempReg))%10+'0');
	if(downTempReg >= 0)
	{
		LCD_WriteEnglish(2,56,' ');
	}
	else
	{
		LCD_WriteEnglish(2,56,'-');
	}
}

/*******************************************************************
*函数：void LED_TubeMessage(uint8_t workTemp, uint8_t *dataBuf)
*功能：LED数码管信息显示处理，LED数码管信息显示处理，根据系统工作状态处
*		理数码管的显示内容；
*输入：uint8_t workTemp-系统工作状态， 
*			uint8_t *dataBuf-RTC数据，年月日时分秒
*输出：无
*特殊说明：无
*******************************************************************/
void LED_TubeMessage(uint8_t workTemp, uint8_t *dataBuf)
{
	switch(work)
	{
		case(0):
		case(1):
		case(2):
				if(Time_CheckValidity((uint8_t *)dataBuf) != 0)//判断RTC时钟格式是否有误
				{//RTC格式正确
					buf_led[0]=(dataBuf[2]>>4)&0x0F;//月
					buf_led[1]=dataBuf[2]&0x0F;
					buf_led[2]=(dataBuf[3]>>4)&0x0F;//日
					buf_led[3]=dataBuf[3]&0x0F;
					buf_led[4]=(dataBuf[4]>>4)&0x0F;//时
					buf_led[5]=dataBuf[4]&0x0F;
					buf_led[6]=(dataBuf[5]>>4)&0x0F;//分
					buf_led[7]=dataBuf[5]&0x0F;
					if(secondTemp != dataBuf[6])//秒
					{
						secondTemp = dataBuf[6];
						buf_led[5]=((~buf_led[5])&0x80)|buf_led[5];
					}
					set_led_dispbuf(buf_led,0,8);
				}
				else
				{//RTC格式不正确
					buf_led[0]=24;/*r 24*/
					buf_led[1]=25;/*t 25*/
					buf_led[2]=12;/*c 12*/
					buf_led[3]=14;/*E 14*/
					buf_led[4]=24;/*r 24*/
					buf_led[5]=24;/*r 24*/
					buf_led[6]=21;/*o 21*/
					buf_led[7]=24;/*r 24*/
					set_led_dispbuf(buf_led,0,8);
				}
			break;
		case(3)://设置年份
				buf_led[0]=(RTC_Buf[0]/10)&0x0F;
				buf_led[1]=(RTC_Buf[0]%10)&0x0F;;
				buf_led[2]=(RTC_Buf[1]/10)&0x0F;
				buf_led[3]=(RTC_Buf[1]%10)&0x0F;;
				buf_led[4]=(RTC_Buf[2]/10)&0x0F;
				buf_led[5]=(RTC_Buf[2]%10)&0x0F;;
				buf_led[6]=(RTC_Buf[3]/10)&0x0F;
				buf_led[7]=(RTC_Buf[3]%10)&0x0F;;
				if(secondTemp != dataBuf[6])//被设置位闪烁
				{
					secondTemp = dataBuf[6];
					buf_led[2]=29;/*空格 29*/
					buf_led[3]=29;/*空格 29*/
				}
				set_led_dispbuf(buf_led,0,8);
			break;
		case(4)://设置月
				buf_led[0]=(RTC_Buf[0]/10)&0x0F;
				buf_led[1]=(RTC_Buf[0]%10)&0x0F;;
				buf_led[2]=(RTC_Buf[1]/10)&0x0F;
				buf_led[3]=(RTC_Buf[1]%10)&0x0F;;
				buf_led[4]=(RTC_Buf[2]/10)&0x0F;
				buf_led[5]=(RTC_Buf[2]%10)&0x0F;;
				buf_led[6]=(RTC_Buf[3]/10)&0x0F;
				buf_led[7]=(RTC_Buf[3]%10)&0x0F;;
				if(secondTemp != dataBuf[6])
				{//被设置位闪烁
					secondTemp = dataBuf[6];
					buf_led[4]=29;/*空格 29*/
					buf_led[5]=29;/*空格 29*/
				}
				set_led_dispbuf(buf_led,0,8);
			break;
		case(5)://设置日
				buf_led[0]=(RTC_Buf[0]/10)&0x0F;
				buf_led[1]=(RTC_Buf[0]%10)&0x0F;;
				buf_led[2]=(RTC_Buf[1]/10)&0x0F;
				buf_led[3]=(RTC_Buf[1]%10)&0x0F;;
				buf_led[4]=(RTC_Buf[2]/10)&0x0F;
				buf_led[5]=(RTC_Buf[2]%10)&0x0F;;
				buf_led[6]=(RTC_Buf[3]/10)&0x0F;
				buf_led[7]=(RTC_Buf[3]%10)&0x0F;;
				if(secondTemp != dataBuf[6])
				{//被设置位闪烁
					secondTemp = dataBuf[6];
					buf_led[6]=29;/*空格 29*/
					buf_led[7]=29;/*空格 29*/
				}
				set_led_dispbuf(buf_led,0,8);
			break;
		case(6)://设置时
				buf_led[0]=29;/*空格 29*/
				buf_led[1]=(RTC_Buf[4]/10)&0x0F;
				buf_led[2]=(RTC_Buf[4]%10)&0x0F;;
				buf_led[3]=(RTC_Buf[5]/10)&0x0F;
				buf_led[4]=(RTC_Buf[5]%10)&0x0F;;
				buf_led[5]=(RTC_Buf[6]/10)&0x0F;
				buf_led[6]=(RTC_Buf[6]%10)&0x0F;;
				buf_led[7]=29;/*空格 29*/
				if(secondTemp != dataBuf[6])
				{//被设置位闪烁
					secondTemp = dataBuf[6];
					buf_led[1]=29;/*空格 29*/
					buf_led[2]=29;/*空格 29*/
				}
				set_led_dispbuf(buf_led,0,8);
			break;
		case(7)://设置分
				buf_led[0]=29;/*空格 29*/
				buf_led[1]=(RTC_Buf[4]/10)&0x0F;
				buf_led[2]=(RTC_Buf[4]%10)&0x0F;;
				buf_led[3]=(RTC_Buf[5]/10)&0x0F;
				buf_led[4]=(RTC_Buf[5]%10)&0x0F;;
				buf_led[5]=(RTC_Buf[6]/10)&0x0F;
				buf_led[6]=(RTC_Buf[6]%10)&0x0F;;
				buf_led[7]=29;/*空格 29*/
				if(secondTemp != dataBuf[6])
				{//被设置位闪烁
					secondTemp = dataBuf[6];
					buf_led[3]=29;/*空格 29*/
					buf_led[4]=29;/*空格 29*/
				}
				set_led_dispbuf(buf_led,0,8);
			break;
		case(8)://设置秒
				buf_led[0]=29;/*空格 29*/
				buf_led[1]=(RTC_Buf[4]/10)&0x0F;
				buf_led[2]=(RTC_Buf[4]%10)&0x0F;;
				buf_led[3]=(RTC_Buf[5]/10)&0x0F;
				buf_led[4]=(RTC_Buf[5]%10)&0x0F;;
				buf_led[5]=(RTC_Buf[6]/10)&0x0F;
				buf_led[6]=(RTC_Buf[6]%10)&0x0F;;
				buf_led[7]=29;/*空格 29*/
				if(secondTemp != dataBuf[6])
				{//被设置位闪烁
					secondTemp = dataBuf[6];
					buf_led[5]=29;/*空格 29*/
					buf_led[6]=29;/*空格 29*/
				}
				set_led_dispbuf(buf_led,0,8);
			break;
		default: 
				buf_led[0]=14;/*E 14*/
				buf_led[1]=24;/*r 24*/
				buf_led[2]=24;/*r 24*/
				buf_led[3]=21;/*o 21*/
				buf_led[4]=24;/*r 24*/
				buf_led[5]=29;/*空格 29*/
				buf_led[6]=29;/*空格 29*/
				buf_led[7]=29;/*空格 29*/
				set_led_dispbuf(buf_led,0,8);
			break;
	}
}

/*******************************************************************
*函数：void LCD_DisplayProcess(void)
*功能：LCD显示进程
*输入：无
*输出：无
*特殊说明：无
*******************************************************************/
void LCD_DisplayProcess(void)
{
	if(F_Error != 0)
	{//系统错误处理
		ERROR_Message(F_Error);
		F_Error = 0;
	}
#ifdef PT100
	WORKSTATION_Message(work, upTemp, downTemp, temperature1, temperature2);
#else
	WORKSTATION_Message(work, upTemp, downTemp, temperature1);
#endif /*PT100*/
	PCF8563_GetTime((uint8_t *)RTC_BCD_Buf);//读取RTC时钟
	LED_TubeMessage(work, RTC_BCD_Buf);
}

/*******************************************************************
*函数：void LCD_ClrDispAirConRun(void)
*功能：清空LCD显示空调运行状态提示信息
*输入：无
*输出：无
*特殊说明：无
*******************************************************************/
void LCD_ClrDispAirConRun(void)
{
	P_LAMP=OFF;//关灯
	P_FAN=OFF;//关扇散热
	LCD_WriteEnglishString( 0, 96, (uint8_t *)"    ");
	LCD_WriteEnglishString( 2, 96, (uint8_t *)"    ");
	LCD_WriteEnglishString( 4, 96, (uint8_t *)"    ");
	//LCD_WriteEnglishString( 6, 96, (uint8_t *)"    ");
}

/*******************************************************************
*函数：void NTC_temperatureProcess(void)
*功能：采集温度部分进程
*输入：无
*输出：无
*特殊说明：无
*******************************************************************/
void NTC_temperatureProcess(void)
{
		temperature1 = getTemperature(0);//采集ADC0通道的热敏电阻温度
		if(temperature1 > MAX_TEMP || temperature1 <= MIN_TEMP)	 //PCF8591通信异常
		{
			;
//			printf("读取ADC1失败 或 超出温度测量范围！\r\n");
//			printf("Read ADC1 Error !\r\n");
		}
#ifdef PT100
		temperature2 = getPt100Temperature(1);//采集ADC0通道的Pt100温度
		if(temperature2 > ERROR_MAX_TEMP2 || temperature2 <= ERROR_MIN_TEMP2)	 //PCF8591通信异常
		{
			;
//			printf("读取ADC2失败 或 超出温度测量范围！\r\n");
//			printf("Read ADC2 Error !\r\n");
		}
#endif /*PT100*/
		
/****************************************************************************
*电位器调整上下限温度部分
*****************************************************************************/
#ifdef POTENTIOMETER
		POTENTIOMETER_Control(&upTemp, &downTemp, 2, 3);
#endif /*POTENTIOMETER*/
}

/*******************************************************************
*函数：void Air_conditionProcess(void)
*功能：温度控制进程
*输入：无
*输出：无
*特殊说明：无
*******************************************************************/
void Air_conditionProcess(void)
{
	//根据上、下限温度值，自动控制箱内温度
	/*
	P33口用于控制风扇转动或停止,P34口用于控制加热灯的打开或关闭。

	1. 
	函数“void PCF8563_GetTime(uint8_t *ptr);”用于获取RTC时钟。
	获取到的时钟信息“ptr”的数据格式如下：（注意：数据格式是BCD码格式）
	*-|----|-------------|------|------|------|------|------|-
	* |数组|ptr[0]~ptr[1]|ptr[2]|ptr[3]|ptr[4]|ptr[5]|ptr[6]|
	*-|----|-------------|------|------|------|------|------|-
	* |解释|年         	 |月    | 日   |时    |分    |秒    |
	*-|----|-------------|------|------|------|------|------|-
	* |示例|0x2017       |0x12  |0x25  |0x12  |0x34  |0x56  |
	*-|----|-------------|------|------|------|------|------|-
	* |时钟|2017年       |12月  |25日  |12时  |34分  |56秒  |
	*-|----|-------------|------|------|------|------|------|-

	2. 
	函数“Convert_BcdToHex(uint8_t *bcd, uint8_t *hex)”用于将数组bcd[]内的BCD码转换成常用的十六进制，并存到数组hex[]中。
	比如bcd[]={0x20,0x17}，经过函数Convert_BcdToHex()处理后，转换结果为hex[]={0x14,0x11}。

	特别说明：函数PCF8563_GetTime()、Convert_BcdToHex()在“hal_pcf8563.c”中定义，本文件默认已经包含头文件“hal_pcf8563.h”。

	3. 
	“int8_t temperature1;//存放温度1数值，由热敏电阻采集的温度，单位：℃”，temperature1是有符号整型数据。
	“uint8_t RTC_SecondOld;//缓存RTC的秒，以前的数据.”，RTC_SecondOld是无符号整型数据。
	“uint8_t RTC_Second;	//缓存RTC的秒，现在的数据.”，RTC_Second是无符号整型数据。
	“uint8_t RTC_TestBuf[7];//RTC时钟缓存，特殊说明：编码格式--BCD码”，数组RTC_TestBuf[]是无符号整型数据。
	上述变量已经在main()函数起始处定义，这里可以直接使用。老师们也可以根据自己的需要定义变量。

	*/

	 //系统控制加热灯和风扇转动来，从而调节温控箱内温度。这部分的功能代码由竞赛者补充填写到下方空白区域
		
		/*-----------空白部分起始线-----------*/
		
		//参考代码如下：
		PCF8563_GetTime((uint8_t *)RTC_TestBuf);
		Convert_BcdToHex(&RTC_TestBuf[6], &RTC_Second); //获取RTC秒钟数值
		
		if(temperature1 >= MAX_CTRL_TEMP)
		{//超出系统控制能力，亮灯1S，关灯2S表示警告
			alarm=1;
//			P_FAN = ON;//温度过高，开风扇散热
			//printf("温度失控警告 & temperature1 >= MAX_CTRL_TEMP\r\n");
			/*if(P_LAMP_STA != ON)
			{
				if((RTC_Second-RTC_SecondOld) >= 2)//关灯2秒
				{
					P_LAMP = ON;//开灯
					RTC_SecondOld = RTC_Second;//保存当前时间以供下次参考
					printf("温度失控警告！！\r\n");
				}
			}
			else
			{
				if((RTC_Second-RTC_SecondOld) >= 1)//开灯1秒
				{
					P_LAMP = OFF;//关灯
					RTC_SecondOld = RTC_Second;//保存当前时间以供下次参考
					HalBuzzerBlink (HAL_BUZZER_0, 2, 50, 200);//蜂鸣器警告
					printf("温度失控警告！\r\n");
				}
			}*/
			//LCD_WriteChineseString( 2, 96, (uint8_t *)IND_ShiKong, 2);
			LCD_WriteChineseString( 4, 96, (uint8_t *)IND_JingGao, 2);
			if(temperature1>upTemp)
			{//温度过高，开风扇散热
				P_FAN=ON;
			}
			else
			{//关风扇
				P_FAN=OFF;
			}
			/*if(temperature1<downTemp)
			{//温度过低，开灯加热
				P_LAMP=ON;
			}
			else
			{//关灯
				P_LAMP=OFF;
			}*/
			//显示提示
			if(temperature1<downTemp)
			{//温度过低，开灯加热
				LCD_WriteChineseString( 2, 96, (uint8_t *)IND_JiaRe, 2);
			}
			else if(temperature1>upTemp)
			{//温度过高，开风扇散热
				LCD_WriteChineseString( 2, 96, (uint8_t *)IND_ZhiLeng, 2);
			}
			else
			{
				LCD_WriteEnglishString( 2, 96, (uint8_t *)"    ");
			}
		}
		else
		{//正常控制
			alarm=0;
			RTC_SecondOld = RTC_Second;//保存当前时间以供下次参考
			if(upTemp >= downTemp)
			{
				if(temperature1<downTemp)
				{//温度过低，开灯加热
					P_LAMP=ON;
				}
				else
				{//关灯
					P_LAMP=OFF;
				}
				if(temperature1>upTemp)
				{//温度过高，开风扇散热
					P_FAN=ON;
				}
				else
				{//关风扇
					P_FAN=OFF;
				}
				//显示提示
				if(temperature1<downTemp)
				{//温度过低，开灯加热
					LCD_WriteChineseString( 2, 96, (uint8_t *)IND_JiaRe, 2);
				}
				else if(temperature1>upTemp)
				{//温度过高，开风扇散热
					LCD_WriteChineseString( 2, 96, (uint8_t *)IND_ZhiLeng, 2);
				}
				else
				{
					LCD_WriteEnglishString( 2, 96, (uint8_t *)"    ");
				}
				
				LCD_WriteEnglishString( 0, 96, (uint8_t *)"    ");
				LCD_WriteEnglishString( 4, 96, (uint8_t *)"    ");
				LCD_WriteEnglishString( 6, 96, (uint8_t *)"    ");
			}
			else
			{//处理阀值冲突的情况
				P_FAN=OFF;
				P_LAMP=OFF;
				LCD_WriteChineseString( 0, 96, (uint8_t *)ERR_FaZhi, 2);
				LCD_WriteChineseString( 2, 96, (uint8_t *)ERR_ChongTu, 2);
				LCD_WriteChineseString( 4, 96, (uint8_t *)ERR_TingZhi, 2);
				LCD_WriteChineseString( 6, 96, (uint8_t *)ERR_YunXing, 2);
			}
		}
		
		/*-----------空白部分结束线-----------*/
}

/*******************************************************************
*函数：void WorkStaSwitch(uint8_t *workSta, 
						uint8_t *workStaOld, keyType keyIn)
*功能：工作状态切换
*输入：uint8_t *workSta-当前工作状态,
*		uint8_t *workStaOld-上个回合的工作状态, 
*		keyType keyIn-当前按键键值
*输出：无
*特殊说明：无
*******************************************************************/
void WorkStaSwitch(uint8_t *workSta, uint8_t *workStaOld, 
											keyType keyIn)
{
	if(keyIn==s121)//功能键
	{
		*workSta = *workSta+1;
	}
	
	if(*workStaOld != *workSta)
	{
		if((*workStaOld==2)&&(*workSta == 3))
		{
			dataTransfer(RTC_BCD_Buf, RTC_Buf, DECIMAL_FORMAT, 7);
		}
		if((*workStaOld==8)&&(*workSta == 9))
		{
			dataTransfer(RTC_Buf, RTC_BCD_Buf, BCD_FORMAT, 7);
		}
		*workStaOld = *workSta;
	}

	if(*workSta>=9)
	{
		if(upTemp < downTemp)
		{//错误设置，需重新设置
			*workSta=1;
			F_Error = 1; //系统错误标记,上下限温度冲突
		}
		else if(Time_CheckValidity(RTC_BCD_Buf) != 1)// 备　　注: 返回值为0表示不合法, 为1表示合法
		{
			*workSta=3;
			F_Error = 2; //系统错误标记,时间格式不合规则
		}
		else
		{//正确设置，进入正常工作状态
			PCF8563_WirteTime(RTC_BCD_Buf);//写入新的时间
			printf("设置正确，进入正常工作状态,并写入新的RTC时间\r\n");
			LCD_Clr();
			*workSta=0;
		}
		
	}
	
	if(*workSta==1)//设置上限温度
	{ 
		if(keyIn==s106)//"上"键
		{
			if(upTemp<MAX_TEMP)
			{upTemp++;}
		}
		if(keyIn==s116)//"下"键
		{
			if(upTemp>MIN_TEMP)
			{upTemp--;}
		}
	}
	if(*workSta==2)//设置下限温度
	{
		if(keyIn==s106)//"上"键
		{
			if(downTemp<MAX_TEMP)
			{downTemp++;}
		}
		if(keyIn==s116)//"下"键
		{
			if(downTemp>MIN_TEMP)
			{downTemp--;}
		}
	}
	switch(*workSta)
	{
		case(0):
			break;
		case(1):
			break;
		case(2):
			break;
		case(3)://设置年份
				if(keyIn==s106)//"上"键
				{
					if(RTC_Buf[1]<99)
					{RTC_Buf[1]++;}
				}
				if(keyIn==s116)//"下"键
				{
					if(RTC_Buf[1]>0)
					{RTC_Buf[1]--;}
				}
			break;
		case(4)://设置月
				if(keyIn==s106)//"上"键
				{
					if(RTC_Buf[2]<12)
					{RTC_Buf[2]++;}
				}
				if(keyIn==s116)//"下"键
				{
					if(RTC_Buf[2]>1)
					{RTC_Buf[2]--;}
				}
			break;
		case(5)://设置日
				if(keyIn==s106)//"上"键
				{
					if(RTC_Buf[3]<31)
					{RTC_Buf[3]++;}
				}
				if(keyIn==s116)//"下"键
				{
					if(RTC_Buf[3]>1)
					{RTC_Buf[3]--;}
				}
			break;
		case(6)://设置时
				if(keyIn==s106)//"上"键
				{
					if(RTC_Buf[4]<23)
					{RTC_Buf[4]++;}
				}
				if(keyIn==s116)//"下"键
				{
					if(RTC_Buf[4]>0)
					{RTC_Buf[4]--;}
				}
			break;
		case(7)://设置分
				if(keyIn==s106)//"上"键
				{
					if(RTC_Buf[5]<59)
					{RTC_Buf[5]++;}
				}
				if(keyIn==s116)//"下"键
				{
					if(RTC_Buf[5]>0)
					{RTC_Buf[5]--;}
				}
			break;
		case(8)://设置秒
				if(keyIn==s106)//"上"键
				{
					if(RTC_Buf[6]<59)
					{RTC_Buf[6]++;}
				}
				if(keyIn==s116)//"下"键
				{
					if(RTC_Buf[6]>0)
					{RTC_Buf[6]--;}
				}
			break;
		default: break;
	}
}

/*******************************************************************
*函数：void KEY_HandleProcess(void)
*功能：按键事件处理进程
*输入：无
*输出：无
*特殊说明：无
*******************************************************************/
void KEY_HandleProcess(void)
{
	keyType keyValueTemp=noneKey;//按键键值寄存器
	
	keyValueTemp=KEY_ScanTriggerOnce();//按键扫描
	WorkStaSwitch(&work, &oldWork, keyValueTemp);
}

/*******************************************************************
*函数：void IR_HandleProcess(uint8_t irData)
*功能：红外遥控按键键值处理进程
*输入：uint8_t irData，红外键值
*输出：无
*特殊说明：无
*******************************************************************/
void IR_HandleProcess(uint8_t irData)
{
//遥控器第一行 CHANNEL
#define PWR_ON_OFF 162//用作开关机
#define MENU 226//用作设置键
//遥控器第二行
#define PREV 224//设置上限值
#define NEXT 144//设置下限值
#define PLAY_PAUSE 168//设置键
//遥控器第三行
#define VOL_ADD 2//加数值
#define VOL_SUB 152//减数值
#define E_QUIT 194//退出

	switch(irData)
	{
		case(PWR_ON_OFF):
			systemPowerOnOff = !systemPowerOnOff;
			if(systemPowerOnOff != 1)
			{
				LCD_Clr();
				LCD12864_BL  = 1;    //关闭背光
				memset(buf_led,MAX_LEDCH,sizeof(buf_led));//设置数码管初始显示内容
				set_led_dispbuf(buf_led,0,8); //数码管显示初始化，置为全灭
				P_FAN = OFF;//控制风扇
				P_LAMP = OFF;//控制加热灯
				delay_ms(40);//等待数码管刷新完成
#ifdef USE_IWDG	
				IWDG_ReloadCounter();//重载看门狗定时器
#endif /*USE_IWDG*/
			}
			else
			{
				NVIC_SystemReset();//重启系统
			}
			break;
		case(PREV):
			work = 1;
			break;
		case(NEXT):
			work = 2;
			break;
		case(MENU):
		case(PLAY_PAUSE):
			work++;
			if(work>=3)
			{
				LCD_Clr();
				work = 0;
			}
			break;
		case(VOL_ADD):
			if(work == 1)
			{
				if(upTemp < MAX_TEMP)
				{upTemp++;}
			}
			else if(work == 2)
			{
				if(downTemp < MAX_TEMP)
				{downTemp++;}
			}
			break;
		case(VOL_SUB):
			if(work == 1)
			{
				if(upTemp > MIN_TEMP)
				{upTemp--;}
			}
			else if(work == 2)
			{
				if(downTemp > MIN_TEMP)
				{downTemp--;}
			}
			break;
		case(E_QUIT):
			LCD_Clr();
			work = 0;
			break;
		default:
			//work = 0;
			break;
	
	}
	
	if(work == 1)
	{
		if(upTemp<downTemp)
		{
			upTemp++;//downTemp--;
		}
	}
	else if(work == 2)
	{
		if(upTemp<downTemp)
		{
			downTemp--;//upTemp++;
		}
	}
}

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
						char *ApiTag2, uint32_t Value2,char *TimeStr)
{
	uint8_t TryGo = 0;
	int8_t error = 0;
	uint8_t TrySend = 0;
	uint8_t TxetBuf[MAX_AT_TX_LEN];
	memset(TxetBuf,0x00,MAX_AT_TX_LEN);//清空缓存
	sprintf((char *)TxetBuf,"{\"t\":3,\"datatype\":2,\"datas\":{\"%s\":{\"%s\":%d},\"%s\":{\"%s\":%d},\"%s\":{\"%s\":%d}},\"msgid\":001}",
																ApiTag0,TimeStr,Value0,
																ApiTag1,TimeStr,Value1,
																ApiTag2,TimeStr,Value2);
se_send_sensor:
	if(ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf)) < 0)
	{//发送失败
		error=-1;
	}
	else
	{//发送成功
		for(TryGo = 0; TryGo<50; TryGo++)//最多等待时间50*10=500ms
		{
			if(strstr((const char *)AT_RX_BUF, (const char *)"\"status\":0") == NULL)//检查响应状态是否为"上报成功"
			{
				error = -1;
			}
			else
			{
				error = 0;
				break;
			}
			delay_ms(10);
		}
	}
	if((error != 0)&&(TrySend<2))
	{
		printf("重发传感数据%d次\r\n",TrySend+1);
		TrySend++;
		goto se_send_sensor;
	}
	return error;
}

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
						char *ApiTag1, uint32_t Value1,char *TimeStr)
{
	uint8_t TryGo = 0;
	int8_t error = 0;
	uint8_t TrySend = 0;
	uint8_t TxetBuf[MAX_AT_TX_LEN];
	memset(TxetBuf,0x00,MAX_AT_TX_LEN);//清空缓存
	sprintf((char *)TxetBuf,"{\"t\":3,\"datatype\":2,\"datas\":{\"%s\":{\"%s\":%d},\"%s\":{\"%s\":%d}},\"msgid\":001}",
																ApiTag0,TimeStr,Value0,
																ApiTag1,TimeStr,Value1);
se_send_sensor:
	if(ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf)) < 0)
	{//发送失败
		error=-1;
	}
	else
	{//发送成功
		for(TryGo = 0; TryGo<50; TryGo++)//最多等待时间50*10=500ms
		{
			if(strstr((const char *)AT_RX_BUF, (const char *)"\"status\":0") == NULL)//检查响应状态是否为"上报成功"
			{
				error = -1;
			}
			else
			{
				error = 0;
				break;
			}
			delay_ms(10);
		}
	}
	if((error != 0)&&(TrySend<2))
	{
		printf("重发传感数据%d次\r\n",TrySend+1);
		TrySend++;
		goto se_send_sensor;
	}
	return error;
}

/*******************************************************************
*函数：char *USER_GetJsonValue(char *cJson, char *Tag)
*功能：json为字符串序列，将json格式中的目标对象Tag对应的值字符串提取出来
*输入：
		char *cJson json字符串
		char *Tag 要操作的对象标签
*输出：返回数值的字符串形式的启始地址
*特殊说明：用户可以在此基础上改造和扩展该函数，这里只是个简单的DEMO
*******************************************************************/
char *USER_GetJsonValue(char *cJson, char *Tag)
{
	char *target = NULL;
	static char temp[10];
	int8_t i=0;
	
	memset(temp, 0x00, 128);
	sprintf(temp,"\"%s\":",Tag);
	target=strstr((const char *)cJson, (const char *)temp);
	if(target == NULL)
	{
		//printf("空字符！\r\n");
		return NULL;
	}
	i=strlen((const char *)temp);
	target=target+i;
	memset(temp, 0x00, 128);
	for(i=0; i<10; i++, target++)//数值超过10个位为非法，由于2^32=4294967296
	{
		if((*target<='9')&&(*target>='0'))
		{
			temp[i]=*target;
		}
		else
		{
			break;
		}
	}
	temp[i+1] = '\0';
	//printf("数值=%s\r\n",temp);
	return (char *)temp;
}

/*******************************************************************
*函数：char *USER_GetTempJsonValue(char *cJson, char *Tag)
*功能：json为字符串序列，将json格式中的目标对象Tag对应的值字符串转换为数值
*输入：
		char *cJson json字符串
		char *Tag 要操作的对象标签
*输出：返回数值
*特殊说明：用户可以在此基础上改造和扩展该函数，这里只是个简单的DEMO
*******************************************************************/
uint8_t USER_GetTempJsonValue(char *cJson, char *Tag)
{
	char *target = NULL;
	static char temp[10];
	uint8_t len=0;
	uint8_t value=0;
	int8_t i=0;
	
	memset(temp, 0x00, 128);
	sprintf(temp,"\"%s\":\"",Tag);
	target=strstr((const char *)cJson, (const char *)temp);
	if(target == NULL)
	{
		//printf("空字符！\r\n");
		return NULL;
	}
	i=strlen((const char *)temp);
	target=target+i;
	memset(temp, 0x00, 128);
	for(i=0; i<10; i++, target++)//数值超过10个位为非法，由于2^32=4294967296
	{
		if((*target<='9')&&(*target>='0'))
		{
			temp[i]=*target;
		}
		else
		{
			break;
		}
	}
	temp[i+1] = '\0';
	//printf("数值str=%s\r\n",temp);
	len=strlen((const char *)temp);
	switch(len)
	{
		case(1):value=temp[0]-'0';break;
		case(2):value=temp[1]-'0'+(temp[0]-'0')*10;break;
		case(3):value=temp[2]-'0'+(temp[0]-'0')*100+(temp[1]-'0')*10;break;
		default:break;
	}
	//printf("数值value=%d\r\n",value);
	return value;
}

/*******************************************************************
*函数：void USER_DataAnalysisProcess(char *RxBuf)
*功能：解析服务器数据
*输入：char *RxBuf 服务器下发数据
*输出：
*特殊说明：用户可以在此基础上改造和扩展该函数，这里只是个简单的DEMO
*******************************************************************/
void USER_DataAnalysisProcess(char *RxBuf)
{
	char *cmdid = NULL;
	uint8_t TxetBuf[128];
	uint8_t angle=0;//刻度
	if(strstr((const char *)RxBuf, (const char *)PING_REQ) != NULL)//心跳请求？
	{
		if(ESP8266_IpSend((char *)PING_RSP, strlen((const char *)PING_RSP)) < 0)//响应心跳
		{//发送失败
			printf("发送心跳包失败！\r\n");
		}
		else
		{
			printf("心跳包！\r\n");
		}
	}
	else if(strstr((const char *)RxBuf, (const char *)"\"t\":5") != NULL)//命令请求？
	{
		if(strstr((const char *)RxBuf, (const char *)"\"apitag\":\"powerCtrl\"") != NULL)//控制开关机
		{
			memset(TxetBuf,0x00,128);//清空缓存
			if((strstr((const char *)RxBuf, (const char *)"\"data\":1") != NULL))//开机
			{
				printf("开机！\r\n");
				;//...
				;//...
				cmdid = USER_GetJsonValue((char *)RxBuf, (char *)"cmdid");
				sprintf((char *)TxetBuf,"{\"t\":6,\"cmdid\":%s,\"status\":0,\"data\":1}",cmdid);
				//printf("%s\r\n",TxetBuf);////////////////////////////////////////////////////////////
				if(ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf)) < 0)
				{//发送失败
					printf("发送响应失败！\r\n");
				}
				/*******************************************发送开关机信息**********************************************/
				sprintf(RTC_TimeTemp,"%d%d%d%d-%d%d-%d%d ",
							RTC_BCD_Buf[0]>>4,RTC_BCD_Buf[0]&0x0F,RTC_BCD_Buf[1]>>4,RTC_BCD_Buf[1]&0x0F,
							RTC_BCD_Buf[2]>>4,RTC_BCD_Buf[2]&0x0F,RTC_BCD_Buf[3]>>4,RTC_BCD_Buf[3]&0x0F);
				sprintf(&RTC_TimeTemp[11],"%d%d:%d%d:%d%d",
							RTC_BCD_Buf[4]>>4,RTC_BCD_Buf[4]&0x0F,RTC_BCD_Buf[5]>>4,RTC_BCD_Buf[5]&0x0F,
							RTC_BCD_Buf[6]>>4,RTC_BCD_Buf[6]&0x0F);
				if(SE_SendSensor2((char *)"alarm", alarm, 
							 (char *)"power", 1/*systemPowerOnOff*/,(char *)RTC_TimeTemp))
				{//发送失败
					printf("发送开/关机状态失败！\r\n");
				}
				/*******************************************开关机显示处理********************************************/
				if(systemPowerOnOff != 1)
				{//若之前的状态是关机，才执行下面的开关指令
					PowerOffDisplay(1);
				}
				systemPowerOnOff=1;
			}
			else if((strstr((const char *)RxBuf, (const char *)"\"data\":0") != NULL))//关机
			{
				printf("关机！\r\n");
				P_FAN = OFF;//控制风扇
				P_LAMP = OFF;//控制加热灯
				systemPowerOnOff=0;
				PowerOffDisplay(systemPowerOnOff);
				;//...
				;//...
				cmdid = USER_GetJsonValue((char *)RxBuf, (char *)"cmdid");
				sprintf((char *)TxetBuf,"{\"t\":6,\"cmdid\":%s,\"status\":0,\"data\":0}",cmdid);
				//printf("%s\r\n",TxetBuf);////////////////////////////////////////////////////////////
				if(ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf)) < 0)
				{//发送失败
					printf("发送响应失败！\r\n");
				}
				/*******************************************发送开关机信息**********************************************/
				sprintf(RTC_TimeTemp,"%d%d%d%d-%d%d-%d%d ",
							RTC_BCD_Buf[0]>>4,RTC_BCD_Buf[0]&0x0F,RTC_BCD_Buf[1]>>4,RTC_BCD_Buf[1]&0x0F,
							RTC_BCD_Buf[2]>>4,RTC_BCD_Buf[2]&0x0F,RTC_BCD_Buf[3]>>4,RTC_BCD_Buf[3]&0x0F);
				sprintf(&RTC_TimeTemp[11],"%d%d:%d%d:%d%d",
							RTC_BCD_Buf[4]>>4,RTC_BCD_Buf[4]&0x0F,RTC_BCD_Buf[5]>>4,RTC_BCD_Buf[5]&0x0F,
							RTC_BCD_Buf[6]>>4,RTC_BCD_Buf[6]&0x0F);
				if(SE_SendSensor2((char *)"alarm", alarm, 
							 (char *)"power", systemPowerOnOff,(char *)RTC_TimeTemp))
				{//发送失败
					printf("发送开/关机状态失败！\r\n");
				}
			}
		}
#ifndef POTENTIOMETER	
		else if(strstr((const char *)RxBuf, (const char *)"\"apitag\":\"upperLimitCtrl\"") != NULL)//设置上限值
		{
			memset(TxetBuf,0x00,128);//清空缓存
			angle=USER_GetTempJsonValue((char *)RxBuf, (char *)"data");
			upTemp=angle;
			printf("设置上限值：%d！℃\r\n",angle);
		}
		else if(strstr((const char *)RxBuf, (const char *)"\"apitag\":\"lowerLimitCtrl\"") != NULL)//设置下限值
		{
			memset(TxetBuf,0x00,128);//清空缓存
			angle=USER_GetTempJsonValue((char *)RxBuf, (char *)"data");
			downTemp=angle;
			printf("设置下限值：%d！℃\r\n",angle);
		}
#endif /*POTENTIOMETER*/
	}
}

/*******************************************************************
*函数：void PowerOffDisplay(uint8_t power)
*功能：电源开关机显示界面处理
*输入：uint8_t power， 0是关机，1是开机
*输出：无
*特殊说明：无
*******************************************************************/
void PowerOffDisplay(uint8_t power)
{
	if(power != 1)
	{
		LCD_Clr();
		LCD12864_BL  = 1;    //关闭背光
		memset(buf_led,MAX_LEDCH,sizeof(buf_led));//设置数码管初始显示内容
		set_led_dispbuf(buf_led,0,8); //数码管显示初始化，置为全灭
		delay_ms(40);//等待数码管刷新完成
#ifdef USE_IWDG	
		IWDG_ReloadCounter();//重载看门狗定时器
#endif /*USE_IWDG*/
	}
	else
	{
		NVIC_SystemReset();//重启系统
	}
}

/*******************************************************************
*函数：void AlarmProcess(uint8_t alarm)
*功能：告警进程处理
*输入：uint8_t alarm，取值0或1,1的时候执行告警，0的时候不执行
*输出：无
*特殊说明：无
*******************************************************************/
void AlarmProcess(uint8_t alarm)
{
	if(alarm != 0)
	{
		if(P_LAMP_STA != ON)
		{
			if((uint16_t)(T_ALARM_MS - T_ALARM_MS_OLD) > (2000/3))//关灯2秒
			{
				T_ALARM_MS_OLD = (uint16_t)T_ALARM_MS;
				P_LAMP = ON;//开灯
				printf("温度失控警告！！\r\n");
			}
		}
		else
		{
			if((uint16_t)(T_ALARM_MS - T_ALARM_MS_OLD) > (1000/3))//开灯1秒
			{
				T_ALARM_MS_OLD = (uint16_t)T_ALARM_MS;
				P_LAMP = OFF;//关灯
				HalBuzzerBlink (HAL_BUZZER_0, 2, 50, 200);//蜂鸣器警告
				printf("温度失控警告！\r\n");
			}
		}
	}
	else
	{
		T_ALARM_MS_OLD = T_ALARM_MS = 0;
	}
}

