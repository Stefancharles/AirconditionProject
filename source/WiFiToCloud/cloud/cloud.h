#ifndef NEWLAND_CLOUD_H_
#define NEWLAND_CLOUD_H_

#include<stdio.h>      /*标准输入输出定义*/  
#include<stdlib.h>     /*标准函数库定义*/  
#include<stdint.h>     /*Unix 标准函数定义*/  
//#include<sys/types.h>   
//#include<sys/stat.h>     
//#include<fcntl.h>      /*文件控制定义*/  
//#include<termios.h>    /*PPSIX 终端控制定义*/  
#include<errno.h>      /*错误号定义*/  
#include<string.h>  

#include "tcpsock.h"
#include "cJSON.h"



//#define NEWLAND_CLOUD_DEBUG
#ifdef NEWLAND_CLOUD_DEBUG
#define NEWLAND_CLOUD_DBG(fmt, args...)	printf("NEWLAND CLOUD DEBUG:%s:%d, "fmt, __FUNCTION__, __LINE__, ##args)
#else
#define NEWLAND_CLOUD_DBG(fmt, args...)
#endif
#define NEWLAND_CLOUD_ERR(fmt, args...)	printf("NEWLAND CLOUD ERROR:%s:%d, "fmt, __FUNCTION__, __LINE__, ##args)



#define PACKET_TYPE_CONN_REQ		1	//连接建立请求
#define PACKET_TYPE_CONN_RSP		2	//连接建立响应
#define PACKET_TYPE_POST_DATA		3	//上报数据
#define PACKET_TYPE_POST_RSP		4	//上报数据响应
#define PACKET_TYPE_CMD_REQ			5	//命令请求
#define PACKET_TYPE_CMD_RSP			6	//命令响应
#define PACKET_TYPE_KEEP_ALIVE_REQ	7	//心跳请求
#define PACKET_TYPE_KEEP_ALIVE_RSP	8	//心跳响应


/* JSON DATA's KEY  */
//	请求类型
#define	PACKET_TYPE_KEY			"t"

//连接建立请求
#define	CON_REQ_DEVICE_ID_KEY	"device"
#define	CON_REQ_KEY_KEY			"key"
#define	CON_REQ_VERSION_KEY		"ver"

//连接建立响应
#define	CON_RSP_STATUS_KEY		"status"

//上报数据
#define	POST_REQ_MSG_ID_KEY		"msgid"
#define	POST_REQ_DATA_TYPE_KEY	"datatype"
#define	POST_REQ_DATA_KEY		"datas"

//上报数据响应
#define	POST_RSP_MSG_ID_KEY		"msgid"
#define	POST_RSP_STATUS_KEY		"status"

//命令请求
#define	CMD_REQ_CMD_ID_KEY		"cmdid"
#define	CMD_REQ_API_TAG_KEY		"apitag"
#define	CMD_REQ_DATA_KEY		"data"

//命令响应
#define	CMD_RSP_CMD_ID_KEY		"cmdid"
#define	CMD_RSP_API_TAG_KEY		"apitag"
#define	CMD_RSP_DATA_KEY		"data"


typedef char* PACKET;

//连接建立请求
typedef struct{
	int msg_type;
	char* device_id;
	char* key;
	char* ver;
}CON_REQ;

//连接建立响应
typedef struct{
	int msg_type;
	int status;
}CON_REQ_RSP;

//上报数据
typedef struct{
	int msg_type;
	char* device_id;
	int msg_id;
	int data_type;  //3：JSON格式3字符串；2：JSON格式2字符串；1：JSON格式1字符串；
	int data_len;
	char* data;
}POST_REQ;

//上报数据响应
typedef struct{
	int msg_type;
	int msg_id;
	int status;
}POST_REQ_RSP;

#define	CMD_DATA_TYPE_NUM			1
#define	CMD_DATA_TYPE_DOUBLE		2
#define	CMD_DATA_TYPE_STRING		3
#define	CMD_DATA_TYPE_JSON			4

//命令请求
typedef struct{
	int msg_type;
	int cmd_id;
	char* api_tag;
	int data_type;	//数字、浮点、字符串, joson
	void* data;
}CMD_REQ;

//命令响应
typedef struct{
	int msg_type;
	int cmd_id;
	char* api_tag;
	int data_type;	//数字、浮点、字符串, joson
	void* data;
}CMD_REQ_RSP;

PACKET packet_con_req(CON_REQ* con_req);
PACKET packet_post_req(POST_REQ* post_req);
PACKET packet_cmd_req_rsp(CMD_REQ_RSP* cmd_rsp);
PACKET packet_msg(void* msg);
void free_packet_msg(PACKET packet);
void* unpacket_con_req_rsp(cJSON* msg);
void* unpacket_post_req_rsp(cJSON* msg);
void* unpacket_cmd_req(cJSON* msg);
void* unpacket_msg(PACKET msg);
void free_unpacket_msg(void* msg);
void hex_dump(const unsigned char *buf, int len);
void test_con_req_rsp(void);
void test_post_req_rsp(void);
void test_cmd_req_rsp(void);

#endif

