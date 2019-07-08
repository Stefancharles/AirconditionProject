/******************************************************************************
*
*
*
*
*
*
*
*
*
*
******************************************************************************/

#ifndef _WiFiToCloud_h_
#define _WiFiToCloud_h_

#include <stdint.h>
#include "HAL_WiFiToCloud.h"
#include "WiFiToCloud.h"
#include "CloudReference.h"



//�궨��
#define AT_CWMODE	"AT+CWMODE_CUR=1" //����Ϊ��station��ģʽ

#define PING_REQ "$#AT#\r"//��������
#define PING_RSP "$OK##\r"//������Ӧ

/*******************************************************************
*������int8_t ESP8266_SetStation(void)
*���ܣ�ESP8266����Ϊstationģʽ
*���룺��
*�����
		return = 0 ,sucess
		return < 0 ,error
*����˵����
*******************************************************************/
int8_t ESP8266_SetStation(void);

/*******************************************************************
*������int8_t ESP8266_SetAP(void)
*���ܣ�����ESP8266Ҫ���ӵ��ȵ����ƺ�����
*���룺char *wifi-�ȵ����� char *pwd-�ȵ�����
*�����
		return = 0 ,sucess
		return < 0 ,error
*����˵����
*******************************************************************/
int8_t ESP8266_SetAP(char *wifi, char *pwd);

/*******************************************************************
*������int8_t ESP8266_SetStation(void)
*���ܣ�ESP8266����TCP����
*���룺
	char *IpAddr-IP��ַ�����磺120.77.58.34
	uint16_t port-�˿ںţ�ȡֵ0~65535
*�����
		return = 0 ,sucess
		return < 0 ,error
*����˵����
*******************************************************************/
int8_t ESP8266_IpStart(char *IpAddr, uint16_t port);

/*******************************************************************
*������int8_t ESP8266_IpSend(char *IpBuf, uint8_t len)
*���ܣ�ESP8266��������
*���룺
		char *IpBuf-IP����
		uint8_t len-���ݳ���
*�����
		return = 0 ,sucess
		return < 0 ,error
*����˵����
*******************************************************************/
int8_t ESP8266_IpSend(char *IpBuf, uint8_t len);

/*******************************************************************
*������int8_t ConnectToServer(void)
*���ܣ����ӵ�������
*���룺��
*�����
		return = 0 ,sucess
		return < 0 ,error
*����˵����
*******************************************************************/
int8_t ConnectToServer(char *DeviceID, char *SecretKey);

/*******************************************************************
*������int8_t ESP8266_SendSensor(char *ApiTag, 
									uint32_t sensor, char *TimeStr)
*���ܣ����ʹ������ݵ�������
*���룺
	char *ApiTag ʵ������ǩ����
	uint32_t sensor, ������ֵ
	char *TimeStr���ɼ�����ʱ��ʱ�䣬����yyyy-mm-dd hh:mm:ss��ʽ
*�����
		return = 0 ,sucess
		return < 0 ,error
*����˵����
*******************************************************************/
int8_t ESP8266_SendSensor(char *ApiTag, uint32_t sensor, char *TimeStr);

/*******************************************************************
*������int8_t ESP8266_SendSensorTry3rd(char *SensorTag, char *TimeStr, uint8_t sensor)
*���ܣ����ʹ������ݵ�������
*���룺
	char *SensorTag, �����ǩ����
	uint8_t sensor, ������ֵ
	char *TimeStr,�ɼ�����ʱ��ʱ�䣬����yyyy-mm-dd hh:mm:ss��ʽ
*�����
		return = 0 ,sucess
		return < 0 ,error
*����˵��������ʧ�ܻ��ط����ط��������
*******************************************************************/
int8_t ESP8266_SendSensorTry3rd(char *SensorTag, uint8_t sensor, char *TimeStr);

/*******************************************************************
*������uint8_t ESP8266_GetIpData(uint8_t *AtRxBuf, char *GetIpData)
*���ܣ���������������
*���룺
		uint8_t *AtRxBuf ��ԭʼAT���ڻ���
		char *GetIpData ����ȡ������Itcp/ip����
*����������յ���IP���ݳ���
*����˵����
	AT+CIPSEND=76
	{"t":3,"datatype":2,"datas":{"alarm":{"2018-06-19 18:15:02":0}},"msgid":001}
	+IPD,29:{"msgid":1,"status":0,"t":4}
*******************************************************************/
uint8_t ESP8266_GetIpData(uint8_t *AtRxBuf, char *GetIpData);
	
/*******************************************************************
*������void ESP8266_DataAnalysisProcess(char *RxBuf)
*���ܣ���������������
*���룺char *RxBuf �������·�����
*�����
*����˵����
*******************************************************************/
void ESP8266_DataAnalysisProcess(char *RxBuf);



#endif //_WiFiToCloud_h_
