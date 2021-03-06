//
//  RaspServer.c
//	(a.k.a main.c)
//  Private_Blockchain_forDB
//
//  Created by Seung Hyun Wang on 2018. 6. 19..
//  Copyright © 2018년 Seung Hyun Wang. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <cjson/cJSON.h>
#include "cjson/cJSON.h"
#include "cJSON.h"
#include "RaspServer.h"


#pragma warning(disable: 4996)
#define _CRT_SECURE_NO_WARNINGS
#define BUF_SIZE 4096
#define MAX_CLNT 256
#define EPOLL_SIZE 50



int clnt_cnt = 0;
int clnt_socks[MAX_CLNT];
char jsonarr[BUF_SIZE];
pthread_mutex_t mutx;

int main(int argc, char *argv[])
{
	

	InterAndro(argc, argv); //나중에는 andro안에서 block이 호출되어야함
	 //InterBlock("askjjdhskdjkjs");//for debug Client test
		
	return 0;
}



void InterAndro(int argc,char *argv[] ) 
{
	int serv_sock, clnt_sock;
	int nSockOpt = 1;
	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t clnt_adr_sz;
	pthread_t serversnd_thread, serverrcv_thread;
	
	char buf[BUF_SIZE];
	int str_len, i;
	
	struct epoll_event *ep_events;
	struct epoll_event event;
	int epfd, event_cnt;

	
	if(argc != 2) 
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
  
	pthread_mutex_init(&mutx, NULL);
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET; 
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));	//port number

	setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &nSockOpt, sizeof(nSockOpt));
	//Prevention of the same port's bind error
	
	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1)
		error_handling("bind() error");
	
	if(listen(serv_sock, 5) == -1)
		error_handling("listen() error");
	
	epfd = epoll_create(EPOLL_SIZE);
	ep_events = (struct epoll_event *)malloc(sizeof(struct epoll_event)*EPOLL_SIZE);

	setnonblockingmode(serv_sock);	//Non-blocking Mode
	event.events = EPOLLIN;
	event.data.fd = serv_sock;	
	epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);

	while(1)
	{
		event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
		if(event_cnt == -1)
		{
			puts("epoll_wait() error");
			break;
		}

		puts("return epoll_wait");
		
		for(i = 0; i < event_cnt; i++)
		{
			if(ep_events[i].data.fd == serv_sock) 
			{
				clnt_adr_sz = sizeof(clnt_adr);
				clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
				setnonblockingmode(clnt_sock);
				
				event.events = EPOLLIN|EPOLLET;
				event.data.fd = clnt_sock;
				epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
				printf("connected file descriptor of client: %d \n", clnt_sock);
			}
			else
			{
					while(1)
					{
						str_len = read(ep_events[i].data.fd, buf, BUF_SIZE);
						if(str_len == 0)    // close request!
						{
							epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
							close(ep_events[i].data.fd);
							printf("closed file descriptor of client: %d \n", ep_events[i].data.fd);
							break;
						}
						else if(str_len < 0)
						{
							if(errno == EAGAIN)
								break;
						}
						else	//Request to BlockChain
						{	
							
												
							if(strcmp(buf, "InitialConnect"))
							{
								printf("From App:%s\n", buf);
								//cJSON * Temp = NULL;
								//Temp = SetFromJson(buf);				
								strcpy(buf,"{\"Body\":{\"UserAddress\":\"D8F9D0A480DF7599F2F35DD8C83463332285D67DE2D0E8FDE8E979C810B4D07B\" , \"DeviceId\":\"Device1\"},\"Header\":{\"Type\":\"InitialConnect\",\"PublicKey\":\"02F0F20701B911FFC3B312BEF534C968F9FE442A6AF606BAE1A68A4BCB7A55DFFB\",\"EncryptR\":\"1D243BF0CEF18B9F104D5EF8F501C76233A3EC9F89F77F713F5E14603F0E755A\",\"EncryptS\":\"00AED183E38009C567BEA2F7BA6129C59283BAC9305E0ABB1B9062D646C8DFCFC1\"}}");			
								
								
								write(ep_events[i].data.fd, buf, sizeof(buf));
								//printf("jsonbuf%s\n", (char *)Temp);
								//일단 어플리케이션에 디바이스추가해서 재전송
							}
							else
							{
										
							write(ep_events[i].data.fd, InterBlock(buf), str_len); 
							}
							//strncpy(buf,buf,sizeof(buf)-2);
							//strcat(buf,"\“DeviceId\” : \“Device1\”}}");
							//write(ep_events[i].data.fd, buf, str_len);
							//write(ep_events[i].data.fd, InterBlock(buf), str_len); 
							 
						}
				}
			}
		}
	}
	close(serv_sock);
	close(epfd);
	
}

void *InterBlock(char *buf) //act like client
{
	int sock;
	
	//buf="{ \"Body\" : null,	\"Header\" :	{		\"Type\" : \"NodeBroadCast\"	} }";

 
	strncpy(jsonarr,buf,BUF_SIZE);
	
	int str_len;
	struct sockaddr_in serv_adr;

	if(1) {
		printf("Usage : %s <IP> <port>\n", "127.0.0.1 7777");
		//exit(1);
	}
	
	sock = socket(PF_INET, SOCK_STREAM, 0);   
	if(sock==-1)
		error_handling("socket() error");
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serv_adr.sin_port = htons(atoi("7777"));
	
	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("connect() error!");
	else
		puts("Connected...........");
	
	while(1) 
	{
		write(sock, jsonarr, sizeof(jsonarr)); 
		str_len = read(sock, jsonarr, BUF_SIZE-1);
		jsonarr[str_len] = 0;
		//if(strcmp(jsonarr," "))
		
		printf("%s\n", jsonarr);
		return (char *)jsonarr;
	}
	close(sock);
}

	
void * handle_clnt(void * arg)	 // send to all
{
	int clnt_sock =* ((int*)arg);
	int str_len = 0, i;
	char blockinfo[BUF_SIZE];
	
	while((str_len = read(clnt_sock, blockinfo, sizeof(blockinfo))) != 0)
		send_blockinfo(blockinfo, str_len);
	
	pthread_mutex_lock(&mutx);
	for(i = 0; i < clnt_cnt; i++)   // remove disconnected client
	{
		if(clnt_sock == clnt_socks[i])
		{
			while(i++ < clnt_cnt-1)
				clnt_socks[i] = clnt_socks[i+1];
			break;
		}
	}
	clnt_cnt--;
	pthread_mutex_unlock(&mutx);
	close(clnt_sock);
	return NULL;
}

void send_blockinfo(char * blockinfo, int len)   // send to all
{
	int i;
	pthread_mutex_lock(&mutx);
	for(i = 0; i < clnt_cnt; i++)
		write(clnt_socks[i], blockinfo, len);
	pthread_mutex_unlock(&mutx);
}

void error_handling(char * blockinfo)
{
	fputs(blockinfo, stderr);
	fputc('\n', stderr);
	exit(1);
}

void setnonblockingmode(int fd)
{
	int flag=fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flag|O_NONBLOCK);
}

cJSON *SetFromJson(char *buf)
{
	cJSON * json = cJSON_Parse(buf);
	cJSON * Type = NULL;
	cJSON * Body = NULL;
	cJSON * DeviceId = NULL;
	cJSON * UserAddress = NULL;
	//uint32_t valid_json = 0;
	//bool led_operation = false;
	if(json != NULL)
	{
		Type = cJSON_GetObjectItem(json, "InitialConnect");
		Body = cJSON_GetObjectItem(json, "Body");
		//밑에 switch(Type)으로 변경 나중에
		
		UserAddress =  cJSON_GetObjectItem(json, "UserAddress");
			
		if(Type != NULL && Type->valuestring !=NULL)
		{
			cJSON_AddItemToObject(json, "DeviceId", DeviceId=cJSON_CreateObject());
			cJSON_AddStringToObject(Body, "device1", "DeviceId");
			return json;
		}
		
	}


}

