#include<stdio.h>      /*标准输入输出定义*/  
#include<stdlib.h>     /*标准函数库定义*/  
#include<stdint.h>   
//#include<sys/types.h>   
//#include<sys/stat.h>     
//#include<fcntl.h>      /*文件控制定义*/  
//#include<termios.h>    /*PPSIX 终端控制定义*/  
#include<errno.h>      /*错误号定义*/  
#include<string.h>  
#include "cJSON.h"
#include "cloud.h"


PACKET packet_con_req(CON_REQ* con_req)
{
	cJSON* json_msg;
	char* str;

	if(con_req == NULL)
		return NULL;
	
	json_msg = cJSON_CreateObject();
	cJSON_AddNumberToObject(json_msg, PACKET_TYPE_KEY, con_req->msg_type);
	cJSON_AddStringToObject(json_msg, CON_REQ_DEVICE_ID_KEY, con_req->device_id);
	cJSON_AddStringToObject(json_msg, CON_REQ_KEY_KEY, con_req->key);
	cJSON_AddStringToObject(json_msg, CON_REQ_VERSION_KEY, con_req->ver);
	str = cJSON_Print(json_msg);
	cJSON_Delete(json_msg);

	return str;
}

PACKET packet_post_req(POST_REQ* post_req)
{
	cJSON* json_msg;
	char* str;
	cJSON* json_data;

	if(post_req == NULL || post_req->data == NULL)
		return NULL;
	
	json_data = cJSON_Parse(post_req->data);
	if(json_data == NULL){
		NEWLAND_CLOUD_ERR("cJSON_Parse error\n");
		return NULL;
	}
	
	json_msg = cJSON_CreateObject();
	cJSON_AddNumberToObject(json_msg, PACKET_TYPE_KEY, post_req->msg_type);
	cJSON_AddNumberToObject(json_msg, POST_REQ_MSG_ID_KEY, post_req->msg_id);
	cJSON_AddNumberToObject(json_msg, POST_REQ_DATA_TYPE_KEY, post_req->data_type);
	cJSON_AddItemToObject(json_msg, POST_REQ_DATA_KEY, json_data);
	
	str = cJSON_Print(json_msg);
	cJSON_Delete(json_msg);

	return str;
}

PACKET packet_cmd_req_rsp(CMD_REQ_RSP* cmd_rsp)
{
	cJSON* json_msg;
	char* str;
	cJSON* json_data;

	if(cmd_rsp == NULL || cmd_rsp->data == NULL)
		return NULL;

	json_msg = cJSON_CreateObject();
	cJSON_AddNumberToObject(json_msg, PACKET_TYPE_KEY, cmd_rsp->msg_type);
	cJSON_AddNumberToObject(json_msg, CMD_RSP_CMD_ID_KEY, cmd_rsp->cmd_id);
	cJSON_AddStringToObject(json_msg, CMD_RSP_API_TAG_KEY, cmd_rsp->api_tag);

	switch(cmd_rsp->data_type){
		case CMD_DATA_TYPE_NUM:
			cJSON_AddNumberToObject(json_msg, CMD_RSP_DATA_KEY, *((int*)cmd_rsp->data));
			break;
		case CMD_DATA_TYPE_DOUBLE:
			cJSON_AddNumberToObject(json_msg, CMD_RSP_DATA_KEY, *((double*)cmd_rsp->data));
			break;
		case CMD_DATA_TYPE_STRING:
			cJSON_AddStringToObject(json_msg, CMD_RSP_DATA_KEY, cmd_rsp->data);
			break;
		case CMD_DATA_TYPE_JSON:
			json_data = cJSON_Parse(cmd_rsp->data);
			if(json_data == NULL){
				NEWLAND_CLOUD_ERR("cJSON_Parse error\n");
				goto packet_cmd_req_error;
			}
			cJSON_AddItemToObject(json_msg, CMD_RSP_DATA_KEY, json_data);
			break;
		default:
			NEWLAND_CLOUD_ERR("cmd_req->data_type(%d) error\n", cmd_rsp->data_type);
			goto packet_cmd_req_error;
	}
	
	str = cJSON_Print(json_msg);
	cJSON_Delete(json_msg);
	return str;

packet_cmd_req_error:
	cJSON_Delete(json_msg);
	return NULL;
}


PACKET packet_msg(void* msg)
{
	int* msg_type;
	PACKET ret = NULL;

	if(msg == NULL)
		return NULL;

	msg_type = (int*)msg;
	switch(*msg_type){
		case PACKET_TYPE_CONN_REQ:{
			ret = packet_con_req((CON_REQ*)msg);
			break;
		}
		case PACKET_TYPE_POST_DATA:{
			ret = packet_post_req((POST_REQ*)msg);
			break;
		}
		case PACKET_TYPE_CMD_RSP:{
			ret = packet_cmd_req_rsp((CMD_REQ_RSP*)msg);
			break;
		}
		default:
			NEWLAND_CLOUD_ERR("msg_type(%d) error\n", *msg_type);
			break;
	}

	return ret;
}

void free_packet_msg(PACKET packet)
{
	if(packet)
		free(packet);
}

void* unpacket_con_req_rsp(cJSON* msg)
{
	CON_REQ_RSP* con_req_rsp;
	cJSON *status;

	status = cJSON_GetObjectItem(msg, CON_RSP_STATUS_KEY);
	if(status && status->type != cJSON_Number){
		NEWLAND_CLOUD_ERR("status->type(%d) error\n", status->type);
		return NULL;
	}

	con_req_rsp = (CON_REQ_RSP*)malloc(sizeof(CON_REQ_RSP));
	if(con_req_rsp){
		con_req_rsp->msg_type = PACKET_TYPE_CONN_RSP;
		con_req_rsp->status = status->valueint;
	}

	return con_req_rsp;
}

void* unpacket_post_req_rsp(cJSON* msg)
{
	POST_REQ_RSP* post_req_rsp;
	cJSON *status;
	cJSON *msg_id;

	msg_id = cJSON_GetObjectItem(msg, POST_RSP_MSG_ID_KEY);
	if(msg_id && msg_id->type != cJSON_Number){
		NEWLAND_CLOUD_ERR("msg_id->type(%d) error\n", msg_id->type);
		return NULL;
	}

	status = cJSON_GetObjectItem(msg, POST_RSP_STATUS_KEY);
	if(status && status->type != cJSON_Number){
		NEWLAND_CLOUD_ERR("status->type(%d) error\n", status->type);
		return NULL;
	}

	post_req_rsp = (POST_REQ_RSP*)malloc(sizeof(POST_REQ_RSP));
	if(post_req_rsp){
		post_req_rsp->msg_type = PACKET_TYPE_POST_RSP;
		post_req_rsp->msg_id = msg_id->valueint;
		post_req_rsp->status = status->valueint;
	}

	return post_req_rsp;
}

void* unpacket_cmd_req(cJSON* msg)
{
	CMD_REQ* cmd_req;
	cJSON *data;
	cJSON *cmd_id;
	cJSON *api_tag;

	cmd_req = (CMD_REQ*)malloc(sizeof(CMD_REQ));
	if(cmd_req == NULL)
		return NULL;
	memset(cmd_req, 0, sizeof(CMD_REQ));

	cmd_id = cJSON_GetObjectItem(msg, CMD_REQ_CMD_ID_KEY);
	if(cmd_id && cmd_id->type != cJSON_Number){
		NEWLAND_CLOUD_ERR("msg_id->type(%d) error\n", cmd_id->type);
		goto unpacket_cmd_req_error;
	}

	api_tag = cJSON_GetObjectItem(msg, CMD_REQ_API_TAG_KEY);
	if(api_tag && api_tag->type != cJSON_String){
		NEWLAND_CLOUD_ERR("api_tag->type(%d) error\n", api_tag->type);
		goto unpacket_cmd_req_error;
	}

	data = cJSON_GetObjectItem(msg, CMD_REQ_DATA_KEY);
	if(data && data->type == cJSON_Number){

		if((data->valuedouble - data->valueint) > 0){
			cmd_req->data = malloc(sizeof(double));
			*((double*)cmd_req->data) = data->valuedouble;
			cmd_req->data_type = CMD_DATA_TYPE_DOUBLE;
		}else{
			cmd_req->data = malloc(sizeof(int));
			*((int*)cmd_req->data) = data->valueint;
			cmd_req->data_type = CMD_DATA_TYPE_NUM;
		}
	}else if(data && data->type == cJSON_String){
		cmd_req->data = (void*)malloc(strlen(data->valuestring) + 1);
		memset(cmd_req->data, 0, strlen(data->valuestring) + 1);
		strcpy(cmd_req->data, data->valuestring);
		cmd_req->data_type = CMD_DATA_TYPE_STRING;
	}else if(data && data->type == cJSON_Object){
		cmd_req->data = cJSON_Print(data);
		cmd_req->data_type = CMD_DATA_TYPE_JSON;
	}else{
		NEWLAND_CLOUD_ERR("status->type(%d) error\n", data->type);
		goto unpacket_cmd_req_error;
	}

	cmd_req->msg_type = PACKET_TYPE_CMD_REQ;
	cmd_req->cmd_id = cmd_id->valueint;
	cmd_req->api_tag = (char*)malloc(strlen(api_tag->valuestring) + 1);
	memcpy(cmd_req->api_tag, api_tag->valuestring, strlen(api_tag->valuestring) + 1);
	
	return cmd_req;

unpacket_cmd_req_error:
	free(cmd_req);
	return NULL;
}


void* unpacket_msg(PACKET msg)
{
	cJSON *msg_json = NULL;
	cJSON *msg_type = NULL;
	void* ret = NULL;

	if(msg == NULL)
		return NULL;
	
	msg_json = cJSON_Parse(msg);
	if(msg_json == NULL){
		NEWLAND_CLOUD_DBG("cJSON_Parse error\n");
		return NULL;
	}

	if(msg_json->type != cJSON_Object){
		NEWLAND_CLOUD_ERR("packet->type(%d) error\n", msg_json->type);
		return NULL;
	}

	msg_type = cJSON_GetObjectItem(msg_json, PACKET_TYPE_KEY);
	if(msg_type && msg_type->type != cJSON_Number){
		NEWLAND_CLOUD_ERR("msg_type->type(%d) error\n", msg_type->type);
		return NULL;
	}
	switch(msg_type->valueint){
		case PACKET_TYPE_CONN_RSP:{
			ret = unpacket_con_req_rsp(msg_json);
			break;
		}
		case PACKET_TYPE_POST_RSP:{
			ret = unpacket_post_req_rsp(msg_json);
			break;
		}
		case PACKET_TYPE_CMD_REQ:{
			ret = unpacket_cmd_req(msg_json);
			break;
		}
		default:
			NEWLAND_CLOUD_ERR("msg_type(%d) error\n", msg_type->valueint);
			break;
	}

	cJSON_Delete(msg_json);

	return ret;
	
}

void free_unpacket_msg(void* msg)
{
	int* msg_type;

	if(msg == NULL)
		return;

	msg_type = (int*)msg;
	switch(*msg_type){
		case PACKET_TYPE_CONN_RSP:
		case PACKET_TYPE_POST_RSP:
			free(msg);
			break;
		case PACKET_TYPE_CMD_REQ:{
			CMD_REQ* cmd_req = (CMD_REQ*)msg;
			
			if(cmd_req->api_tag)
				free(cmd_req->api_tag);
			if(cmd_req->data)
				free(cmd_req->data);
			free(cmd_req);
			break;
		}
		default:
			NEWLAND_CLOUD_ERR("msg_type(%d) error\n", *msg_type);
			break;
	}
}


void hex_dump(const unsigned char *buf, int len)
{
    int i;
	
    for (i = 0; i < len; i++) 
    {
        printf("%02X ", buf[i]);
        if ((i+1)%8 == 0) 
            printf("\n");
    }
    printf("\n");
}


void test_con_req_rsp(void)
{
	PACKET packet;
	CON_REQ con_req;
	CON_REQ_RSP* con_req_rsp;

	con_req.msg_type = PACKET_TYPE_CONN_REQ;
	con_req.device_id = "123456";
	con_req.key = "999999";
	con_req.ver = "V1.0";
	packet = packet_msg(&con_req);
	printf("%s\n", packet);
	free_packet_msg(packet);

	con_req_rsp = unpacket_msg("{\
	\"t\": 2,\
	\"status\": 0\
	}");
	printf("unpacket, msg_type:%d, status:%d\n", con_req_rsp->msg_type, con_req_rsp->status);
	free_unpacket_msg(con_req_rsp);

}


void test_post_req_rsp(void)
{
	PACKET packet;
	POST_REQ post_req;
	POST_REQ_RSP* post_req_rsp;

	//数据类型为1（JSON格式1字符串）
	post_req.msg_type = PACKET_TYPE_POST_DATA;
	post_req.device_id = "123456890";
	post_req.msg_id = 1;
	post_req.data_type = 1;
	post_req.data = "{\
	\"apitag1\": \"value1\",\
	\"apitag2\": 2\
	}";
	post_req.data_len = strlen(post_req.data);
	
	packet = packet_msg(&post_req);
	if(packet){
		printf("%s\n", packet);
		free_packet_msg(packet);
	}else{
		printf("packet_msg error\n");
	}

	//数据类型为2（JSON格式2字符串）
	post_req.msg_type = PACKET_TYPE_POST_DATA;
	post_req.device_id = "123456890";
	post_req.msg_id = 1;
	post_req.data_type = 2;
	post_req.data = "{\
		\"apitag1\":{\"datetime1\":\"value1\"},\
		\"apitag2\": {\"datetime2\":\"value2\"}\
		}";
	post_req.data_len = strlen(post_req.data);
	
	packet = packet_msg(&post_req);
	if(packet){
		printf("%s\n", packet);
		free_packet_msg(packet);
	}else{
		printf("packet_msg error\n");
	}


	//数据类型为3（JSON格式3字符串）
	post_req.msg_type = PACKET_TYPE_POST_DATA;
	post_req.device_id = "123456890";
	post_req.msg_id = 1;
	post_req.data_type = 3;
	post_req.data = "\
	[\
		{\
			\"apitag\":\"temperature1\",\
			\"datapoints\":[\
				{\
				\"dt\":\"2018-01-22 22:22:22\",\
				\"value\": 36.5\
				},\
				{\
				\"dt\":\"2018-01-22 11:11:11\",\
				\"value\": 30.5\
				}\
			]\
		},\
		{\
			\"apitag\":\"temperature2\",\
			\"datapoints\":[\
				{\
				\"dt\":\"2018-01-22 00:00:00\",\
				\"value\": 36.5\
				},\
				{\
				\"dt\":\"2018-01-22 12:12:12\",\
				\"value\": 55.5\
				}\
			]\
		}\
	]";
	post_req.data_len = strlen(post_req.data);
	
	packet = packet_msg(&post_req);
	if(packet){
		printf("%s\n", packet);
		free_packet_msg(packet);
	}else{
		printf("packet_msg error\n");
	}


	post_req_rsp = unpacket_msg("{\
			\"t\": 4,\
			\"msgid\": 123,\
			\"status\":0\
		}");
	printf("unpacket, msg_type:%d, msg_id:%d status:%d\n", post_req_rsp->msg_type, post_req_rsp->msg_id, post_req_rsp->status);
	free_unpacket_msg(post_req_rsp);

}


void test_cmd_req_rsp(void)
{
	PACKET packet;
	CMD_REQ* cmd_rcv;
	CMD_REQ_RSP cmd_req_rsp;
	int num = 555;
	double temp = 123.456;
	
	//数字
	cmd_req_rsp.msg_type = PACKET_TYPE_CMD_RSP;
	cmd_req_rsp.cmd_id = 123456;
	cmd_req_rsp.api_tag = "asdfbasf";
	cmd_req_rsp.data_type = 1;
	cmd_req_rsp.data = &num;
	packet = packet_msg(&cmd_req_rsp);
	printf("%s\n", packet);
	free_packet_msg(packet);

	//符点
	cmd_req_rsp.msg_type = PACKET_TYPE_CMD_RSP;
	cmd_req_rsp.cmd_id = 123456;
	cmd_req_rsp.api_tag = "asdfbasf";
	cmd_req_rsp.data_type = 2;
	cmd_req_rsp.data = &temp;
	packet = packet_msg(&cmd_req_rsp);
	printf("%s\n", packet);
	free_packet_msg(packet);


	//字符串
	cmd_req_rsp.msg_type = PACKET_TYPE_CMD_RSP;
	cmd_req_rsp.cmd_id = 123456;
	cmd_req_rsp.api_tag = "asdfbasf";
	cmd_req_rsp.data_type = 3;
	cmd_req_rsp.data = "data";
	packet = packet_msg(&cmd_req_rsp);
	printf("%s\n", packet);
	free_packet_msg(packet);


	//JSON
	cmd_req_rsp.msg_type = PACKET_TYPE_CMD_RSP;
	cmd_req_rsp.cmd_id = 123456;
	cmd_req_rsp.api_tag = "asdfbasf";
	cmd_req_rsp.data_type = 4;
	cmd_req_rsp.data = "{\
	\"apitag1\": \"value1\",\
	\"apitag2\": 2\
	}";
	packet = packet_msg(&cmd_req_rsp);
	printf("%s\n", packet);
	free_packet_msg(packet);

	printf("========================================================\n");
	
	cmd_rcv = unpacket_msg("\
	{\
		\"t\": 5,\
		\"cmdid\": 123,\
		\"apitag\":\"rgb_open\",\
		\"data\":10\
	}");
		
	if(cmd_rcv){
		printf("unpacket, msg_type:%d, msg_id:%d data:%d\n", cmd_rcv->msg_type, cmd_rcv->cmd_id, *((int*)cmd_rcv->data));
	
		free_unpacket_msg(cmd_rcv);
	}
	else{
		printf("unpacket_msg error\n");
	}

	cmd_rcv = unpacket_msg("\
	{\
		\"t\": 5,\
		\"cmdid\": 456,\
		\"apitag\":\"rgb_open\",\
		\"data\": \"55667899\"\
	}");
	if(cmd_rcv){
		printf("unpacket, msg_type:%d, msg_id:%d data:%s\n", cmd_rcv->msg_type, cmd_rcv->cmd_id, (char*)cmd_rcv->data);
	
		free_unpacket_msg(cmd_rcv);
	}
	else{
		printf("unpacket_msg error\n");
	}

	cmd_rcv = unpacket_msg("\
	{\
		\"t\": 5,\
		\"cmdid\": 456,\
		\"apitag\":\"rgb_open\",\
		\"data\": 88.345\
	}");
	if(cmd_rcv){
		printf("unpacket, msg_type:%d, msg_id:%d data:%f\n", cmd_rcv->msg_type, cmd_rcv->cmd_id, *((double*)cmd_rcv->data));
	
		free_unpacket_msg(cmd_rcv);
	}
	else{
		printf("unpacket_msg error\n");
	}

	
	cmd_rcv = unpacket_msg("\
		{\
			\"t\": 5,\
			\"cmdid\": 16783,\
			\"apitag\":\"rgb_open\",\
			\"data\": {\
						\"apitag1\": \"value1\",\
						\"apitag2\": 2\
						}\
		}");
		if(cmd_rcv){
			printf("unpacket, msg_type:%d, msg_id:%d data:%s\n", cmd_rcv->msg_type, cmd_rcv->cmd_id, (char*)cmd_rcv->data);
	
			free_unpacket_msg(cmd_rcv);
		}
		else{
			printf("unpacket_msg error\n");
		}

}


