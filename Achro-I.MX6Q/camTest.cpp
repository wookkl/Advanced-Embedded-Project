#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#define FBDEV	"/dev/fb0"
#define CAMERA_COUNT	100
#define CAM_WIDTH	640
#define CAM_HEIGHT	480

using namespace cv;

int main(void)
{
	IplImage* image = 0;
	CvCapture* capture = cvCaptureFromCAM(2);
	cvReleaseCapture(&capture);
	return 0;
}
