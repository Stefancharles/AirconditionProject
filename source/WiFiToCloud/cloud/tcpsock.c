/**********************************************************
	Modify History: qiuzhb make, 2016-8-21
**********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#include "delay.h"
#include "hal_uart4.h"
#include "tcpsock.h"


#define MAX_WAIT_TIME 12 //MS
#define MAX_AT_RX_LEN 255
#define MAX_AT_TX_LEN 128
#define AT_RX_BUF UART4_RX_BUF

/*******************************************************************
*函数：void ClrAtRxBuf(void)
*功能：清空AT串口接收缓存相关寄存器
*输入：无
*输出：无
*特殊说明：无
*******************************************************************/
void ClrAtRxBuf(void)
{
	memset(AT_RX_BUF, 0x00, MAX_AT_RX_LEN);//清空缓存
	UART4_RX_COUNT = 0;
	F_UART4_RX_FINISH = 0;
}
/*******************************************************************
*函数：void SendAtCmd(uint8_t *string)
*功能：发送AT指令字符串
*输入：
		uint8_t *string 待发送的AT指令
		uint8_t len 长度
*输出：无
*特殊说明：无
*******************************************************************/
void SendAtCmd(uint8_t *string, uint8_t len)
{
	UART4_SendStrlen(string, len);
	UART4_SendStrlen((uint8_t *)"\r\n", 2);
}

/*******************************************************************
*函数：void SendStrLen(uint8_t *string, uint8_t len)
*功能：发送字符串到AT指令串口
*输入：
		uint8_t *string 待发送的字符串
		uint8_t len 长度

*输出：无
*特殊说明：无
*******************************************************************/
void SendStrLen(uint8_t *string, uint8_t len)
{
	UART4_SendStrlen(string, len);
}


/* create a tcp socket with server mode
   client_port(opt)
   return > 0(socket fd), sucess
   return -1, error */
int open_client_port(unsigned short client_port)
{
	return -1;
}
/*******************************************************************
*函数：int8_t ESP8266_SetStation(void)
*功能：ESP8266设置为station模式
*输入：无
*输出：
		return = 0 ,sucess
		return < 0 ,error
*特殊说明：
*******************************************************************/
int8_t ESP8266_SetStation(void)
{
	ClrAtRxBuf();//清空缓存
	SendAtCmd((uint8_t *)AT_CWMODE,strlen(AT_CWMODE));
	delay_ms(100);
	if(strstr((const char *)AT_RX_BUF, (const char *)"OK") == NULL)
	{
		return -1;
	}
	return 0;
}
/*******************************************************************
*函数：int8_t ESP8266_SetAP(void)
*功能：设置ESP8266要连接的热点名称和密码
*输入：char *wifi-热点名称 char *pwd-热点密码
*输出：
		return = 0 ,sucess
		return < 0 ,error
*特殊说明：
*******************************************************************/
int8_t ESP8266_SetAP(char *wifi, char *pwd)
{
	uint8_t AtCwjap[MAX_AT_TX_LEN];
	memset(AtCwjap, 0x00, MAX_AT_TX_LEN);//清空缓存
	ClrAtRxBuf();//清空缓存
	sprintf((char *)AtCwjap,"AT+CWJAP_CUR=\"%s\",\"%s\"",wifi, pwd);
	printf("%s\r\n",AtCwjap);////////////////////////////////////////////////////////////
	SendAtCmd((uint8_t *)AtCwjap,strlen((const char *)AtCwjap));
	delay_ms(5500);
	if(strstr((const char *)AT_RX_BUF, (const char *)"OK") == NULL)
	{
		return -1;
	}
	return 0;
}

/*******************************************************************
*函数：int8_t ESP8266_SetStation(void)
*功能：ESP8266建立TCP连接
*输入：
	char *IpAddr-IP地址，例如：120.77.58.34
	uint16_t port-端口号，取值0~65535
*输出：
		return = 0 ,sucess
		return < 0 ,error
*特殊说明：
*******************************************************************/
int8_t ESP8266_IpStart(char *IpAddr, uint16_t port)
{
	uint8_t IpStart[MAX_AT_TX_LEN];
	memset(IpStart, 0x00, MAX_AT_TX_LEN);//清空缓存
	ClrAtRxBuf();//清空缓存
	sprintf((char *)IpStart,"AT+CIPSTART=\"TCP\",\"%s\",%d",IpAddr, port);
	printf("%s\r\n",IpStart);////////////////////////////////////////////////////////////
	SendAtCmd((uint8_t *)IpStart,strlen((const char *)IpStart));
	delay_ms(1500);
	if(strstr((const char *)AT_RX_BUF, (const char *)"OK") == NULL)
	{
		return -1;
	}
	return 0;
}
/*******************************************************************
*函数：int8_t ESP8266_IpSend(char *IpBuf, uint8_t len)
*功能：ESP8266发送数据
*输入：
		char *IpBuf-IP数据
		uint8_t len-数据长度
*输出：
		return = 0 ,sucess
		return < 0 ,error
*特殊说明：
*******************************************************************/
int8_t ESP8266_IpSend(char *IpBuf, uint8_t len)
{
	uint8_t TryGo = 0;
	int8_t error = 0;
	uint8_t IpSend[MAX_AT_TX_LEN];
	memset(IpSend, 0x00, MAX_AT_TX_LEN);//清空缓存
	ClrAtRxBuf();//清空缓存
	sprintf((char *)IpSend,"AT+CIPSEND=%d",len);
	printf("%s\r\n",IpSend);////////////////////////////////////////////////////////////
	SendAtCmd((uint8_t *)IpSend,strlen((const char *)IpSend));
	delay_ms(3);
	if(strstr((const char *)AT_RX_BUF, (const char *)"OK") == NULL)
	{
		return -1;
	}
	ClrAtRxBuf();//清空缓存
	SendStrLen((uint8_t *)IpBuf, len);
	printf("%s\r\n",IpBuf);////////////////////////////////////////////////////////////
	for(TryGo = 0; TryGo<60; TryGo++)//最多等待时间100*60=6000ms
	{
		if(strstr((const char *)AT_RX_BUF, (const char *)"SEND OK") == NULL)
		{
			error = -2;
		}
		else
		{
			error = 0;
			break;
		}
		delay_ms(100);
	}
	return error;
}
/*******************************************************************
*函数：int8_t ConnectToServer(void)
*功能：连接到服务器
*输入：无
*输出：
		return = 0 ,sucess
		return < 0 ,error
*特殊说明：
*******************************************************************/
int8_t ConnectToServer(char *DeviceID, char *SecretKey)
{
	uint8_t TryGo = 0;
	int8_t error = 0;
	uint8_t TxetBuf[MAX_AT_TX_LEN];
	memset(TxetBuf,0x00,MAX_AT_TX_LEN);//清空缓存
	for(TryGo = 0; TryGo<3; TryGo++)
	{
		if(ESP8266_SetStation() == 0)//设置WiFi通讯模块工作模式
		{
			error = 0;
			break;
		}
		else
		{
			error = -1;
		}
	}
	if(error < 0)
	{
		return error;
	}
	for(TryGo = 0; TryGo<3; TryGo++)
	{
		if(ESP8266_SetAP((char *)WIFI_AP, (char *)WIFI_PWD) == 0)//设置热点名称和密码
		{
			error = 0;
			break;
		}
		else
		{
			error = -2;
		}
	}
	if(error < 0)
	{
		return error;
	}
	for(TryGo = 0; TryGo<3; TryGo++)
	{
		if(ESP8266_IpStart((char *)"120.77.58.34",8600) == 0)//连接服务器IP地址，端口：120.77.58.34,8600
		{
			error = 0;
			break;
		}
		else
		{
			error = -3;
		}
	}
	if(error < 0)
	{
		return error;
	}
	
	sprintf((char *)TxetBuf,"{\"t\":1,\"device\":\"%s\",\"key\":\"%s\",\"ver\":\"v0.0.0.0\"}",DeviceID,SecretKey);
	printf("%s\r\n",TxetBuf);////////////////////////////////////////////////////////////
	if(ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf)) < 0)
	{//发送失败
		error=-4;
	}
	else
	{//发送成功
		for(TryGo = 0; TryGo<50; TryGo++)//最多等待时间50*10=500ms
		{
			if(strstr((const char *)AT_RX_BUF, (const char *)"\"status\":0") == NULL)//检查响应状态是否为握手成功
			{
				error = -5;
			}
			else
			{
				error = 0;
				break;
			}
			delay_ms(10);
		}
	}
	
	return error;
}

/*******************************************************************
*函数：int8_t ESP8266_SendSensor(uint8_t sensor)
*功能：发送传感数据到服务器
*输入：
	uint8_t sensor, 传感数值
	char *TimeStr，采集数据时的时间，须是yyyy-mm-dd hh:mm:ss格式
*输出：
		return = 0 ,sucess
		return < 0 ,error
*特殊说明：
*******************************************************************/
int8_t ESP8266_SendSensor(uint8_t sensor, char *TimeStr)
{
	uint8_t TryGo = 0;
	int8_t error = 0;
	uint8_t TxetBuf[MAX_AT_TX_LEN];
	memset(TxetBuf,0x00,MAX_AT_TX_LEN);//清空缓存
	sprintf((char *)TxetBuf,"{\"t\":3,\"datatype\":1,\"datas\":{\"alarm\":{\"%s\":%d}},\"msgid\":001}",TimeStr,sensor);
	printf("%s\r\n",TxetBuf);////////////////////////////////////////////////////////////
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
	return error;
}

/*******************************************************************
*函数：void ESP8266_DataAnalysis(uint8_t sensor, char *TimeStr)
*功能：发送传感数据到服务器
*输入：
	uint8_t sensor, 传感数值
	char *TimeStr，采集数据时的时间，须是yyyy-mm-dd hh:mm:ss格式
*输出：
		return = 0 ,sucess
		return < 0 ,error
*特殊说明：
*******************************************************************/
void ESP8266_DataAnalysisProcess(char *RxBuf)
{
	if(strstr((const char *)RxBuf, (const char *)PING_REQ) != NULL)//心跳请求？
	{
		ESP8266_IpSend((char *)PING_RSP, strlen((const char *)PING_RSP));//响应心跳
	}
	else if(strstr((const char *)RxBuf, (const char *)"\"t\":5") != NULL)//命令请求？
	{
		if(strstr((const char *)RxBuf, (const char *)"\"apitag\":\"ctrl\"") != NULL)//开锁请求
		{
			if((strstr((const char *)RxBuf, (const char *)"\"data\":1") != NULL))//开锁
			{
				ESP8266_IpSend((char *)PING_RSP, strlen((const char *)PING_RSP));//响应心跳
			}
		}
		else if(strstr((const char *)RxBuf, (const char *)"\"apitag\":\"defense\"") != NULL)//布防/撤防请求
		{
			if((strstr((const char *)RxBuf, (const char *)"\"data\":1") != NULL))//布防
			{
				printf("布防！\r\n");
			}
			else if((strstr((const char *)RxBuf, (const char *)"\"data\":0") != NULL))//撤防
			{
				printf("撤防！\r\n");
			}
		}
	}
}

/* set socket non-blocking
   return 0, sucess
   return -1, error */
int set_socket_non_blocking(int sock)
{
	int flags;

	/*if((flags = fcntl(sock, F_GETFL, 0)) == -1){
		TCP_SOCKET_ERR("Get the file access mode and the file status flags error\n");
		return -1;
	}

	if(fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1){
		TCP_SOCKET_ERR("set O_NONBLOCK flags error\n");
		return -1;
	}*/
	return 0;
}

/* connection server
   err_call_bk(opt)
   return < 0, error
   return = 0, sucess*/
int connection_server(int client_sock, char* server_num_and_dot_IPv4, 
	unsigned short server_port)
{
	int ret;
	/*struct sockaddr_in serv_addr;
	in_addr_t addr;

	if(client_sock <= 0){
		TCP_SOCKET_ERR("error client_sock:%d\n", client_sock);
		return -1;
	}
	if(server_port < 0){
		TCP_SOCKET_ERR("error server_port:%d\n", server_port);
		return -1;
	}
	if(server_num_and_dot_IPv4){
		addr = inet_addr(server_num_and_dot_IPv4);
		if(addr == -1){
			TCP_SOCKET_ERR("ip address error:%s\n", server_num_and_dot_IPv4);
			return -1;
		}
	}else{
		TCP_SOCKET_ERR("server_num_and_dot_IPv4 error\n");
		return -1;
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(server_port);
	serv_addr.sin_addr.s_addr = addr;
	if((ret = connect(client_sock, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in))) < 0){
		TCP_SOCKET_ERR("connect error(%d:%s)\n", ret, strerror(errno));
		return ret;
	}*/
	
	return 0;
}		
	  
// do close
int close_connection(int sock)
{
    /*if(close(sock) == -1){
		TCP_SOCKET_ERR("close socket error\n");
		return -1;
    }*/
	return 0;
}

/* return < 0, error
   return > 0,(send data len) sucess*/
int send_packet(int sock, void *buf, int len, int is_no_wait)
{
	int ret;
	/*int flag;
	
	if(sock <= 0){
		TCP_SOCKET_ERR("error sock:%d\n", sock);
		return -1;
	}
	if(buf == NULL){
		TCP_SOCKET_ERR("error buf:%lx\n", (unsigned long)buf);
		return -1;
	}
	if(len < 0){
		TCP_SOCKET_ERR("error len:%d\n", len);
		return -1;
	}

	//set blocking
	if(is_no_wait > 0){
		flag = MSG_DONTWAIT;
	}else{
		flag = 0;
	}

	if((ret = send(sock, buf, len, flag)) < 0){
		TCP_SOCKET_ERR("send packet error(%d:%s)\n", ret, strerror(errno));
		return ret;
	 }*/

	return ret;
}

/* is_no_wait(opt, if > 0 wait until message received)
   err_call_bk(opt)
   return 0, connect has been close by remote side
   return < 0, error
   return > 0,(received data len) sucess*/
int receive_packet(int sock, void *buf, int len, int is_no_wait)
{
	int ret;
	/*int flag;
	
	if(sock <= 0){
		TCP_SOCKET_ERR("error sock:%d\n", sock);
		return -1;
	}
	if(buf == NULL){
		TCP_SOCKET_ERR("error buf:%lx\n", (unsigned long)buf);
		return -1;
	}
	if(len < 0){
		TCP_SOCKET_ERR("error len:%d\n", len);
		return -1;
	}

	//set blocking
	if(is_no_wait > 0){
		flag = MSG_DONTWAIT;
	}else{
		flag = 0;
	}

	if((ret = recv(sock, buf, len, flag)) < 0){
		TCP_SOCKET_ERR("send packet error(%d:%s)\n", ret, strerror(errno));
		return ret;
	}else if(ret == 0){
		TCP_SOCKET_ERR("the connect has been close by remote side.(%d:%s)\n", ret, strerror(errno));
		return 0;
	}else{*/
		return ret;
//	}
}



/* do select 
opt, for select option
return 0, timeout
return < 0, error
return > 0, sucess
*/
int select_socket(int sock, int ms_timeout, SELECT_OPT opt)
{
    /*int ret;
    struct timeval *time_out, timer;
    fd_set rfds, wfds, efds;
	fd_set *p_rfds = NULL, *p_wfds = NULL, *p_efds = NULL;

	if(sock <= 0){
		TCP_SOCKET_ERR("error sock:%d\n", sock);
		return -1;
	}

	if(ms_timeout >= 0){
	    timer.tv_sec = ms_timeout / 1000; 
	    timer.tv_usec = (ms_timeout % 1000) * 1000;
		time_out = & timer;
	}else{
		time_out = NULL;
	}*/

	/**** clear the fdset then set the socket in the fdset for selection ********/
	/*switch(opt){
		case SELECT_READ:
			FD_ZERO(&rfds);
			FD_SET(sock, &rfds);
			p_rfds = &rfds;
			break;
		case SELECT_WRITE:
			FD_ZERO(&wfds);
			FD_SET(sock, &wfds);
			p_wfds = &wfds;
			break;
		case SELECT_EXCEPTION:
			FD_ZERO(&efds);
			FD_SET(sock, &efds);
			p_efds = &efds;
			break;
		case SELECT_FOR_SLEEP:
			//do nothing
			break;
		default:
			return -1;
	}

	//printf("start\n");
    ret = select(sock+1, p_rfds, p_wfds, p_efds, time_out);
	//printf("end\n");
	if(ret < 0){*/
		/**** select error ******/
		/*TCP_SOCKET_ERR("select error(%d:%s)\n", ret, strerror(errno));
		return ret;
	}else if(ret == 0){
		if(opt == SELECT_FOR_SLEEP){
			//for sucess sleep
			return 0;
		}else{*/
			/**** select timeout *********/
			/*TCP_SOCKET_ERR("select timer out!\n");
			return 0;
		}
	}else if(ret == 1){
		return sock;
	}else{
		TCP_SOCKET_ERR("select error , ret = %d\n", ret);
		return -1;
	}*/
	return -1;
}

/*
return < 0, error 
return = 0, sucess
*/
int clear_sock_receive_buffer(int sock)
{
	/*int ret;
	char tmp;
	
	if(sock < 0){
		TCP_SOCKET_ERR("sock error , %d\n", sock);
		return -1;
	}

	while(1){
		ret = select_socket(sock, 0, SELECT_READ);
		if(ret > 0){
			recv(sock, &tmp, 1, 0);
		}else if(ret == 0){
			break;
		}else{
			return -1;
		}
	}*/
	return 0;
}


