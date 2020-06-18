#include "parking.h"
#include "fpga_dot_font.h"
void* t_ultrasonic();
void* t_recv_from_server();
void* t_touch_screen();

void print_lcd(char*, char*);
void charge_button(void);
void parking(char,char*);
void led_on(int);
void led_off(int);
void lcd_clear();


/***************************************************
	This is functions that draw screen.
****************************************************/
int makepixel(int  r, int g, int b);
void drawLineHor(int height, int start_x, int length_x, int pixel, int* pfbdata, struct fb_var_screeninfo fvs);
void drawLineVer(int width, int start_y, int length_y, int pixel, int* pfbdata, struct fb_var_screeninfo fvs);
void drawRect(int x1, int y1, int x2, int y2, int pixel, int* pfbdata, struct fb_var_screeninfo fvs);
void clearDisplay(int* pfbdata, struct fb_var_screeninfo fvs);
void drawUpDiagonal(int x1, int y1, int end_x, int pixel, int* pfbdata, struct fb_var_screeninfo fvs);
void drawDownDiagonal(int x1, int y1, int end_x, int pixel, int* pfbdata, struct fb_var_screeninfo fvs);
void drawMainScreen(int* pfbdata, struct fb_var_screeninfo fvs);
void drawCar(int x1, int y1, int* pfbdata,struct fb_var_screeninfo fvs, int flag);
void  circle( int x_center, int y_center, int radius,int* pfbdata, struct fb_var_screeninfo fvs);
static void ploat_circle(int x_center, int y_center, int x_coor, int y_coor,int* pfbdata, struct fb_var_screeninfo fvs);
void dot(int x, int y, int* pfbdata, struct fb_var_screeninfo fvs);
void drawRect2(int x1, int y1, int x2, int y2, int pixel);
void car(int i, int flag);


int client_sockfd,client_len;
//-----
int* pfbdata;
struct fb_var_screeninfo fvs;
//------
int dev_button,dev_fnd,dev_dot,dev_led;
int available;
int count_car;
unsigned char led;
Parking_user user[AREA_NUM];

int main(int argc, char **argv) {
	struct sockaddr_in clientaddr;
	char us_buff[2];
	char msg[BUFF_SIZE];
	pthread_t p_thread[3];
	int thread_id[3],status;
	int i;
	/******
	screen
	******/	
	int check,frame_fd;
	int pixel;
	int offset, posx1, posy1, posx2, posy2;
	int repx, repy;	
	//----------------------------------------------------------------------
	if ((client_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {  // create server socket
		perror("error : ");
		return 1;
	}
	else printf("Client_socket() sockfd is OK...\n");

	clientaddr.sin_family = AF_INET;
	clientaddr.sin_addr.s_addr = inet_addr(argv[1]); //init server ip
	clientaddr.sin_port = htons(SERV_PORT); // init server port
	memset(&(clientaddr.sin_zero), 0, 8);

	client_len = sizeof(clientaddr);

	if (connect(client_sockfd, (struct sockaddr*)&clientaddr, client_len) == -1) { //connect part
		perror("connect error : ");
		exit(1);
	}
	else printf("Client_connect() is OK...\n\n");
	memset(&user,0,sizeof(user));
	
	dev_button=open("/dev/push_switch",O_RDONLY);
	dev_fnd=open("/dev/fnd",O_RDWR);
	dev_dot=open("/dev/dot",O_WRONLY);
	dev_led=open("/dev/led",O_RDWR);
	if((frame_fd = open("/dev/fb0",O_RDWR))<0)
	{
 		perror("Frame Buffer Open Error!");
 		exit(1);
 	}
 	if((check=ioctl(frame_fd,FBIOGET_VSCREENINFO,&fvs))<0)
	{
 		perror("Get Information Error - VSCREENINFO!");
 		exit(1);
 	}	
	
	pfbdata = (int *) mmap(0, fvs.xres*fvs.yres*4, PROT_READ| \
 	PROT_WRITE, MAP_SHARED, frame_fd, 0);               
	

 	if((unsigned)pfbdata == (unsigned)-1) 
	{
 		perror("Error Mapping!\n");
 	}
	
	write(dev_dot,fpga_number[count_car],sizeof(fpga_number[count_car]));
	write(dev_led, &led,1);
	drawMainScreen(pfbdata,fvs);

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

	munmap(pfbdata,fvs.xres*fvs.yres*4); 
 	close(frame_fd);
	close(client_sockfd);
	close(dev_button);
	close(dev_fnd);
	close(dev_dot);
	close(dev_led);
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
	
		if(dist>=20 && dist<=25){
			printf("distance: %d\n",dist);
			if(available>=AREA_NUM){
				//lcd_print *not available*
				print_lcd("SORRY","NOT AVAILABLE");
			}
			else{
				sleep(1);
				send(client_sockfd,"start",strlen("start")+1,0);
				/*
					Infinite loop until ultrasoic is 0	
				*/
				do
				{				
					read(dev_us,us_buff,2);		
					dist=us_buff[0];
				}while(dist<50);
			}
		}
		usleep(400000);
	}
}

/***************************************************
	This is thread recieves data from server.
****************************************************/
void* t_recv_from_server(){
	char msg[BUFF_SIZE];
	char keyword;
	unsigned char value[16]="";
	int rcv_byte,i;
	while(1){
		rcv_byte=recv(client_sockfd, msg, sizeof(msg), 0);
		for(i = 0; i < strlen(msg); i++){
			printf("%c",msg[i]);		
		}
		printf("\n");

		keyword=msg[0];
		for(i=1;i<strlen(msg);i++){
			value[i-1]=msg[i];
		}
		printf("value: %s\n",value);
		value[i]='\0';
		switch(keyword){
			case	'$':
				print_lcd("PARKING FEE",value);
				lcd_clear();
			default:
				parking(keyword,value);
		}
	}
}

/***************************************************
This is a thread that handles screen events.
****************************************************/
void* t_touch_screen(){
	int screen_fd, ret;
    struct input_event iev[3];
	int x,y;
    screen_fd = open(DEV_SCREEN_PATH, O_RDONLY);
    if(screen_fd < 0) {
        perror("error: could not open evdev\n");
    }
	
	while(1)
    {
        ret = read(screen_fd, iev, sizeof(struct input_event)*3);
		if(ret < 0) {
            perror("error: could not read input event");
            break;
        }
        if(iev[0].type == 1 && iev[1].type == 3 && iev[2].type == 3)
        {
			x=iev[1].value;
			y=iev[2].value;
			if((829<=x && x<=1013)&&(504<=y && y<=589)){
           		printf("x = %d, y = %d \n",x,y);
				charge_button();
			}	
        }
    }
    close(screen_fd);
}

void charge_button(){
	unsigned char push_sw_buf[IOM_PUSH_SWITCH_MAX_BUTTON];
	unsigned char fnd_data[4];
	unsigned char send_data[5]="";
	int push_cnt=0;
	int flag=FALSE;
	int i;
	memset(fnd_data,0,sizeof(fnd_data));
	while(push_cnt<4){
		read(dev_button,&push_sw_buf,sizeof(push_sw_buf));
		for(i=0; i< IOM_PUSH_SWITCH_MAX_BUTTON;i++){
			if(push_sw_buf[i]){
				usleep(400000);
				fnd_data[push_cnt]=i+1;
				send_data[push_cnt]='1'+i;
				push_cnt++;
				write(dev_fnd,fnd_data,FND_MAX_DIGIT);
			}
		}
	}
	sleep(1);
	send_data[4]='\0';
	for(i=0;i<AREA_NUM;i++){
		if(!strcmp(send_data,user[i].car_number)){
			memset(&user[i],0,sizeof(user[i]));
			flag=TRUE;
			count_car--;
			led_off(i);
			car(i,FALSE);
			write(dev_dot,fpga_number[count_car],sizeof(fpga_number[count_car]));
		}
	}
	if(!flag){
		//lcd print not exist car number
		print_lcd("SORRY.","not exist car");
		lcd_clear();
	}
	else{
		send(client_sockfd,send_data,strlen(send_data)+1,0);

	}
	memset(fnd_data,0,sizeof(fnd_data));
	write(dev_fnd,fnd_data,FND_MAX_DIGIT);
}

void print_lcd(char* str1, char* str2) {
	unsigned char buf[TEXT_LCD_MAX_BUF];
	int dev, len1, len2;
	int i;
	len1 = strlen(str1);
	len2 = strlen(str2);
	dev = open(TEXT_LCD_DEVICE, O_WRONLY);
	memset(buf, ' ', TEXT_LCD_MAX_BUF);
	write(dev, buf, TEXT_LCD_MAX_BUF);
	memcpy(buf, str1, len1);
	memcpy(buf + TEXT_LCD_LINE_BUF, str2, len2);
 
	write(dev, buf, TEXT_LCD_MAX_BUF);

	close(dev);
}
void lcd_clear(){
	sleep(3);
	print_lcd("                    ","                    ");
}

void parking(char area,char* num){
	int n=area-'A';
	int i;

	user[n].area=area;
	strcpy(user[n].car_number,num);
	count_car++;
	led_on(n);
	car(n, TRUE);
	write(dev_dot,fpga_number[count_car],sizeof(fpga_number[count_car]));
}
void led_on(int  num){

	switch(num){
		case	0:
			led|= (1<<7);
			break;
		case	1:
			led|= (1<<3);
			break;
		case	2:
			led|= (1<<6);
			break;
		case	3:
			led|= (1<<2);
			break;
		case	4:
			led|= (1<<5);
			break;
		case	5:
			led|= (1<<1);
			break;
	}
	write(dev_led, &led,1);

}
void led_off(int num){

	switch(num){
		case	0:
			led&= ~(1<<7);
			break;
		case	1:
			led&= ~(1<<3);
			break;
		case	2:
			led&= ~(1<<6);
			break;
		case	3:
			led&= ~(1<<2);
			break;
		case	4:
			led&= ~(1<<5);
			break;
		case	5:
			led&= ~(1<<1);
			break;
	}
	write(dev_led, &led,1);
}

int makepixel(int  r, int g, int b)
{
    return (int)((r<<16)|(g<<8)|b);
}

void drawArrow(int x1, int y1, int end_x, int end_y_up, int end_y_down , int* pfbdata, struct fb_var_screeninfo fvs){
	
	int pixel = makepixel(255,255,255);	
	int offset = y1 * fvs.xres + x1;
	int i, j, k;	
	int len = (end_x - x1)/3;

	*(pfbdata + offset) = pixel;
	for(i = y1, j = y1; i <= y1 + len, j >= y1 - len; i ++, j--){
		while(x1 != (x1 + len)){	
			for(k = i; k < j; k--){
				offset = k * fvs.xres + x1;
				*(pfbdata + offset) = pixel;
			}
			x1 += 1;
		}	
	} 
}

void dot(int x, int y, int* pfbdata, struct fb_var_screeninfo fvs){
	int offset;
	int pixel = makepixel(0,0,0);
	offset = y * fvs.xres + x;
	*(pfbdata + offset) = pixel;
}

static void ploat_circle(int x_center, int y_center, int x_coor, int y_coor,int* pfbdata, struct fb_var_screeninfo fvs){
	dot( x_center +x_coor, y_center +y_coor,pfbdata,fvs);
	dot( x_center -x_coor, y_center +y_coor,pfbdata,fvs);
	dot( x_center +x_coor, y_center -y_coor,pfbdata,fvs);
   	dot( x_center -x_coor, y_center -y_coor,pfbdata,fvs);
   	dot( x_center +y_coor, y_center +x_coor,pfbdata,fvs);
   	dot( x_center -y_coor, y_center +x_coor,pfbdata,fvs);
   	dot( x_center +y_coor, y_center -x_coor,pfbdata,fvs);
   	dot( x_center -y_coor, y_center -x_coor,pfbdata,fvs);	
} 

void  circle( int x_center, int y_center, int radius,int* pfbdata, struct fb_var_screeninfo fvs)
{
   int      x_coor;
   int      y_coor;
   int      p_value;

   x_coor   = 0;
   y_coor   = radius;
   p_value   = 3 - 2 * radius;
   while   ( x_coor < y_coor)
   {
      ploat_circle( x_center, y_center, x_coor, y_coor,pfbdata,fvs);
      if ( p_value < 0)
      {
         p_value   += 4 * x_coor +6;
      }
      else
      {
         p_value   += 4 * ( x_coor -y_coor) +10;
         y_coor--;
      }
      x_coor++;
   }
   if ( x_coor == y_coor)
      ploat_circle( x_center, y_center, x_coor, y_coor,pfbdata,fvs);
}

void drawLineHor(int height, int start_x, int length_x, int pixel, int* pfbdata, struct fb_var_screeninfo fvs){
	int horOffset = height * fvs.xres;
	int i = 0;
	for(i = start_x; i < start_x + length_x; i++){
		*(pfbdata + horOffset + i) = pixel;	
	}
}

void drawLineVer(int width, int start_y, int length_y, int pixel, int* pfbdata, struct fb_var_screeninfo fvs){
	int verOffset = width;
	int i = 0;
	for(i = start_y; i < start_y + length_y; i++){
		*(pfbdata + verOffset + (i * fvs.xres)) = pixel;	
	}
}

void drawRect(int x1, int y1, int x2, int y2, int pixel, int* pfbdata, struct fb_var_screeninfo fvs){
	int i = 0, j = 0, rectOffset;	
	for(i = y1; i <= y2; i++) {
 		rectOffset = i * fvs.xres;
	 	for(j = x1;j <= x2; j++)
 			*(pfbdata + rectOffset + j) = pixel;
 	}
}

void drawRect2(int x1, int y1, int x2, int y2, int pixel){
	int i = 0, j = 0, rectOffset;	
	for(i = y1; i <= y2; i++) {
 		rectOffset = i * fvs.xres;
	 	for(j = x1;j <= x2; j++)
 			*(pfbdata + rectOffset + j) = pixel;
 	}
}

//car size(width : 106, height : 150)
void drawCar(int x1, int y1, int* pfbdata,struct fb_var_screeninfo fvs, int flag){
	int i, j, offset,pixel;
	int wheel_pixel = makepixel(192,192,192);
	int wheel_Line_Pixel = makepixel(0,0,0);
	if(flag == TRUE)
		pixel = makepixel(255,0,0);
	else
		pixel = makepixel(0,255,0);

	for(i = y1; i <= y1 + 150; i++){
		offset = i * fvs.xres;
		for(j = x1; j <= x1 + 106; j++)
			*(pfbdata + offset + j) = pixel;	
	}
	
	for(i = y1 + 20; i <= y1 + 50 ; i++){
		offset = i * fvs.xres;
		for(j = x1 - 20; j <= x1; j++){
			if(j == (x1 - 10)){
				*(pfbdata + offset + j) = wheel_Line_Pixel;
			}
			else
				*(pfbdata + offset + j) = wheel_pixel;
		}	
	}

	for(i = y1 + 20; i <= y1 + 50; i++){
		offset = i * fvs.xres;
		for(j = x1 + 106; j <= x1 + 126; j++){
			if(j == (x1 + 116))
				*(pfbdata + offset + j) = wheel_Line_Pixel;
			else			
				*(pfbdata + offset +j) = wheel_pixel;		
		}	
	}

	for(i = y1 + 100; i <= y1 + 130; i++){
		offset = i * fvs.xres;
		for(j = x1 - 20; j <= x1; j++){
			if(j == (x1 - 10))
				*(pfbdata + offset + j) = wheel_Line_Pixel;
			else
				*(pfbdata + offset + j) = wheel_pixel;	
		}
	}
	
	for(i = y1 + 100; i <= y1 + 130; i++){
		offset = i * fvs.xres;
		for(j = x1 + 106; j <= x1 + 126; j++){
			if(j == (x1 + 116))
				*(pfbdata + offset + j) = wheel_Line_Pixel;
			else
				*(pfbdata + offset + j) = wheel_pixel;	
		}
	}
} 

void clearDisplay(int* pfbdata, struct fb_var_screeninfo fvs){
	int i = 0, j = 0, rectOffset;	
	for(i = 0; i < 600; i++) {
 		rectOffset = i * fvs.xres;
	 	for(j = 0;j < 1024; j++)
 			*(pfbdata + rectOffset + j) = makepixel(0,0,0);
 	}
}
void drawUpDiagonal(int x1, int y1, int end_x, int pixel, int* pfbdata, struct fb_var_screeninfo fvs){
	int i, upDiagonalOffset;
	for(i = x1; i <= end_x; i++){
		upDiagonalOffset = y1 * fvs.xres + x1;
		*(pfbdata + upDiagonalOffset + i) = pixel;
		y1 += 1;	
	} 
}
void drawDownDiagonal(int x1, int y1, int end_x, int pixel, int* pfbdata, struct fb_var_screeninfo fvs){
	int i, downDiagonalOffset;
	for(i = x1; i <= end_x; i++){
		downDiagonalOffset = y1 * fvs.xres + x1;
		*(pfbdata + downDiagonalOffset + i) = pixel;
		y1 -= 1;	
	} 
}

void drawMainScreen(int* pfbdata, struct fb_var_screeninfo fvs){
	int i;
	clearDisplay(pfbdata,fvs);
	//side	
	drawRect(0,0,1023,10,makepixel(255,255,255),pfbdata,fvs);
	drawRect(0,0,10,599,makepixel(255,255,255),pfbdata,fvs);
	drawRect(0,590,1023,599,makepixel(255,255,255),pfbdata,fvs);
	drawRect(1013,0,1023,599,makepixel(255,255,255),pfbdata,fvs);
	//parking area - up
	drawRect(0,229,659,239,makepixel(255,255,255),pfbdata,fvs);
	drawRect(649,0,659,239,makepixel(255,255,255),pfbdata,fvs);	
	drawRect(215,0,225,239,makepixel(255,255,255),pfbdata,fvs);
	drawRect(431,0,441,239,makepixel(255,255,255),pfbdata,fvs);
	//parking area - down
	drawRect(0,349,658,359,makepixel(255,255,255),pfbdata,fvs);
	drawRect(648,349,658,599,makepixel(255,255,255),pfbdata,fvs);
	drawRect(215,349,225,599,makepixel(255,255,255),pfbdata,fvs);
	drawRect(431,349,441,599,makepixel(255,255,255),pfbdata,fvs);
	//enterLine
	drawRect(824,0,829,239,makepixel(255,255,255),pfbdata,fvs);
	drawRect(824,359,829,599,makepixel(255,255,255),pfbdata,fvs);
	//payButton
	drawRect(824,499,1023,504,makepixel(255,255,255),pfbdata,fvs);
	drawRect(829,504,1013,589,makepixel(255,255,0),pfbdata,fvs);
	drawRect(849,524,993,569,makepixel(0,255,0),pfbdata,fvs);	
	//Line
	drawRect(40,289,80,294,makepixel(255,255,255),pfbdata,fvs);
	drawRect(180,289,220,294,makepixel(255,255,255),pfbdata,fvs);	
	drawRect(320,289,360,294,makepixel(255,255,255),pfbdata,fvs);
	drawRect(460,289,500,294,makepixel(255,255,255),pfbdata,fvs);
	drawRect(600,289,640,294,makepixel(255,255,255),pfbdata,fvs);
	circle(921,547,20,pfbdata,fvs);
	//car

	for(i = 0 ; i < 6; i++)
		car(i,FALSE);


}

void car(int i, int flag){\
	switch(i){
		case 0:
			drawCar(59,49,pfbdata,fvs,flag);
			break;
		case 1:
			drawCar(59,399,pfbdata,fvs,flag);
			
			break;
		case 2:
			drawCar(277,49,pfbdata,fvs,flag);
			break;
		case 3:
			drawCar(277,399,pfbdata,fvs,flag);
			break;
		case 4:
			drawCar(494,49,pfbdata,fvs,flag);
			break;
		case 5:
			drawCar(494,399,pfbdata,fvs,flag);
			break;
		default:
			break;
	}
}

