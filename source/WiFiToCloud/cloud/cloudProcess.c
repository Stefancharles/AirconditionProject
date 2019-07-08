#include<stdio.h>      /*标准输入输出定义*/  
#include<stdlib.h>     /*标准函数库定义*/  
#include<unistd.h>     /*Unix 标准函数定义*/  
#include<sys/types.h>   
#include<sys/stat.h>     
#include<fcntl.h>      /*文件控制定义*/  
#include<termios.h>    /*PPSIX 终端控制定义*/  
#include<errno.h>      /*错误号定义*/  
#include<string.h>  
#include <time.h>
#include <pthread.h>

#include "tcpsock.h"
#include "cJSON.h"
#include "cloud.h"

#define MAIN_DBG(fmt, args...)	printf("DEBUG:%s:%d, "fmt, __FUNCTION__, __LINE__, ##args)
#define MAIN_ERR(fmt, args...)	printf("ERR:%s:%d, "fmt, __FUNCTION__, __LINE__, ##args)



#define SERVER_IP	"120.77.58.34"
#define	SERVER_PORT	8600

int is_auth_ok = 0;

void* post_process(void* param)
{
	int ret;
	CON_REQ con_req;
	PACKET packet;
	POST_REQ post_req;
	struct timespec sleep_time;
	long tick = 0;
	int send_flag = 0;
	int sock = *((int*)param);
	
	con_req.msg_type = PACKET_TYPE_CONN_REQ;
	con_req.device_id = "qiuzhb";
	con_req.key = "677abb3a5ff3456fb831c96482f11536";
	con_req.ver = "V1.0";
	packet = packet_msg(&con_req);
	ret = send_packet(sock, packet, strlen(packet), 0);
	if(ret < 0){
		MAIN_ERR("PACKET_TYPE_CONN_REQ error\n");
	}
	free_packet_msg(packet);

	//10 milli second
	sleep_time.tv_sec = 0;
	sleep_time.tv_nsec = 100000000;

	memset(&post_req, 0, sizeof(post_req));

	while(1){
		if(is_auth_ok){
			if(tick == 30){
				//数据类型为1（JSON格式1字符串）
				post_req.msg_type = PACKET_TYPE_POST_DATA;
				post_req.msg_id++;
				post_req.data_type = 1;
				post_req.data = "{\
				\"nl_temperature\": \"23\",\
				\"nl_light\": 2000\
				}";
				post_req.data_len = strlen(post_req.data);
				packet = packet_msg(&post_req);
				if(packet == NULL){
					MAIN_ERR("packet_msg JSON 1 error\n");
				}else{
					MAIN_DBG("POST JSON 1 \n");
					send_flag = 1;
				}
			}else if(tick == 60){
				//数据类型为2（JSON格式2字符串）
				post_req.msg_type = PACKET_TYPE_POST_DATA;
				post_req.msg_id++;
				post_req.data_type = 2;
				post_req.data = "{\
					\"nl_temperature\":{\"2017-03-22 22:31:12\":\"66\"},\
					\"nl_light\": {\"2017-05-22 12:31:12\":\"800\"}\
					}";
				post_req.data_len = strlen(post_req.data);
				packet = packet_msg(&post_req);
				if(packet == NULL){
					MAIN_ERR("packet_msg JSON 2 error\n");
				}else{
					MAIN_DBG("POST JSON 2 \n");
					send_flag = 1;
				}
			}else if(tick == 90){
				//数据类型为3（JSON格式3字符串）
				post_req.msg_type = PACKET_TYPE_POST_DATA;
				post_req.msg_id++;
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
				if(packet == NULL){
					MAIN_ERR("packet_msg JSON 3 error\n");
				}else{
					MAIN_DBG("POST JSON 3 \n");
					send_flag = 1;
				}
			}
			
			if(send_flag){
				ret = send_packet(sock, packet, strlen(packet), 0);
				if(ret < 0){
					MAIN_ERR("PACKET_TYPE_POST_DATA error\n");
				}
				free_packet_msg(packet);
				send_flag = 0;
			}
		}

		//sleep 10 milli second
		nanosleep((const struct timespec*)&sleep_time, NULL);
		tick++;
		if(tick > 90){
			sleep(80);
			tick = 0;
		}
	}

	return 0;
}

#define	KEEP_ALIVE_MSG		"$#AT#\r"
#define	KEEP_ALIVE_RSP		"$OK##\r"

void* recv_process(void* param)
{
	char msg_buf[128];
	int ret;
	void* msg_unpacket;
	int* msg_type;
	int sock = *((int*)param);

	while(1){
		memset(msg_buf, 0, sizeof(msg_buf));
		ret = receive_packet(sock, msg_buf, sizeof(msg_buf), 0);
		if(ret > 0){
			//hex_dump((const unsigned char*)msg_buf, ret);
			msg_unpacket = unpacket_msg(msg_buf);
			if(msg_unpacket == NULL){
				if(strcmp(msg_buf, KEEP_ALIVE_MSG) == 0){
					MAIN_DBG("recv keep alive\n");
					ret = send_packet(sock, KEEP_ALIVE_RSP, strlen(KEEP_ALIVE_RSP), 0);
					if(ret < 0){
						MAIN_ERR("send keep alive error\n");
					}else{
						MAIN_DBG("send keep alive OK\n");
					}
				}else{
					hex_dump((const unsigned char*)msg_buf, ret);
					MAIN_DBG("not JSON msg or keep alive msg(%s) \n", msg_buf);
				}
			}else{
				MAIN_DBG("recv:\n%s\n", msg_buf);
				msg_type = (int*)msg_unpacket;
				switch(*msg_type){
					case PACKET_TYPE_CONN_RSP:{
						CON_REQ_RSP* con_req_rsp = (CON_REQ_RSP*)msg_unpacket;

						MAIN_DBG("unpacket, msg_type:%d, status:%d\n", con_req_rsp->msg_type, con_req_rsp->status);
						if(con_req_rsp->status == 0){
							MAIN_DBG("server authentication OK\n");
							is_auth_ok = 1;
						}
						break;
					}
					case PACKET_TYPE_POST_RSP:{
						POST_REQ_RSP* post_req_rsp = (POST_REQ_RSP*)msg_unpacket;

						MAIN_DBG("unpacket, msg_type:%d, msg_id:%d status:%d\n", post_req_rsp->msg_type, post_req_rsp->msg_id, post_req_rsp->status);
						if(post_req_rsp->status == 0){
							MAIN_DBG("POST SUCESS\n");
						}
						
						break;
					}
					case PACKET_TYPE_CMD_REQ:{
						CMD_REQ* cmd_rcv = (CMD_REQ*)msg_unpacket;
						int is_cmd_need_rsp = 0;
						PACKET packet;
						CMD_REQ_RSP* cmd_rsp = (CMD_REQ_RSP*)cmd_rcv;	//CMD_REQ struct is same with CMD_REQ_RSP struct

						MAIN_DBG("recv CMD, data type:%d\n", cmd_rcv->data_type);
						switch(cmd_rcv->data_type){
							case CMD_DATA_TYPE_NUM:
								is_cmd_need_rsp = 1;
								MAIN_DBG("unpacket, msg_type:%d, msg_id:%d apitag:%s, data:%d\n", 
										cmd_rcv->msg_type, cmd_rcv->cmd_id, cmd_rcv->api_tag, *((int*)cmd_rcv->data));
	
								break;
							case CMD_DATA_TYPE_DOUBLE:
								is_cmd_need_rsp = 1;
								MAIN_DBG("unpacket, msg_type:%d, msg_id:%d apitag:%s, data:%f\n", 
										cmd_rcv->msg_type, cmd_rcv->cmd_id, cmd_rcv->api_tag, *((double*)cmd_rcv->data));
								break;
							case CMD_DATA_TYPE_STRING:
								is_cmd_need_rsp = 1;
								MAIN_DBG("unpacket, msg_type:%d, msg_id:%d apitag:%s, data:%s\n", 
										cmd_rcv->msg_type, cmd_rcv->cmd_id, cmd_rcv->api_tag, (char*)cmd_rcv->data);
								break;
							case CMD_DATA_TYPE_JSON:
								is_cmd_need_rsp = 1;
								MAIN_DBG("unpacket, msg_type:%d, msg_id:%d apitag:%s, data:%s\n", 
										cmd_rcv->msg_type, cmd_rcv->cmd_id, cmd_rcv->api_tag, (char*)cmd_rcv->data);
								break;
							default:
								MAIN_ERR("data_type(%d) error\n", cmd_rcv->data_type);
						}
						if(is_cmd_need_rsp){
							cmd_rsp->msg_type = PACKET_TYPE_CMD_RSP;
							packet = packet_msg(cmd_rsp);
							cmd_rsp->msg_type = PACKET_TYPE_CMD_REQ;
							if(packet == NULL){
								MAIN_ERR("packet_msg PACKET_TYPE_CMD_RSP error\n");
								break;
							}
							ret = send_packet(sock, packet, strlen(packet), 0);
							if(ret < 0){
								MAIN_ERR("send PACKET_TYPE_CMD_RSP error\n");
							}else{
								MAIN_DBG("cmd rsp:\n%s\n", packet);
							}
							free_packet_msg(packet);
						}
						break;
					}
					default:
						MAIN_ERR("msg_type(%d) error\n", *msg_type);
						break;
				}
				free_unpacket_msg(msg_unpacket);
			}
		}else if(ret == 0){
			break;
		}
	}

	return 0;
}


int main(int argc, char **argv)
{
	int sock;
	pthread_t send_pid;
	pthread_t recv_pid;

	sock = open_client_port(0);
	if(sock == -1){
		MAIN_ERR("open_client_port error\n");
		return 0;
	}

	if(connection_server(sock, SERVER_IP, SERVER_PORT) < 0){
		MAIN_ERR("connection_server error\n");
		return 0;
	}else{
		MAIN_DBG("connect server OK\n");
	}


    pthread_create(&send_pid, NULL, post_process, &sock);
	pthread_create(&recv_pid, NULL, recv_process, &sock);
	
	pthread_join(send_pid, NULL);
	pthread_join(recv_pid, NULL);
	return 0;
}

