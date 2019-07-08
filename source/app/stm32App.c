/************************************************************************************
*
*
*
*
*
************************************************************************************/

//ͷ�ļ�
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


//�¿�����ز���
#define MAX_TEMP (99)  //��������������ֵ
#define MIN_TEMP (0)//(-20) //����������С����ֵ

#ifdef PT100

#define ERROR_MAX_TEMP2 60//�¶�2���ɲ����¶ȣ�������Ҫ���ã�
#define ERROR_MIN_TEMP2 0//�¶�2��С�ɲ����¶ȣ�������Ҫ���ã�

#endif /*PT100*/

#define MAX_CTRL_TEMP 30 //���ɿ��¶ȣ��汨���¶�ֵ

#ifdef POTENTIOMETER

//��λ����ز���
#define VREF_3_3_V 3.28	//�ο���ѹ
#define MAX_VR 2.2 //����λ����ѹ
#define RESOLUTION_RATIO 10	//�ֱ���
#define RESOLUTION_MIN_TEMP (MAX_CTRL_TEMP-(uint8_t)(((MAX_VR/VREF_3_3_V)*256)/RESOLUTION_RATIO)) //����¶�(30-(uint8_t)(((2.2/3.2)*256)/RESOLUTION_RATIO))

#endif /* POTENTIOMETER */



int8_t upTemp=28;//28;//�¶�����ֵ
int8_t downTemp=20;//20;//�¶�����ֵ
uint8_t work=0;//����״̬��־�Ĵ���
uint8_t oldWork=0;//����״̬��־�Ĵ���
uint8_t F_Error = 0; //ϵͳ�����ǣ�0-��������0-�쳣
uint8_t systemPowerOnOff=1;

uint8_t RTC_BCD_Buf[7];//RTCʱ�ӻ���,BCD���ʽ
uint8_t RTC_Buf[7];//RTCʱ�ӻ��棬ʮ�����Ƹ�ʽ
uint8_t buf_led[8]={2,0,1,4,0,5,2,0};
int8_t temperature1;//����¶�1��ֵ������������ɼ����¶ȣ���λ����

#ifdef PT100
int8_t temperature2;//����¶�2��ֵ����PT100�ɼ����¶ȣ���λ����
#endif /*PT100*/

uint8_t secondTemp;	//����RTC���룬����˵���������ʽ--BCD��

uint8_t RTC_SecondOld;//����RTC���룬��ǰ������.
uint8_t RTC_Second;	//����RTC���룬���ڵ�����.
uint8_t RTC_TestBuf[7];//RTCʱ�ӻ��棬����˵���������ʽ--BCD��

uint8_t alarm=0;//�澯��־��0-������1-����

char RTC_TimeTemp[32];//RTCʱ�ӻ���

uint16_t T_ALARM_MS_OLD = 0;

#define P_FAN		RYLAY1//���Ʒ��ȼĴ���
#define P_LAMP		RYLAY2//���Ƽ��ȵƼĴ���
#define P_LAMP_STA	GPIO_ReadOutputDataBit(RYLAY2_PIN_PORT, RYLAY2_PIN)
#define	ON			1	//����
#define	OFF			0	//�ر�

/*******************************************************************
*������void simpleAppInit(void)
*���ܣ�Ӧ�ó�ʼ��
*���룺��
*�������
*����˵������
*******************************************************************/
void simpleAppInit(void)
{
	uint8_t temp=0;
	
	set_led_dispbuf(buf_led,0,8); //��������ܳ�ʼ��ʾ
	
	printf("�յ�����ϵͳ-V03-180327\r\n");
	LCD_Init();//��ʼ����ʾ��
	LCD_Clr();
	
	IIC_Init();//IIC���߳�ʼ��
	IIC1_Init();//IIC1���߳�ʼ��
	keyInit();//������ʼ��
	
	//�̵�����ʼ��
	STM32_GPIOx_Init(RYLAY1_Init);
	STM32_GPIOx_Init(RYLAY2_Init);	
	
	HC595_GpioInit();//��ʼ��74HC5959����I/O��
	
	/****************************************************************************
	*ϵͳ��ʼ������
	*****************************************************************************/
	P_FAN = OFF;//���Ʒ��ȼĴ���
	P_LAMP = OFF;//���Ƽ��ȵƼĴ���
	
	temp=10;
	while(temp--)
	{
		if(PCF8563_Init() != 1)//��ʼ��RTC
		delay_ms(10);//�ȴ�RTC��ʼ�����
	}
	//LCD12864��ʼ����ʾ����
	LCD_Init();
	LCD_DispFullImg((uint8_t *)newLandEduLogo);	//��ʾ�´�½logo
	LCD_WriteChineseString( 6, 16, (uint8_t *)KongTiaoKongZhiXiTong, 6);
	delay_ms(1000);
	LCD_Clr();//����	
	LCD_WriteChineseString( 2, 24, (uint8_t *)newLandEdu, 5);
	LCD_WriteEnglishString( 4, 20, (uint8_t *)"Newland Edu");
	delay_ms(1000);
	LCD_Clr();//����
	memset(buf_led,MAX_LEDCH,sizeof(buf_led));//��������ܳ�ʼ��ʾ����
	set_led_dispbuf(buf_led,0,8); //�������ʾ��ʼ��
	
	Remote_Init();//����ң�ؽ���ģ���ʼ��
	
#ifdef USE_IWDG
	IWDG_Init();
#endif

	//ʱ�������ʼ��
	T_X_MS = 0;
	T_Y_MS = 0;
	T_Z_MS = 0;
	T_NET_MS = 0;
}

/*******************************************************************
*������void simpleAppProcess(void)
*���ܣ�Ӧ�ý���
*���룺��
*�������
*����˵������
*******************************************************************/
void simpleAppProcess(void)
{
	uint8_t temp=0;	
	uint8_t NoWiFiToCloud=0;//�����������0-�з���1-�޷���
ConnectToCloud:
	NoWiFiToCloud=ConnectCloudProcess();
	while(1)
	{
#ifdef USE_IWDG	
		IWDG_ReloadCounter();//���ؿ��Ź���ʱ��
#endif /*USE_IWDG*/
		
		//�������
		if(CloudProcess(NoWiFiToCloud) != 0)
		{
			printf("������ƽ̨��\r\n");
			LCD_Clr();
			work=0;
			goto ConnectToCloud;//����������
		}
		
#ifndef POTENTIOMETER	
		temp=IR_RemoteScan();
		if(temp)//ɨ������
		{
			printf("����ң������ֵ�� = %d(ʮ����)\r\n",temp);
			HalBuzzerBlink (HAL_BUZZER_0, 1, 50, 100);
		}
		IR_HandleProcess(temp);
#endif /*POTENTIOMETER*/
		if(systemPowerOnOff != 1)
		{//�ػ�״̬
			continue;
		}

		if(T_X_MS >= 166) //500MS
		{
			LCD_DisplayProcess();//LCD��ʾ����
			T_X_MS = 0;
		}
		if(T_Y_MS >= 33) //100MS
		{
			NTC_temperatureProcess();//�ɼ��¶Ƚ���
			if(work==0)
			{
				Air_conditionProcess();//�¶ȵ��ڿ��ƽ���
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
			KEY_HandleProcess();//�����¼��������
#endif /*POTENTIOMETER*/
			T_Z_MS=0;	
		}
#if EN_USART1_RX	//���´���1Ԥ�����ܴ���
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
			AlarmProcess(alarm);//�澯����
		}
	}
}

/*******************************************************************
*������uint8_t ConnectCloudProcess(void)
*		������ƽ̨����
*���ܣ�����ƽ̨������Ч����
*���룺��
*������������ӽ����0-�з���1-�޷���
*����˵������
*******************************************************************/
uint8_t ConnectCloudProcess(void)
{
	uint8_t TryCount=0;
	int8_t temp=0;	
	uint8_t NoWiFiToCloud=0;//�����������0-�з���1-�޷���
	
	//��ʼ����ʾ����
	LCD_WriteEnglishString(0,0,(unsigned char *)"Connect To");
	LCD_WriteEnglishString(2,0,(unsigned char *)"Server ...");
	delay_ms(2000);//��ʱ1000Ms��ϵͳWiFiͨѶģ���������
	LCD_WriteEnglishString(2,0,(unsigned char *)"Server ......");
	printf("������ƽ̨...\r\n");
	//���ӷ�����
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
	ClrAtRxBuf();//���AT����
	LCD_Clr();
	return NoWiFiToCloud;
}

/*******************************************************************
*������uint8_t CloudProcess(uint8_t NoWiFiToCloud)
*		�Ʒ������
*���ܣ�����ƽ̨��ͨ�Ž��̣��Խ��յ�����ƽ̨���ݽ��н�����������ƽ̨����
*	  ��ִ����Ӧ��ָ�ÿ��3s����������ݵ���ƽ̨��
*���룺uint8_t NoWiFiToCloud��ϵͳ���������0-��������1-δ����
*������������������������-0��������-1
*����˵������
*******************************************************************/
uint8_t CloudProcess(uint8_t NoWiFiToCloud)
{
	uint8_t IpData[128];
	
	int8_t error=0;
	char RTC_TimeTemp[32];
	static int8_t erroCount=0;
	static uint8_t ReConnectCloudTimeCount=0;//������������ʱ
	/***********************����ͨ�Ž��̴���*************************/
	if(F_AT_RX_FINISH)//����4���յ�����
	{// ���յ����ݰ�
		ESP8266_GetIpData((uint8_t *)AT_RX_BUF, (char *)IpData);
		USER_DataAnalysisProcess((char *)IpData);
		memset(IpData, 0x00, 128);
		ClrAtRxBuf();
	}
	if(T_NET_MS >= 1000)//3S����һ������
	{
		T_NET_MS=0;
		//��ȡʱ�䣬�������ڡ�ʱ���BCD��ת��Ϊ���ڸ�ʽyyyy-mm-dd hh:mm:ss���ַ�����ʽ
		//RTC_GetTimeStr((char *)RTC_TimeTemp);//RTC_BCD_Buf
		sprintf(RTC_TimeTemp,"%d%d%d%d-%d%d-%d%d ",
							RTC_BCD_Buf[0]>>4,RTC_BCD_Buf[0]&0x0F,RTC_BCD_Buf[1]>>4,RTC_BCD_Buf[1]&0x0F,
							RTC_BCD_Buf[2]>>4,RTC_BCD_Buf[2]&0x0F,RTC_BCD_Buf[3]>>4,RTC_BCD_Buf[3]&0x0F);
		sprintf(&RTC_TimeTemp[11],"%d%d:%d%d:%d%d",
							RTC_BCD_Buf[4]>>4,RTC_BCD_Buf[4]&0x0F,RTC_BCD_Buf[5]>>4,RTC_BCD_Buf[5]&0x0F,
							RTC_BCD_Buf[6]>>4,RTC_BCD_Buf[6]&0x0F);
		//printf("��ǰ%s\r\n",RTC_TimeTemp);
		error=SE_SendSensor3((char *)"upperLimit", upTemp, 
							 (char *)"lowerLimit", downTemp, 
							 (char *)"currentTemp", (uint8_t)temperature1, (char *)RTC_TimeTemp);
		ClrAtRxBuf();
		error=SE_SendSensor2((char *)"alarm", alarm, 
							 (char *)"power", systemPowerOnOff,(char *)RTC_TimeTemp);
		printf("alarm=%d\r\n",alarm);
		ClrAtRxBuf();
		printf("SE_SendSensor3 Error No. %d\r\n",error);
		//�ж��Ƿ�ͷ�����ʧȥ�����ۼ�
		if(error != 0)
		{
			erroCount++;
		}
		else
		{
			erroCount=0;
		}
		if(erroCount > 6)//ͨѶʧ�ܴ�������6����Ϊ�豸����
		{
			erroCount=0;
			if(NoWiFiToCloud != 1)//֮ǰ�����ӳɹ�������
			{
				return 1;//�������ӷ�����
			}
		}
		if(NoWiFiToCloud)
		{//���������
			ReConnectCloudTimeCount++;
			if(ReConnectCloudTimeCount>40)//ÿ��120s��������һ�η�������
			{
				ReConnectCloudTimeCount=0;
				return 1;//�������ӷ�����
			}
		}
	}
	return 0;//������������
}

/*************************************************************************************************
*������POTENTIOMETER_Control(uint8_t* upValue, uint8_t* downValue, uint8_t upCh, uint8_t downCh)
*���ܣ����ݶ�Ӧ��ADCͨ�����ɼ����ĵ�λ���ĵ�ѹ�ź���������С����ȷ���������¶�ֵ
*���룺
*	uint8_t* upValue, ָ������ֵ�Ĵ�����ַ
*	uint8_t* downValue, ָ������ֵ�Ĵ�����ַ
*	uint8_t upCh, ��������ֵ��ADCͨ��
*	uint8_t downCh, ��������ֵ��ADCͨ��
*�������
*�ر�˵����Ӳ���е�λ����ѹ�źŷ�Χ0~1.6V���¶ȿ��Ʒ�Χ18~30��ʩ�ȶȣ������Ӳ����·�ĵ�ѹ�źŴ�С��ء�
*************************************************************************************************/
#ifdef POTENTIOMETER
void POTENTIOMETER_Control(int8_t *upValue, int8_t *downValue, uint8_t upCh, uint8_t downCh)
{
	uint8_t upADC,downADC;
	
	PCF8591_ReadChannel(&upADC, upCh);//��ȡ����ͨ����ADC
	PCF8591_ReadChannel(&downADC, downCh);//��ȡ����ͨ����ADC
	*upValue = (upADC/RESOLUTION_RATIO) + RESOLUTION_MIN_TEMP;
	*downValue = (downADC/RESOLUTION_RATIO) + RESOLUTION_MIN_TEMP;
}
#endif /*POTENTIOMETER*/

/*******************************************************************
*������void ERROR_Message(uint8_t erro)
*���ܣ�������Ϣ��ʾ����
*���룺uint8_t erro,����ֵ��Ϣ��ȡֵ1��2,1�Ƿ�ֵ��ͻ��2�Ǹ�ʽ����
*�������
*����˵������
*******************************************************************/
void ERROR_Message(uint8_t erro)
{
	uint8_t count;
	if(erro != 0)
	{//ϵͳ������
		if(erro == 1)
		{
			LCD_Clr();//����
			LCD_WriteEnglishString( 2, 0,(uint8_t *)"Threshold conflict, Please reset!");	
		}
		else if(erro == 2)
		{
			LCD_Clr();//����
			LCD_WriteEnglishString( 2, 0,(uint8_t *)"Time format error, Please reset!");
		}
		count=200;
		while(count--)
		{
			delay_ms(10);
#ifdef USE_IWDG	
			IWDG_ReloadCounter();//���ؿ��Ź���ʱ��
#endif /*USE_IWDG*/
		}
		LCD_Clr();//����
		LCD_WriteChineseString( 0, 0, (uint8_t *)shangXianZhi, 4);//��ʾ����ֵ��
		LCD_WriteChineseString( 2, 0, (uint8_t *)xiaXianZhi, 4);//��ʾ����ֵ��
		LCD_WriteChinese( 0, 80, (uint8_t *)spch_sheShiDu);//��ʾ"��"
		LCD_WriteChinese( 2, 80, (uint8_t *)spch_sheShiDu);
	}	
}

/*******************************************************************
*������void WORKSTATION_Message(uint8_t workTemp, 
*							int8_t upTempReg, 
*							int8_t downTempReg, 
*							int8_t temperature1Temp,
*							int8_t temperature2Temp)
*���ܣ�ϵͳ������Ϣ��ʾ�����ڴ�����ʾ�������ʾ���ݴ���
*���룺uint8_t workTemp,ϵͳ����״̬
*	int8_t upTempReg, �¶Ⱦ�������
*	int8_t downTempReg, �¶Ⱦ�������
*	int8_t temperature1Temp, �¶�1-NTC
*	int8_t temperature2Temp,�¶�2-PT100
*�������
*����˵������
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
				LCD_WriteChineseString( 0, 0, (uint8_t *)shangXianZhi, 4);//��ʾ����ֵ��
				LCD_WriteChineseString( 2, 0, (uint8_t *)xiaXianZhi, 4);//��ʾ����ֵ��
				LCD_WriteChineseString( 4, 0, (uint8_t *)WenDu, 2);//��ʾ�¶�1
#ifndef PT100
				LCD_WriteEnglish( 4, 32, '1');
				LCD_WriteEnglish( 4, 32, ':');
#else
				LCD_WriteEnglish( 4, 32, '1');
				LCD_WriteEnglish( 4, 40, ':');
		
				LCD_WriteChineseString( 6, 0, (uint8_t *)WenDu, 2);//��ʾ�¶�2
				LCD_WriteEnglish( 6, 32, '2');
				LCD_WriteEnglish( 6, 40, ':');
				LCD_WriteChinese( 6, 72, (uint8_t *)spch_sheShiDu);//��ʾ"��"
#endif /*PT100*/
				LCD_WriteChinese( 0, 80, (uint8_t *)spch_sheShiDu);//��ʾ"��"
				LCD_WriteChinese( 2, 80, (uint8_t *)spch_sheShiDu);
				LCD_WriteChinese( 4, 72, (uint8_t *)spch_sheShiDu);
				
				
				LCD_WriteEnglish(4,56,((uint8_t)fabs((float)temperature1Temp))/10+'0');//��ʾ�����¶�1
				LCD_WriteEnglish(4,64,((uint8_t)fabs((float)temperature1Temp))%10+'0');
				if(temperature1Temp >= 0)
				{//����
					LCD_WriteEnglish(4,48,' ');
				}
				else
				{//����
					LCD_WriteEnglish(4,48,'-');
				}
#ifdef PT100
				if((temperature2Temp < ERROR_MAX_TEMP2) && (temperature2Temp > ERROR_MIN_TEMP2))
				{//�����¶����ڵ���ֵ
					LCD_WriteEnglish(6,56,((uint8_t)fabs((float)temperature2Temp))/10+'0');//��ʾ�����¶�2
					LCD_WriteEnglish(6,64,((uint8_t)fabs((float)temperature2Temp))%10+'0');
					if(temperature2Temp >= 0)
					{//����
						LCD_WriteEnglish(6,48,' ');
					}
					else
					{//����
						LCD_WriteEnglish(6,48,'-');
					}
				}
				else
				{//���������¶�
					LCD_WriteEnglish(6,56,'-');//��ʾ�����¶�2
					LCD_WriteEnglish(6,64,'-');
					LCD_WriteEnglish(6,48,'-');
				}
#endif /*PT100*/
			break;//����ܵ���λ����ʾ,LCD��ʾ"�����¿���"
		case(1):
				LCD_WriteEnglishString( 4, 0, (uint8_t *)"               ");
				LCD_WriteEnglishString( 6, 0, (uint8_t *)"               ");
				LCD_WriteChineseString( 6, 24, (uint8_t *)sheZhiShangXianZhi, 5);
			break;//LCD��ʾ"��������ֵ"
		case(2):
				LCD_WriteEnglishString( 4, 0, (uint8_t *)"               ");
				LCD_WriteEnglishString( 6, 0, (uint8_t *)"               ");
				LCD_WriteChineseString( 6, 24, (uint8_t *)sheZhiXiaXianZhi, 5);
			break;//LCD��ʾ"��������ֵ"
		case(3):
				LCD_WriteEnglishString( 4, 0, (uint8_t *)"               ");
				LCD_WriteEnglishString( 6, 0, (uint8_t *)"               ");
				LCD_WriteChineseString( 6, 24, (uint8_t *)sheZhi, 2);
				LCD_WriteChineseString( 6, 24+32, (uint8_t *)RTC_Nian, 1);
			break;//LCD��ʾ"������"
		case(4):
				LCD_WriteEnglishString( 4, 0, (uint8_t *)"               ");
				LCD_WriteEnglishString( 6, 0, (uint8_t *)"               ");
				LCD_WriteChineseString( 6, 24, (uint8_t *)sheZhi, 2);
				LCD_WriteChineseString( 6, 24+32, (uint8_t *)RTC_Yue, 1);
			break;//LCD��ʾ"������"
		case(5):
				LCD_WriteEnglishString( 4, 0, (uint8_t *)"               ");
				LCD_WriteEnglishString( 6, 0, (uint8_t *)"               ");
				LCD_WriteChineseString( 6, 24, (uint8_t *)sheZhi, 2);
				LCD_WriteChineseString( 6, 24+32, (uint8_t *)RTC_Ri, 1);
			break;//LCD��ʾ"������"
		case(6):
				LCD_WriteEnglishString( 4, 0, (uint8_t *)"               ");
				LCD_WriteEnglishString( 6, 0, (uint8_t *)"               ");
				LCD_WriteChineseString( 6, 24, (uint8_t *)sheZhi, 2);
				LCD_WriteChineseString( 6, 24+32, (uint8_t *)RTC_Shi, 1);
			break;//LCD��ʾ"����ʱ"
		case(7):
				LCD_WriteEnglishString( 4, 0, (uint8_t *)"               ");
				LCD_WriteEnglishString( 6, 0, (uint8_t *)"               ");
				LCD_WriteChineseString( 6, 24, (uint8_t *)sheZhi, 2);
				LCD_WriteChineseString( 6, 24+32, (uint8_t *)RTC_Fen, 1);
			break;//LCD��ʾ"���÷�"
		case(8):
				LCD_WriteEnglishString( 4, 0, (uint8_t *)"               ");
				LCD_WriteEnglishString( 6, 0, (uint8_t *)"               ");
				LCD_WriteChineseString( 6, 24, (uint8_t *)sheZhi, 2);
				LCD_WriteChineseString( 6, 24+32, (uint8_t *)RTC_Miao, 1);
			break;//LCD��ʾ"������"
		default:
				LCD_Clr();//����
			break;//Ĭ��״̬
	}
	LCD_WriteEnglish(0,64,((uint8_t)fabs((float)upTempReg))/10+'0');//��ʾ����ֵ��ֵ
	LCD_WriteEnglish(0,72,((uint8_t)fabs((float)upTempReg))%10+'0');
	if(upTempReg >= 0)
	{
		LCD_WriteEnglish(0,56,' ');
	}
	else
	{
		LCD_WriteEnglish(0,56,'-');
	}
	LCD_WriteEnglish(2,64,((uint8_t)fabs((float)downTempReg))/10+'0');//��ʾ����ֵ��ֵ
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
*������void LED_TubeMessage(uint8_t workTemp, uint8_t *dataBuf)
*���ܣ�LED�������Ϣ��ʾ����LED�������Ϣ��ʾ��������ϵͳ����״̬��
*		������ܵ���ʾ���ݣ�
*���룺uint8_t workTemp-ϵͳ����״̬�� 
*			uint8_t *dataBuf-RTC���ݣ�������ʱ����
*�������
*����˵������
*******************************************************************/
void LED_TubeMessage(uint8_t workTemp, uint8_t *dataBuf)
{
	switch(work)
	{
		case(0):
		case(1):
		case(2):
				if(Time_CheckValidity((uint8_t *)dataBuf) != 0)//�ж�RTCʱ�Ӹ�ʽ�Ƿ�����
				{//RTC��ʽ��ȷ
					buf_led[0]=(dataBuf[2]>>4)&0x0F;//��
					buf_led[1]=dataBuf[2]&0x0F;
					buf_led[2]=(dataBuf[3]>>4)&0x0F;//��
					buf_led[3]=dataBuf[3]&0x0F;
					buf_led[4]=(dataBuf[4]>>4)&0x0F;//ʱ
					buf_led[5]=dataBuf[4]&0x0F;
					buf_led[6]=(dataBuf[5]>>4)&0x0F;//��
					buf_led[7]=dataBuf[5]&0x0F;
					if(secondTemp != dataBuf[6])//��
					{
						secondTemp = dataBuf[6];
						buf_led[5]=((~buf_led[5])&0x80)|buf_led[5];
					}
					set_led_dispbuf(buf_led,0,8);
				}
				else
				{//RTC��ʽ����ȷ
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
		case(3)://�������
				buf_led[0]=(RTC_Buf[0]/10)&0x0F;
				buf_led[1]=(RTC_Buf[0]%10)&0x0F;;
				buf_led[2]=(RTC_Buf[1]/10)&0x0F;
				buf_led[3]=(RTC_Buf[1]%10)&0x0F;;
				buf_led[4]=(RTC_Buf[2]/10)&0x0F;
				buf_led[5]=(RTC_Buf[2]%10)&0x0F;;
				buf_led[6]=(RTC_Buf[3]/10)&0x0F;
				buf_led[7]=(RTC_Buf[3]%10)&0x0F;;
				if(secondTemp != dataBuf[6])//������λ��˸
				{
					secondTemp = dataBuf[6];
					buf_led[2]=29;/*�ո� 29*/
					buf_led[3]=29;/*�ո� 29*/
				}
				set_led_dispbuf(buf_led,0,8);
			break;
		case(4)://������
				buf_led[0]=(RTC_Buf[0]/10)&0x0F;
				buf_led[1]=(RTC_Buf[0]%10)&0x0F;;
				buf_led[2]=(RTC_Buf[1]/10)&0x0F;
				buf_led[3]=(RTC_Buf[1]%10)&0x0F;;
				buf_led[4]=(RTC_Buf[2]/10)&0x0F;
				buf_led[5]=(RTC_Buf[2]%10)&0x0F;;
				buf_led[6]=(RTC_Buf[3]/10)&0x0F;
				buf_led[7]=(RTC_Buf[3]%10)&0x0F;;
				if(secondTemp != dataBuf[6])
				{//������λ��˸
					secondTemp = dataBuf[6];
					buf_led[4]=29;/*�ո� 29*/
					buf_led[5]=29;/*�ո� 29*/
				}
				set_led_dispbuf(buf_led,0,8);
			break;
		case(5)://������
				buf_led[0]=(RTC_Buf[0]/10)&0x0F;
				buf_led[1]=(RTC_Buf[0]%10)&0x0F;;
				buf_led[2]=(RTC_Buf[1]/10)&0x0F;
				buf_led[3]=(RTC_Buf[1]%10)&0x0F;;
				buf_led[4]=(RTC_Buf[2]/10)&0x0F;
				buf_led[5]=(RTC_Buf[2]%10)&0x0F;;
				buf_led[6]=(RTC_Buf[3]/10)&0x0F;
				buf_led[7]=(RTC_Buf[3]%10)&0x0F;;
				if(secondTemp != dataBuf[6])
				{//������λ��˸
					secondTemp = dataBuf[6];
					buf_led[6]=29;/*�ո� 29*/
					buf_led[7]=29;/*�ո� 29*/
				}
				set_led_dispbuf(buf_led,0,8);
			break;
		case(6)://����ʱ
				buf_led[0]=29;/*�ո� 29*/
				buf_led[1]=(RTC_Buf[4]/10)&0x0F;
				buf_led[2]=(RTC_Buf[4]%10)&0x0F;;
				buf_led[3]=(RTC_Buf[5]/10)&0x0F;
				buf_led[4]=(RTC_Buf[5]%10)&0x0F;;
				buf_led[5]=(RTC_Buf[6]/10)&0x0F;
				buf_led[6]=(RTC_Buf[6]%10)&0x0F;;
				buf_led[7]=29;/*�ո� 29*/
				if(secondTemp != dataBuf[6])
				{//������λ��˸
					secondTemp = dataBuf[6];
					buf_led[1]=29;/*�ո� 29*/
					buf_led[2]=29;/*�ո� 29*/
				}
				set_led_dispbuf(buf_led,0,8);
			break;
		case(7)://���÷�
				buf_led[0]=29;/*�ո� 29*/
				buf_led[1]=(RTC_Buf[4]/10)&0x0F;
				buf_led[2]=(RTC_Buf[4]%10)&0x0F;;
				buf_led[3]=(RTC_Buf[5]/10)&0x0F;
				buf_led[4]=(RTC_Buf[5]%10)&0x0F;;
				buf_led[5]=(RTC_Buf[6]/10)&0x0F;
				buf_led[6]=(RTC_Buf[6]%10)&0x0F;;
				buf_led[7]=29;/*�ո� 29*/
				if(secondTemp != dataBuf[6])
				{//������λ��˸
					secondTemp = dataBuf[6];
					buf_led[3]=29;/*�ո� 29*/
					buf_led[4]=29;/*�ո� 29*/
				}
				set_led_dispbuf(buf_led,0,8);
			break;
		case(8)://������
				buf_led[0]=29;/*�ո� 29*/
				buf_led[1]=(RTC_Buf[4]/10)&0x0F;
				buf_led[2]=(RTC_Buf[4]%10)&0x0F;;
				buf_led[3]=(RTC_Buf[5]/10)&0x0F;
				buf_led[4]=(RTC_Buf[5]%10)&0x0F;;
				buf_led[5]=(RTC_Buf[6]/10)&0x0F;
				buf_led[6]=(RTC_Buf[6]%10)&0x0F;;
				buf_led[7]=29;/*�ո� 29*/
				if(secondTemp != dataBuf[6])
				{//������λ��˸
					secondTemp = dataBuf[6];
					buf_led[5]=29;/*�ո� 29*/
					buf_led[6]=29;/*�ո� 29*/
				}
				set_led_dispbuf(buf_led,0,8);
			break;
		default: 
				buf_led[0]=14;/*E 14*/
				buf_led[1]=24;/*r 24*/
				buf_led[2]=24;/*r 24*/
				buf_led[3]=21;/*o 21*/
				buf_led[4]=24;/*r 24*/
				buf_led[5]=29;/*�ո� 29*/
				buf_led[6]=29;/*�ո� 29*/
				buf_led[7]=29;/*�ո� 29*/
				set_led_dispbuf(buf_led,0,8);
			break;
	}
}

/*******************************************************************
*������void LCD_DisplayProcess(void)
*���ܣ�LCD��ʾ����
*���룺��
*�������
*����˵������
*******************************************************************/
void LCD_DisplayProcess(void)
{
	if(F_Error != 0)
	{//ϵͳ������
		ERROR_Message(F_Error);
		F_Error = 0;
	}
#ifdef PT100
	WORKSTATION_Message(work, upTemp, downTemp, temperature1, temperature2);
#else
	WORKSTATION_Message(work, upTemp, downTemp, temperature1);
#endif /*PT100*/
	PCF8563_GetTime((uint8_t *)RTC_BCD_Buf);//��ȡRTCʱ��
	LED_TubeMessage(work, RTC_BCD_Buf);
}

/*******************************************************************
*������void LCD_ClrDispAirConRun(void)
*���ܣ����LCD��ʾ�յ�����״̬��ʾ��Ϣ
*���룺��
*�������
*����˵������
*******************************************************************/
void LCD_ClrDispAirConRun(void)
{
	P_LAMP=OFF;//�ص�
	P_FAN=OFF;//����ɢ��
	LCD_WriteEnglishString( 0, 96, (uint8_t *)"    ");
	LCD_WriteEnglishString( 2, 96, (uint8_t *)"    ");
	LCD_WriteEnglishString( 4, 96, (uint8_t *)"    ");
	//LCD_WriteEnglishString( 6, 96, (uint8_t *)"    ");
}

/*******************************************************************
*������void NTC_temperatureProcess(void)
*���ܣ��ɼ��¶Ȳ��ֽ���
*���룺��
*�������
*����˵������
*******************************************************************/
void NTC_temperatureProcess(void)
{
		temperature1 = getTemperature(0);//�ɼ�ADC0ͨ�������������¶�
		if(temperature1 > MAX_TEMP || temperature1 <= MIN_TEMP)	 //PCF8591ͨ���쳣
		{
			;
//			printf("��ȡADC1ʧ�� �� �����¶Ȳ�����Χ��\r\n");
//			printf("Read ADC1 Error !\r\n");
		}
#ifdef PT100
		temperature2 = getPt100Temperature(1);//�ɼ�ADC0ͨ����Pt100�¶�
		if(temperature2 > ERROR_MAX_TEMP2 || temperature2 <= ERROR_MIN_TEMP2)	 //PCF8591ͨ���쳣
		{
			;
//			printf("��ȡADC2ʧ�� �� �����¶Ȳ�����Χ��\r\n");
//			printf("Read ADC2 Error !\r\n");
		}
#endif /*PT100*/
		
/****************************************************************************
*��λ�������������¶Ȳ���
*****************************************************************************/
#ifdef POTENTIOMETER
		POTENTIOMETER_Control(&upTemp, &downTemp, 2, 3);
#endif /*POTENTIOMETER*/
}

/*******************************************************************
*������void Air_conditionProcess(void)
*���ܣ��¶ȿ��ƽ���
*���룺��
*�������
*����˵������
*******************************************************************/
void Air_conditionProcess(void)
{
	//�����ϡ������¶�ֵ���Զ����������¶�
	/*
	P33�����ڿ��Ʒ���ת����ֹͣ,P34�����ڿ��Ƽ��ȵƵĴ򿪻�رա�

	1. 
	������void PCF8563_GetTime(uint8_t *ptr);�����ڻ�ȡRTCʱ�ӡ�
	��ȡ����ʱ����Ϣ��ptr�������ݸ�ʽ���£���ע�⣺���ݸ�ʽ��BCD���ʽ��
	*-|----|-------------|------|------|------|------|------|-
	* |����|ptr[0]~ptr[1]|ptr[2]|ptr[3]|ptr[4]|ptr[5]|ptr[6]|
	*-|----|-------------|------|------|------|------|------|-
	* |����|��         	 |��    | ��   |ʱ    |��    |��    |
	*-|----|-------------|------|------|------|------|------|-
	* |ʾ��|0x2017       |0x12  |0x25  |0x12  |0x34  |0x56  |
	*-|----|-------------|------|------|------|------|------|-
	* |ʱ��|2017��       |12��  |25��  |12ʱ  |34��  |56��  |
	*-|----|-------------|------|------|------|------|------|-

	2. 
	������Convert_BcdToHex(uint8_t *bcd, uint8_t *hex)�����ڽ�����bcd[]�ڵ�BCD��ת���ɳ��õ�ʮ�����ƣ����浽����hex[]�С�
	����bcd[]={0x20,0x17}����������Convert_BcdToHex()�����ת�����Ϊhex[]={0x14,0x11}��

	�ر�˵��������PCF8563_GetTime()��Convert_BcdToHex()�ڡ�hal_pcf8563.c���ж��壬���ļ�Ĭ���Ѿ�����ͷ�ļ���hal_pcf8563.h����

	3. 
	��int8_t temperature1;//����¶�1��ֵ������������ɼ����¶ȣ���λ���桱��temperature1���з����������ݡ�
	��uint8_t RTC_SecondOld;//����RTC���룬��ǰ������.����RTC_SecondOld���޷����������ݡ�
	��uint8_t RTC_Second;	//����RTC���룬���ڵ�����.����RTC_Second���޷����������ݡ�
	��uint8_t RTC_TestBuf[7];//RTCʱ�ӻ��棬����˵���������ʽ--BCD�롱������RTC_TestBuf[]���޷����������ݡ�
	���������Ѿ���main()������ʼ�����壬�������ֱ��ʹ�á���ʦ��Ҳ���Ը����Լ�����Ҫ���������

	*/

	 //ϵͳ���Ƽ��ȵƺͷ���ת�������Ӷ������¿������¶ȡ��ⲿ�ֵĹ��ܴ����ɾ����߲�����д���·��հ�����
		
		/*-----------�հײ�����ʼ��-----------*/
		
		//�ο��������£�
		PCF8563_GetTime((uint8_t *)RTC_TestBuf);
		Convert_BcdToHex(&RTC_TestBuf[6], &RTC_Second); //��ȡRTC������ֵ
		
		if(temperature1 >= MAX_CTRL_TEMP)
		{//����ϵͳ��������������1S���ص�2S��ʾ����
			alarm=1;
//			P_FAN = ON;//�¶ȹ��ߣ�������ɢ��
			//printf("�¶�ʧ�ؾ��� & temperature1 >= MAX_CTRL_TEMP\r\n");
			/*if(P_LAMP_STA != ON)
			{
				if((RTC_Second-RTC_SecondOld) >= 2)//�ص�2��
				{
					P_LAMP = ON;//����
					RTC_SecondOld = RTC_Second;//���浱ǰʱ���Թ��´βο�
					printf("�¶�ʧ�ؾ��棡��\r\n");
				}
			}
			else
			{
				if((RTC_Second-RTC_SecondOld) >= 1)//����1��
				{
					P_LAMP = OFF;//�ص�
					RTC_SecondOld = RTC_Second;//���浱ǰʱ���Թ��´βο�
					HalBuzzerBlink (HAL_BUZZER_0, 2, 50, 200);//����������
					printf("�¶�ʧ�ؾ��棡\r\n");
				}
			}*/
			//LCD_WriteChineseString( 2, 96, (uint8_t *)IND_ShiKong, 2);
			LCD_WriteChineseString( 4, 96, (uint8_t *)IND_JingGao, 2);
			if(temperature1>upTemp)
			{//�¶ȹ��ߣ�������ɢ��
				P_FAN=ON;
			}
			else
			{//�ط���
				P_FAN=OFF;
			}
			/*if(temperature1<downTemp)
			{//�¶ȹ��ͣ����Ƽ���
				P_LAMP=ON;
			}
			else
			{//�ص�
				P_LAMP=OFF;
			}*/
			//��ʾ��ʾ
			if(temperature1<downTemp)
			{//�¶ȹ��ͣ����Ƽ���
				LCD_WriteChineseString( 2, 96, (uint8_t *)IND_JiaRe, 2);
			}
			else if(temperature1>upTemp)
			{//�¶ȹ��ߣ�������ɢ��
				LCD_WriteChineseString( 2, 96, (uint8_t *)IND_ZhiLeng, 2);
			}
			else
			{
				LCD_WriteEnglishString( 2, 96, (uint8_t *)"    ");
			}
		}
		else
		{//��������
			alarm=0;
			RTC_SecondOld = RTC_Second;//���浱ǰʱ���Թ��´βο�
			if(upTemp >= downTemp)
			{
				if(temperature1<downTemp)
				{//�¶ȹ��ͣ����Ƽ���
					P_LAMP=ON;
				}
				else
				{//�ص�
					P_LAMP=OFF;
				}
				if(temperature1>upTemp)
				{//�¶ȹ��ߣ�������ɢ��
					P_FAN=ON;
				}
				else
				{//�ط���
					P_FAN=OFF;
				}
				//��ʾ��ʾ
				if(temperature1<downTemp)
				{//�¶ȹ��ͣ����Ƽ���
					LCD_WriteChineseString( 2, 96, (uint8_t *)IND_JiaRe, 2);
				}
				else if(temperature1>upTemp)
				{//�¶ȹ��ߣ�������ɢ��
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
			{//����ֵ��ͻ�����
				P_FAN=OFF;
				P_LAMP=OFF;
				LCD_WriteChineseString( 0, 96, (uint8_t *)ERR_FaZhi, 2);
				LCD_WriteChineseString( 2, 96, (uint8_t *)ERR_ChongTu, 2);
				LCD_WriteChineseString( 4, 96, (uint8_t *)ERR_TingZhi, 2);
				LCD_WriteChineseString( 6, 96, (uint8_t *)ERR_YunXing, 2);
			}
		}
		
		/*-----------�հײ��ֽ�����-----------*/
}

/*******************************************************************
*������void WorkStaSwitch(uint8_t *workSta, 
						uint8_t *workStaOld, keyType keyIn)
*���ܣ�����״̬�л�
*���룺uint8_t *workSta-��ǰ����״̬,
*		uint8_t *workStaOld-�ϸ��غϵĹ���״̬, 
*		keyType keyIn-��ǰ������ֵ
*�������
*����˵������
*******************************************************************/
void WorkStaSwitch(uint8_t *workSta, uint8_t *workStaOld, 
											keyType keyIn)
{
	if(keyIn==s121)//���ܼ�
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
		{//�������ã�����������
			*workSta=1;
			F_Error = 1; //ϵͳ������,�������¶ȳ�ͻ
		}
		else if(Time_CheckValidity(RTC_BCD_Buf) != 1)// ������ע: ����ֵΪ0��ʾ���Ϸ�, Ϊ1��ʾ�Ϸ�
		{
			*workSta=3;
			F_Error = 2; //ϵͳ������,ʱ���ʽ���Ϲ���
		}
		else
		{//��ȷ���ã�������������״̬
			PCF8563_WirteTime(RTC_BCD_Buf);//д���µ�ʱ��
			printf("������ȷ��������������״̬,��д���µ�RTCʱ��\r\n");
			LCD_Clr();
			*workSta=0;
		}
		
	}
	
	if(*workSta==1)//���������¶�
	{ 
		if(keyIn==s106)//"��"��
		{
			if(upTemp<MAX_TEMP)
			{upTemp++;}
		}
		if(keyIn==s116)//"��"��
		{
			if(upTemp>MIN_TEMP)
			{upTemp--;}
		}
	}
	if(*workSta==2)//���������¶�
	{
		if(keyIn==s106)//"��"��
		{
			if(downTemp<MAX_TEMP)
			{downTemp++;}
		}
		if(keyIn==s116)//"��"��
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
		case(3)://�������
				if(keyIn==s106)//"��"��
				{
					if(RTC_Buf[1]<99)
					{RTC_Buf[1]++;}
				}
				if(keyIn==s116)//"��"��
				{
					if(RTC_Buf[1]>0)
					{RTC_Buf[1]--;}
				}
			break;
		case(4)://������
				if(keyIn==s106)//"��"��
				{
					if(RTC_Buf[2]<12)
					{RTC_Buf[2]++;}
				}
				if(keyIn==s116)//"��"��
				{
					if(RTC_Buf[2]>1)
					{RTC_Buf[2]--;}
				}
			break;
		case(5)://������
				if(keyIn==s106)//"��"��
				{
					if(RTC_Buf[3]<31)
					{RTC_Buf[3]++;}
				}
				if(keyIn==s116)//"��"��
				{
					if(RTC_Buf[3]>1)
					{RTC_Buf[3]--;}
				}
			break;
		case(6)://����ʱ
				if(keyIn==s106)//"��"��
				{
					if(RTC_Buf[4]<23)
					{RTC_Buf[4]++;}
				}
				if(keyIn==s116)//"��"��
				{
					if(RTC_Buf[4]>0)
					{RTC_Buf[4]--;}
				}
			break;
		case(7)://���÷�
				if(keyIn==s106)//"��"��
				{
					if(RTC_Buf[5]<59)
					{RTC_Buf[5]++;}
				}
				if(keyIn==s116)//"��"��
				{
					if(RTC_Buf[5]>0)
					{RTC_Buf[5]--;}
				}
			break;
		case(8)://������
				if(keyIn==s106)//"��"��
				{
					if(RTC_Buf[6]<59)
					{RTC_Buf[6]++;}
				}
				if(keyIn==s116)//"��"��
				{
					if(RTC_Buf[6]>0)
					{RTC_Buf[6]--;}
				}
			break;
		default: break;
	}
}

/*******************************************************************
*������void KEY_HandleProcess(void)
*���ܣ������¼��������
*���룺��
*�������
*����˵������
*******************************************************************/
void KEY_HandleProcess(void)
{
	keyType keyValueTemp=noneKey;//������ֵ�Ĵ���
	
	keyValueTemp=KEY_ScanTriggerOnce();//����ɨ��
	WorkStaSwitch(&work, &oldWork, keyValueTemp);
}

/*******************************************************************
*������void IR_HandleProcess(uint8_t irData)
*���ܣ�����ң�ذ�����ֵ�������
*���룺uint8_t irData�������ֵ
*�������
*����˵������
*******************************************************************/
void IR_HandleProcess(uint8_t irData)
{
//ң������һ�� CHANNEL
#define PWR_ON_OFF 162//�������ػ�
#define MENU 226//�������ü�
//ң�����ڶ���
#define PREV 224//��������ֵ
#define NEXT 144//��������ֵ
#define PLAY_PAUSE 168//���ü�
//ң����������
#define VOL_ADD 2//����ֵ
#define VOL_SUB 152//����ֵ
#define E_QUIT 194//�˳�

	switch(irData)
	{
		case(PWR_ON_OFF):
			systemPowerOnOff = !systemPowerOnOff;
			if(systemPowerOnOff != 1)
			{
				LCD_Clr();
				LCD12864_BL  = 1;    //�رձ���
				memset(buf_led,MAX_LEDCH,sizeof(buf_led));//��������ܳ�ʼ��ʾ����
				set_led_dispbuf(buf_led,0,8); //�������ʾ��ʼ������Ϊȫ��
				P_FAN = OFF;//���Ʒ���
				P_LAMP = OFF;//���Ƽ��ȵ�
				delay_ms(40);//�ȴ������ˢ�����
#ifdef USE_IWDG	
				IWDG_ReloadCounter();//���ؿ��Ź���ʱ��
#endif /*USE_IWDG*/
			}
			else
			{
				NVIC_SystemReset();//����ϵͳ
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
*������int8_t SE_SendSensor3(char *ApiTag0, uint32_t Value0, 
							char *ApiTag1, uint32_t Value1, 
							char *ApiTag2, uint32_t Value2,char *TimeStr)
*���ܣ����ʹ������ݵ�������
*���룺
	char *ApiTag0 ʵ������ǩ����
	uint32_t sensor0, ������ֵ
	char *ApiTag1 ʵ������ǩ����
	uint32_t sensor1, ������ֵ
	char *ApiTag2 ʵ������ǩ����
	uint32_t sensor2, ������ֵ
	char *TimeStr���ɼ�����ʱ��ʱ�䣬����yyyy-mm-dd hh:mm:ss��ʽ
*�����
		return = 0 ,sucess
		return < 0 ,error
*����˵����
*******************************************************************/
 int8_t SE_SendSensor3(char *ApiTag0, uint32_t Value0, 
						char *ApiTag1, uint32_t Value1, 
						char *ApiTag2, uint32_t Value2,char *TimeStr)
{
	uint8_t TryGo = 0;
	int8_t error = 0;
	uint8_t TrySend = 0;
	uint8_t TxetBuf[MAX_AT_TX_LEN];
	memset(TxetBuf,0x00,MAX_AT_TX_LEN);//��ջ���
	sprintf((char *)TxetBuf,"{\"t\":3,\"datatype\":2,\"datas\":{\"%s\":{\"%s\":%d},\"%s\":{\"%s\":%d},\"%s\":{\"%s\":%d}},\"msgid\":001}",
																ApiTag0,TimeStr,Value0,
																ApiTag1,TimeStr,Value1,
																ApiTag2,TimeStr,Value2);
se_send_sensor:
	if(ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf)) < 0)
	{//����ʧ��
		error=-1;
	}
	else
	{//���ͳɹ�
		for(TryGo = 0; TryGo<50; TryGo++)//���ȴ�ʱ��50*10=500ms
		{
			if(strstr((const char *)AT_RX_BUF, (const char *)"\"status\":0") == NULL)//�����Ӧ״̬�Ƿ�Ϊ"�ϱ��ɹ�"
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
		printf("�ط���������%d��\r\n",TrySend+1);
		TrySend++;
		goto se_send_sensor;
	}
	return error;
}

/*******************************************************************
*������int8_t SE_SendSensor2(char *ApiTag0, uint32_t Value0, 
							char *ApiTag1, uint32_t Value1,char *TimeStr)
*���ܣ����ʹ������ݵ�������
*���룺
	char *ApiTag0 ʵ������ǩ����
	uint32_t sensor0, ������ֵ
	char *ApiTag1 ʵ������ǩ����
	uint32_t sensor1, ������ֵ
	char *TimeStr���ɼ�����ʱ��ʱ�䣬����yyyy-mm-dd hh:mm:ss��ʽ
*�����
		return = 0 ,sucess
		return < 0 ,error
*����˵����
*******************************************************************/
int8_t SE_SendSensor2(char *ApiTag0, uint32_t Value0, 
						char *ApiTag1, uint32_t Value1,char *TimeStr)
{
	uint8_t TryGo = 0;
	int8_t error = 0;
	uint8_t TrySend = 0;
	uint8_t TxetBuf[MAX_AT_TX_LEN];
	memset(TxetBuf,0x00,MAX_AT_TX_LEN);//��ջ���
	sprintf((char *)TxetBuf,"{\"t\":3,\"datatype\":2,\"datas\":{\"%s\":{\"%s\":%d},\"%s\":{\"%s\":%d}},\"msgid\":001}",
																ApiTag0,TimeStr,Value0,
																ApiTag1,TimeStr,Value1);
se_send_sensor:
	if(ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf)) < 0)
	{//����ʧ��
		error=-1;
	}
	else
	{//���ͳɹ�
		for(TryGo = 0; TryGo<50; TryGo++)//���ȴ�ʱ��50*10=500ms
		{
			if(strstr((const char *)AT_RX_BUF, (const char *)"\"status\":0") == NULL)//�����Ӧ״̬�Ƿ�Ϊ"�ϱ��ɹ�"
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
		printf("�ط���������%d��\r\n",TrySend+1);
		TrySend++;
		goto se_send_sensor;
	}
	return error;
}

/*******************************************************************
*������char *USER_GetJsonValue(char *cJson, char *Tag)
*���ܣ�jsonΪ�ַ������У���json��ʽ�е�Ŀ�����Tag��Ӧ��ֵ�ַ�����ȡ����
*���룺
		char *cJson json�ַ���
		char *Tag Ҫ�����Ķ����ǩ
*�����������ֵ���ַ�����ʽ����ʼ��ַ
*����˵�����û������ڴ˻����ϸ������չ�ú���������ֻ�Ǹ��򵥵�DEMO
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
		//printf("���ַ���\r\n");
		return NULL;
	}
	i=strlen((const char *)temp);
	target=target+i;
	memset(temp, 0x00, 128);
	for(i=0; i<10; i++, target++)//��ֵ����10��λΪ�Ƿ�������2^32=4294967296
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
	//printf("��ֵ=%s\r\n",temp);
	return (char *)temp;
}

/*******************************************************************
*������char *USER_GetTempJsonValue(char *cJson, char *Tag)
*���ܣ�jsonΪ�ַ������У���json��ʽ�е�Ŀ�����Tag��Ӧ��ֵ�ַ���ת��Ϊ��ֵ
*���룺
		char *cJson json�ַ���
		char *Tag Ҫ�����Ķ����ǩ
*�����������ֵ
*����˵�����û������ڴ˻����ϸ������չ�ú���������ֻ�Ǹ��򵥵�DEMO
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
		//printf("���ַ���\r\n");
		return NULL;
	}
	i=strlen((const char *)temp);
	target=target+i;
	memset(temp, 0x00, 128);
	for(i=0; i<10; i++, target++)//��ֵ����10��λΪ�Ƿ�������2^32=4294967296
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
	//printf("��ֵstr=%s\r\n",temp);
	len=strlen((const char *)temp);
	switch(len)
	{
		case(1):value=temp[0]-'0';break;
		case(2):value=temp[1]-'0'+(temp[0]-'0')*10;break;
		case(3):value=temp[2]-'0'+(temp[0]-'0')*100+(temp[1]-'0')*10;break;
		default:break;
	}
	//printf("��ֵvalue=%d\r\n",value);
	return value;
}

/*******************************************************************
*������void USER_DataAnalysisProcess(char *RxBuf)
*���ܣ���������������
*���룺char *RxBuf �������·�����
*�����
*����˵�����û������ڴ˻����ϸ������չ�ú���������ֻ�Ǹ��򵥵�DEMO
*******************************************************************/
void USER_DataAnalysisProcess(char *RxBuf)
{
	char *cmdid = NULL;
	uint8_t TxetBuf[128];
	uint8_t angle=0;//�̶�
	if(strstr((const char *)RxBuf, (const char *)PING_REQ) != NULL)//��������
	{
		if(ESP8266_IpSend((char *)PING_RSP, strlen((const char *)PING_RSP)) < 0)//��Ӧ����
		{//����ʧ��
			printf("����������ʧ�ܣ�\r\n");
		}
		else
		{
			printf("��������\r\n");
		}
	}
	else if(strstr((const char *)RxBuf, (const char *)"\"t\":5") != NULL)//��������
	{
		if(strstr((const char *)RxBuf, (const char *)"\"apitag\":\"powerCtrl\"") != NULL)//���ƿ��ػ�
		{
			memset(TxetBuf,0x00,128);//��ջ���
			if((strstr((const char *)RxBuf, (const char *)"\"data\":1") != NULL))//����
			{
				printf("������\r\n");
				;//...
				;//...
				cmdid = USER_GetJsonValue((char *)RxBuf, (char *)"cmdid");
				sprintf((char *)TxetBuf,"{\"t\":6,\"cmdid\":%s,\"status\":0,\"data\":1}",cmdid);
				//printf("%s\r\n",TxetBuf);////////////////////////////////////////////////////////////
				if(ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf)) < 0)
				{//����ʧ��
					printf("������Ӧʧ�ܣ�\r\n");
				}
				/*******************************************���Ϳ��ػ���Ϣ**********************************************/
				sprintf(RTC_TimeTemp,"%d%d%d%d-%d%d-%d%d ",
							RTC_BCD_Buf[0]>>4,RTC_BCD_Buf[0]&0x0F,RTC_BCD_Buf[1]>>4,RTC_BCD_Buf[1]&0x0F,
							RTC_BCD_Buf[2]>>4,RTC_BCD_Buf[2]&0x0F,RTC_BCD_Buf[3]>>4,RTC_BCD_Buf[3]&0x0F);
				sprintf(&RTC_TimeTemp[11],"%d%d:%d%d:%d%d",
							RTC_BCD_Buf[4]>>4,RTC_BCD_Buf[4]&0x0F,RTC_BCD_Buf[5]>>4,RTC_BCD_Buf[5]&0x0F,
							RTC_BCD_Buf[6]>>4,RTC_BCD_Buf[6]&0x0F);
				if(SE_SendSensor2((char *)"alarm", alarm, 
							 (char *)"power", 1/*systemPowerOnOff*/,(char *)RTC_TimeTemp))
				{//����ʧ��
					printf("���Ϳ�/�ػ�״̬ʧ�ܣ�\r\n");
				}
				/*******************************************���ػ���ʾ����********************************************/
				if(systemPowerOnOff != 1)
				{//��֮ǰ��״̬�ǹػ�����ִ������Ŀ���ָ��
					PowerOffDisplay(1);
				}
				systemPowerOnOff=1;
			}
			else if((strstr((const char *)RxBuf, (const char *)"\"data\":0") != NULL))//�ػ�
			{
				printf("�ػ���\r\n");
				P_FAN = OFF;//���Ʒ���
				P_LAMP = OFF;//���Ƽ��ȵ�
				systemPowerOnOff=0;
				PowerOffDisplay(systemPowerOnOff);
				;//...
				;//...
				cmdid = USER_GetJsonValue((char *)RxBuf, (char *)"cmdid");
				sprintf((char *)TxetBuf,"{\"t\":6,\"cmdid\":%s,\"status\":0,\"data\":0}",cmdid);
				//printf("%s\r\n",TxetBuf);////////////////////////////////////////////////////////////
				if(ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf)) < 0)
				{//����ʧ��
					printf("������Ӧʧ�ܣ�\r\n");
				}
				/*******************************************���Ϳ��ػ���Ϣ**********************************************/
				sprintf(RTC_TimeTemp,"%d%d%d%d-%d%d-%d%d ",
							RTC_BCD_Buf[0]>>4,RTC_BCD_Buf[0]&0x0F,RTC_BCD_Buf[1]>>4,RTC_BCD_Buf[1]&0x0F,
							RTC_BCD_Buf[2]>>4,RTC_BCD_Buf[2]&0x0F,RTC_BCD_Buf[3]>>4,RTC_BCD_Buf[3]&0x0F);
				sprintf(&RTC_TimeTemp[11],"%d%d:%d%d:%d%d",
							RTC_BCD_Buf[4]>>4,RTC_BCD_Buf[4]&0x0F,RTC_BCD_Buf[5]>>4,RTC_BCD_Buf[5]&0x0F,
							RTC_BCD_Buf[6]>>4,RTC_BCD_Buf[6]&0x0F);
				if(SE_SendSensor2((char *)"alarm", alarm, 
							 (char *)"power", systemPowerOnOff,(char *)RTC_TimeTemp))
				{//����ʧ��
					printf("���Ϳ�/�ػ�״̬ʧ�ܣ�\r\n");
				}
			}
		}
#ifndef POTENTIOMETER	
		else if(strstr((const char *)RxBuf, (const char *)"\"apitag\":\"upperLimitCtrl\"") != NULL)//��������ֵ
		{
			memset(TxetBuf,0x00,128);//��ջ���
			angle=USER_GetTempJsonValue((char *)RxBuf, (char *)"data");
			upTemp=angle;
			printf("��������ֵ��%d����\r\n",angle);
		}
		else if(strstr((const char *)RxBuf, (const char *)"\"apitag\":\"lowerLimitCtrl\"") != NULL)//��������ֵ
		{
			memset(TxetBuf,0x00,128);//��ջ���
			angle=USER_GetTempJsonValue((char *)RxBuf, (char *)"data");
			downTemp=angle;
			printf("��������ֵ��%d����\r\n",angle);
		}
#endif /*POTENTIOMETER*/
	}
}

/*******************************************************************
*������void PowerOffDisplay(uint8_t power)
*���ܣ���Դ���ػ���ʾ���洦��
*���룺uint8_t power�� 0�ǹػ���1�ǿ���
*�������
*����˵������
*******************************************************************/
void PowerOffDisplay(uint8_t power)
{
	if(power != 1)
	{
		LCD_Clr();
		LCD12864_BL  = 1;    //�رձ���
		memset(buf_led,MAX_LEDCH,sizeof(buf_led));//��������ܳ�ʼ��ʾ����
		set_led_dispbuf(buf_led,0,8); //�������ʾ��ʼ������Ϊȫ��
		delay_ms(40);//�ȴ������ˢ�����
#ifdef USE_IWDG	
		IWDG_ReloadCounter();//���ؿ��Ź���ʱ��
#endif /*USE_IWDG*/
	}
	else
	{
		NVIC_SystemReset();//����ϵͳ
	}
}

/*******************************************************************
*������void AlarmProcess(uint8_t alarm)
*���ܣ��澯���̴���
*���룺uint8_t alarm��ȡֵ0��1,1��ʱ��ִ�и澯��0��ʱ��ִ��
*�������
*����˵������
*******************************************************************/
void AlarmProcess(uint8_t alarm)
{
	if(alarm != 0)
	{
		if(P_LAMP_STA != ON)
		{
			if((uint16_t)(T_ALARM_MS - T_ALARM_MS_OLD) > (2000/3))//�ص�2��
			{
				T_ALARM_MS_OLD = (uint16_t)T_ALARM_MS;
				P_LAMP = ON;//����
				printf("�¶�ʧ�ؾ��棡��\r\n");
			}
		}
		else
		{
			if((uint16_t)(T_ALARM_MS - T_ALARM_MS_OLD) > (1000/3))//����1��
			{
				T_ALARM_MS_OLD = (uint16_t)T_ALARM_MS;
				P_LAMP = OFF;//�ص�
				HalBuzzerBlink (HAL_BUZZER_0, 2, 50, 200);//����������
				printf("�¶�ʧ�ؾ��棡\r\n");
			}
		}
	}
	else
	{
		T_ALARM_MS_OLD = T_ALARM_MS = 0;
	}
}

