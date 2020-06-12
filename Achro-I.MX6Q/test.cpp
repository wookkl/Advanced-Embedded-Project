#include <iostream>
#include <fstream>
#include <sstream>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <linux/input.h>
#include <dirent.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include <wait.h>

#define WIDTH 1024
#define HEIGHT 600

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	unsigned short	pix ;
	unsigned short	*fbData ;
	int		fd ;					// TFTLCD fd
	Mat image;

  	if((fd = open("/dev/fb0", O_RDWR)) < 0)			// TFTLCD device open
	{
		printf("fb open error\n") ;
		exit(0) ;
	}
	
	fbData = (unsigned short*)mmap(0, WIDTH*HEIGHT*2,PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0) ;	// 

	for(int i=0 ; i<HEIGHT*WIDTH; i++)
	{	
		*(fbData+i) = 0x0000 ;
	}

    	
    	image = imread("index.png");
	cout << cv::getBuildInformation() << endl;


	if ( !image.data )
    	{
        	printf("No image data \n");
        	return -1;
    	}
    	else{
   		cout << image <<endl;
    	}

	for(int k=HEIGHT-1 ; k>=0 ; k--)			// Drawing on TFTLCD
	{
		for(int j=WIDTH-1 ; j>=0 ; j--)		
		{
			pix = image.at<Vec3b>(k, j)[0]/8 + 
			((image.at<Vec3b>(k, j)[1]/4)<<5) + 
			((image.at<Vec3b>(k,j)[2]/8)<<11) ;
			*(fbData+k*WIDTH+j) = pix ;
		}
	}
    	
    	return 0;
}
