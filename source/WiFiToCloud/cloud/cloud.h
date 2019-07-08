#ifndef NEWLAND_CLOUD_H_
#define NEWLAND_CLOUD_H_

#include<stdio.h>      /*��׼�����������*/  
#include<stdlib.h>     /*��׼�����ⶨ��*/  
#include<stdint.h>     /*Unix ��׼��������*/  
//#include<sys/types.h>   
//#include<sys/stat.h>     
//#include<fcntl.h>      /*�ļ����ƶ���*/  
//#include<termios.h>    /*PPSIX �ն˿��ƶ���*/  
#include<errno.h>      /*����Ŷ���*/  
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



#define PACKET_TYPE_CONN_REQ		1	//���ӽ�������
#define PACKET_TYPE_CONN_RSP		2	//���ӽ�����Ӧ
#define PACKET_TYPE_POST_DATA		3	//�ϱ�����
#define PACKET_TYPE_POST_RSP		4	//�ϱ�������Ӧ
#define PACKET_TYPE_CMD_REQ			5	//��������
#define PACKET_TYPE_CMD_RSP			6	//������Ӧ
#define PACKET_TYPE_KEEP_ALIVE_REQ	7	//��������
#define PACKET_TYPE_KEEP_ALIVE_RSP	8	//������Ӧ


/* JSON DATA's KEY  */
//	��������
#define	PACKET_TYPE_KEY			"t"

//���ӽ�������
#define	CON_REQ_DEVICE_ID_KEY	"device"
#define	CON_REQ_KEY_KEY			"key"
#define	CON_REQ_VERSION_KEY		"ver"

//���ӽ�����Ӧ
#define	CON_RSP_STATUS_KEY		"status"

//�ϱ�����
#define	POST_REQ_MSG_ID_KEY		"msgid"
#define	POST_REQ_DATA_TYPE_KEY	"datatype"
#define	POST_REQ_DATA_KEY		"datas"

//�ϱ�������Ӧ
#define	POST_RSP_MSG_ID_KEY		"msgid"
#define	POST_RSP_STATUS_KEY		"status"

//��������
#define	CMD_REQ_CMD_ID_KEY		"cmdid"
#define	CMD_REQ_API_TAG_KEY		"apitag"
#define	CMD_REQ_DATA_KEY		"data"

//������Ӧ
#define	CMD_RSP_CMD_ID_KEY		"cmdid"
#define	CMD_RSP_API_TAG_KEY		"apitag"
#define	CMD_RSP_DATA_KEY		"data"


typedef char* PACKET;

//���ӽ�������
typedef struct{
	int msg_type;
	char* device_id;
	char* key;
	char* ver;
}CON_REQ;

//���ӽ�����Ӧ
typedef struct{
	int msg_type;
	int status;
}CON_REQ_RSP;

//�ϱ�����
typedef struct{
	int msg_type;
	char* device_id;
	int msg_id;
	int data_type;  //3��JSON��ʽ3�ַ�����2��JSON��ʽ2�ַ�����1��JSON��ʽ1�ַ�����
	int data_len;
	char* data;
}POST_REQ;

//�ϱ�������Ӧ
typedef struct{
	int msg_type;
	int msg_id;
	int status;
}POST_REQ_RSP;

#define	CMD_DATA_TYPE_NUM			1
#define	CMD_DATA_TYPE_DOUBLE		2
#define	CMD_DATA_TYPE_STRING		3
#define	CMD_DATA_TYPE_JSON			4

//��������
typedef struct{
	int msg_type;
	int cmd_id;
	char* api_tag;
	int data_type;	//���֡����㡢�ַ���, joson
	void* data;
}CMD_REQ;

//������Ӧ
typedef struct{
	int msg_type;
	int cmd_id;
	char* api_tag;
	int data_type;	//���֡����㡢�ַ���, joson
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

