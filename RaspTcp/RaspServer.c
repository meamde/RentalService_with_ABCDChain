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
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include "../CJson/cJSON.h"
#include "RaspServer.h"

#pragma warning(disable: 4996)
#define _CRT_SECURE_NO_WARNINGS
#define BUF_SIZE 100
#define MAX_CLNT 256



int clnt_cnt = 0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;

int main(int argc, char *argv[])
{
	char buf[BUF_SIZE];

	//InterAndro(argc, argv); //지금은 주석처리 , but 나중에는 andro안에서 block이 호출되어야함
	InterBlock(argc, argv);
		
	return 0;
}

void InterBlock(int argc,char *argv[] ) //Acting like Client
{
	int sock;
	unsigned int jsonsize;
	char * jsonarr[BUF_SIZE];
	unsigned char bytes[4];
	unsigned long n = 175;
	
	int str_len;
	struct sockaddr_in serv_adr;

	if(argc!=3) {
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	sock = socket(PF_INET, SOCK_STREAM, 0);   
	if(sock==-1)
		error_handling("socket() error");
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_adr.sin_port = htons(atoi(argv[2]));
	
	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("connect() error!");
	else
		puts("Connected...........");
	
	while(1) 
	{
		write(sock, bytes, sizeof(jsonarr)); //Request for jsonlenth
		str_len = read(sock, jsonarr, BUF_SIZE-1);
		jsonarr[str_len] = 0;
		printf("bytes from BlockChain: %s", bytes);
	}
	
	close(sock);

}


void InterAndro(int argc,char *argv[] )
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t clnt_adr_sz;
	pthread_t t_id;
	
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
	serv_adr.sin_port = htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1)
		error_handling("bind() error");
	
	if(listen(serv_sock, 5) == -1)
		error_handling("listen() error");
	
	while(1)
	{
		clnt_adr_sz = sizeof(clnt_adr);
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
		
		pthread_mutex_lock(&mutx);
		clnt_socks[clnt_cnt++] = clnt_sock;

		
		
		pthread_mutex_unlock(&mutx);
	
		pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
		pthread_detach(t_id);
		printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
	}
	close(serv_sock);
	
}
	
void * handle_clnt(void * arg)
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

