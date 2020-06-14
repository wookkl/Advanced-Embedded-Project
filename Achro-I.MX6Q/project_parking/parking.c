#include "parking.h"
void* t_ultrasonic();
void* t_button();
void* t_recv_from_server();

int client_sockfd,client_len;
int dev_lcd,dev_button,dev_fnd,dev_dot;
char car_num[4];
char car_area[3];
void charge();
int main(int argc, char **argv) {
	struct sockaddr_in clientaddr;
	char us_buff[2];
	char msg[BUFF_SIZE];
	pthread_t p_thread[5];
	int thread_id[5],status;
	
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

	dev_lcd=open("/dev/text_lcd",O_WRONLY);
	dev_button=open("/dev/push_switch",O_RDONLY);
	dev_fnd=open("/dev/fnd",O_RDWR);
	dev_dot=open("/dev/dot",O_WRONLY);

	thread_id[0] = pthread_create(&p_thread[0], NULL, t_ultrasonic,NULL);
	thread_id[1] = pthread_create(&p_thread[1], NULL, t_recv_from_server,NULL);
	pthread_cancel(p_thread[0]);
	pthread_cancel(p_thread[1]);
	pthread_join(p_thread[0], (void**)&status) ;
	pthread_join(p_thread[1], (void**)&status) ;	
	close(client_sockfd);
	return 0;
}
void* t_ultrasonic(){
	int dev_us;
	char us_buff[2];
	int dist;
	int flag=0;
	dev_us=open("/dev/us",O_RDWR);
	
	while(1)
	{
		read(dev_us,us_buff,2);
		dist=us_buff[0];
		
		if(dis>=20 && dis<=25){
			printf("value: %d %d\n",dist,flag);
			send(client_sockfd,"start",strlen("start")+1,0);
			flag=1;
		}
		while(flag);
	}
}

void* t_recv_from_server(){
	char msg[BUFF_SIZE];
	char keyword;
	char value[BUFF_SIZE];
	while(1){
		rcv_byte=recv(client_sockfd, msg, sizeof(msg), 0);
		keyword=msg[0];
		for(int i=1;i<strlen(msg);i++){
			value[i-1]=msg[i];
		}
		switch(msg[0]){
			case	'$':
				charge(value);
			default	:
				parking(value);
		}
	}
}
void charge(){
	unsigned char push_sw_buf[IOM_PUSH_SWITCH_MAX_BUTTON];
	unsigned char fnd_data[4];
	int push_cnt=0;
	int i;
	memset(fnd_data,0,sizeof(fnd_data));
	while(push_cnt<4){
		usleep(200000);
		read(dev_button,&push_sw_buf,sizeof(push_sw_buf));
		for(i=0; i< IOM_PUSH_SWITCH_MAX_BUTTON;i++){
			if(push_sw_buf[i]){
				fnd_data[push_cnt]=i+1;
				write(dev_fnd,fnd_data,FND_MAX_DIGIT);
				push_cnt++;
			}
		}
	}
	sleep(1);
	memset(fnd_data,0,sizeof(fnd_data));
	write(dev_fnd,fnd_data,FND_MAX_DIGIT);
	send(client_sockfc,fnd_data,strlen(fnd_data));

}
void parking(char info){
	char area;
	char num[4];
	int i;
	area=info[0]; 
	for(i=0;i<4;i++)
		num[i]=info[i+1];

}
