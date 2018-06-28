#ifndef __RaspServer_h__
#define __RaspServer_h__

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/epoll.h>

#ifndef cJSON__h
#define cJSON__h
#endif

void *InterBlock(char *buf);//Tcp communication with BlockChain Node
void InterAndro(int argc, char *argv[]);//Tcp communication with Application
void * handle_clnt(void * arg);
void send_blockinfo(char * blockinfo, int len);
void error_handling(char * blockinfo);
void setnonblockingmode(int fd);
cJSON *SetFromJson(char *buf);
#endif