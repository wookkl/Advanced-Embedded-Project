#include "parking.h"
void* t_ultrasonic();
void* t_recv_from_server();
void* t_touch_screen();

void print_lcd() {
void charge_button();
void parking();

int client_sockfd,client_len;
int dev_lcd,dev_button,dev_fnd,dev_dot;
int available;
Parking_user user[3];

int main(int argc, char **argv) {
	struct sockaddr_in clientaddr;
	char us_buff[2];
	char msg[BUFF_SIZE];
	pthread_t p_thread[3];
	int thread_id[3],status;
	int i;
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

	for(i=0;i<3;i++)
		user[i].area=NULL;
		user[i].car_number=NULL;
	dev_lcd=open("/dev/text_lcd",O_WRONLY);
	dev_button=open("/dev/push_switch",O_RDONLY);
	dev_fnd=open("/dev/fnd",O_RDWR);
	dev_dot=open("/dev/dot",O_WRONLY);

	thread_id[0] = pthread_create(&p_thread[0], NULL, t_ultrasonic,NULL);
	thread_id[1] = pthread_create(&p_thread[1], NULL, t_recv_from_server,NULL);
	thread_id[2] = pthread_create(&p_thread[2], NULL, t_touch_screen,NULL);
	while(1);
	pthread_cancel(p_thread[0]);
	pthread_cancel(p_thread[1]);
	pthread_cancel(p_thread[2]);
	pthread_join(p_thread[0], (void**)&status);
	pthread_join(p_thread[1], (void**)&status);
	pthread_join(p_thread[2], (void**)&status);	
	close(client_sockfd);
	return 0;
}

/***************************************************
	This is thread that handles ultrasonic value.
****************************************************/
void* t_ultrasonic(){
	int dev_us;
	char us_buff[2];
	int dist;
	dev_us=open("/dev/us",O_RDWR);
	
	while(1)
	{
		read(dev_us,us_buff,2);
		dist=us_buff[0];
		
		if(dis>=20 && dis<=25){
			if(available>=3){
				//lcd_print *not available*
				print_lcd("SORRY","NOT AVAILABLE");
			}
			else{
				sleep(1);
				send(client_sockfd,"start",strlen("start")+1,0);
				/*
					Infinite loop until ultrasoic is 0	
				*/
				while(!dist){				
					read(dev_us,us_buff,2);		
					dist=us_buff[0];
				}
			}
		}
	}
}

/***************************************************
	This is thread recieves data from server.
****************************************************/
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
		switch(keyword){
			case	'$':
				//print_lcd fee
				print_lcd("PARKING FEE",value);
			case	'!':
				parking(value);
		}
	}
}

/***************************************************
This is a thread that handles screen events.
****************************************************/
void* t_touch_screen(){
	//touch_screen thread
}

void charge_button(){
	unsigned char push_sw_buf[IOM_PUSH_SWITCH_MAX_BUTTON];
	unsigned char fnd_data[4];
	int push_cnt=0;
	int flag=FALSE;
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
	for(i=0;i<3;i++){
		if(strcmp(fnd_data,user[i].car_number)){
			memset(user[i].car_number,NULL,sizeof(user[i].car_number));
			memset(user[i].area,NULL,sizeof(user[i].car_number));
			flag=TRUE;
		}
	}
	if(!flag)
		//lcd print not exist car number
		print_lcd("SORRY.","not exist ur car")
	else
		send(client_sockfd,fnd_data,strlen(fnd_data));
}

void parking(char* num){
	int i;
	char msg[BUFF_SIZE];
	for(int i=0;i<3;){
		if (user[i].area == NULL){
			user[i].area='A'+i;
			strcpy(user[i].car_number,num);
			strcpy(msg,user[i].car_number);
			strcat(msg,user[i].area);
			send(client_sockfd,msg,strlen(msg)+1,0);
			available++;
			return;
		}
	}
}
void print_lcd(char* str1, char* str2) {
	unsigned char buf[TEXT_LCD_MAX_BUF];
	int dev, len1, len2;
	int i;
	len1 = strlen(str1);
	len2 = strlen(str2);
	dev = open(TEXT_LCD_DEVICE, O_WRONLY);

	memset(buf, ' ', TEXT_LCD_MAX_BUF);
	memcpy(buf, str1, len1);
	memcpy(buf + TEXT_LCD_LINE_BUF, str2, len2);
 
	write(dev, buf, TEXT_LCD_MAX_BUF);

	close(dev);
}

