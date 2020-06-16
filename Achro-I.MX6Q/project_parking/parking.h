#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>   
#include <sys/ioctl.h>
#include <unistd.h>  
#include <errno.h>
#include <pthread.h>
#include <signal.h>

#define LED_DEVICE	"/dev/fpga_led"
#define LED_MIN		0
#define LED_MAX		255

#define FND_DEVICE	"/dev/fpga_fnd"
#define FND_MAX_DIGIT	4

#define DOT_DEVICE 	"/dev/fpga_dot"
#define DOT_MIN		0
#define DOT_MAX		9

#define TEXT_LCD_DEVICE 	"/dev/fpga_text_lcd"
#define TEXT_LCD_MAX_BUF 	32
#define TEXT_LCD_LINE_BUF	16

#define BUZZER_DEVICE		"/dev/fpga_buzzer"
#define BUZZER_ON		0
#define BUZZER_OFF		1
#define BUZZER_TOGGLE(x)	(1-(x))

#define PUSH_SWITCH_DEVICE		"/dev/fpga_push_switch"
#define IOM_PUSH_SWITCH_MAX_BUTTON	9

#define BUFF_SIZE 1024             
#define SERV_IP "172.20.10.3"  
#define SERV_PORT 9999
#define CAR_NUMBER  5
#define TRUE 1
#define FALSE 0
typedef struct strct_parking_user{
	char area;
	char car_number[CAR_NUMBER];
}Parking_user;
