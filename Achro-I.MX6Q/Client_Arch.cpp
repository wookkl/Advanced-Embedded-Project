#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>   
#include<sys/ioctl.h>
#include<unistd.h>  
#include<iostream>
#include<fstream>
#include<errno.h>
#include<pthread.h>

#define BUFF_SIZE 1024              //def buffsize
#define SERV_IP "172.20.10.2"  
#define SERV_PORT 8000
void* t_recv_from_server();


int main(int argc, char **argv) {
	struct sockaddr_in clientaddr;
	int client_sockfd;
	int client_len;
	int rcv_byte;
	int msg[BUFF_SIZE];
	int pthread_t p_thread;
	int thread_id;
	int status;
	if ((client_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {  // create server socket
		perror("error : ");
		return 1;
	}
	else printf("Client_socket() sockfd is OK...\n");

	clientaddr.sin_family = AF_INET;
	clientaddr.sin_addr.s_addr = inet_addr(SERV_IP); //init server ip
	clientaddr.sin_port = htons(SERV_PORT); // init server port
	memset(&(clientaddr.sin_zero), 0, 8);

	client_len = sizeof(clientaddr);

	if (connect(client_sockfd, (struct sockaddr*)&clientaddr, client_len) == -1) { //connect part
		perror("connect error : ");
		exit(1);
	}
	else printf("Client_connect() is OK...\n\n");

	thread_id = pthread_create(&p_thread, NULL, t_recv_from_server);
	rcv_byte = recv(client_sockfd, msg, sizeof(msg), 0);
	printf("%s",msg);

	pthread_cancel(p_thread);
	pthread_join(p_thread, (void**)&status) ;
	close(client_sockfd);
	return 0;
}

void* t_recv_from_server(){
	int rcv_byte;
	while(1)
	{
		rcv_byte=recv(client_sockfd, msg, sizeof(msg), 0);
		switch(msg){
			case	:
			case	:
			default	:
				break;
		}
	}
}