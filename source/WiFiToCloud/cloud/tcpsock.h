/**********************************************************
	Modify History: qiuzhb make, 2016-8-21
**********************************************************/


#ifndef TCPSOCK_H_
#define TCPSOCK_H_

#define MAX_CONN	10
#define ERROR_PROCESS	1

#define TCP_SOCKET_DEBUG
#ifdef TCP_SOCKET_DEBUG
#define TCP_SOCKET_DBG(fmt, args...)	printf("TCP SOCKET DEBUG:%s:%d, "fmt, __FUNCTION__, __LINE__, ##args)
#else
#define TCP_SOCKET_DBG(fmt, args...)
#endif
#define TCP_SOCKET_ERR(fmt, args...)	printf("TCP SOCKET ERROR:%s:%d, "fmt, __FUNCTION__, __LINE__, ##args)

typedef enum
{
	SELECT_READ,
	SELECT_WRITE,
	SELECT_EXCEPTION,
	SELECT_FOR_SLEEP
}SELECT_OPT;

//AT指令列表
#define WIFI_AP		"newland edu ma"
#define WIFI_PWD	"maliwei8266"
#define AT_CWMODE	"AT+CWMODE_CUR=1" //设置为“station”模式
#define CWJAP		"AT+CWJAP_CUR"
#define AT_CWJAP	"AT+CWJAP_CUR=\"zhihuiyun\",\"yanghang\""
#define AT_CIPSTART	"AT+CIPSTART=\"TCP\",\"120.77.58.34\",8600"
#define	AT_CIPSEND	"AT+CIPSEND=" 
//{"t":1,"device":"cloudsafebox615","key":"94ccf37b469a4985a65b31ac04cc0910","ver":"v0.0.0.0"}
//{"t":3,"datatype":1,"datas":{"alarm":{"2018-06-15 18:15:02":1}},"msgid":001}
#define PING_REQ "$#AT#\r"//$#AT#\r
#define PING_RSP "$OK##\r"//$OK##\r




int open_client_port(unsigned short client_port);
int set_socket_non_blocking(int sock);
int connection_server(int client_sock, char* server_num_and_dot_IPv4, unsigned short server_port);
int close_connection(int sock);
int send_packet(int sock, void *buf, int len, int is_no_wait);
int receive_packet(int sock, void *buf, int len, int is_no_wait);
int select_socket(int sock, int ms_timeout, SELECT_OPT opt);
int clear_sock_receive_buffer(int sock);

#endif // #ifndef TCPSOCK_H_
