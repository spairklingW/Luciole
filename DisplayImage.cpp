#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/core.hpp>
#include <iostream>
#include <map>
#include <vector>
#include <numeric>
#include <string>
#include <functional>
#include "DisplayImage.h"

using namespace cv;


int main(int argc, char** argv )
{
	// ******************** Test OpenCV import ***************************************************************************

	Mat tigerImg = imread("C:/Users/brene/Documents/CapstoneProjectWithoutQt/images_room/TestOpenCV/tiger.jpg", IMREAD_GRAYSCALE);
	namedWindow("tiger", WINDOW_NORMAL);
	imshow("tiger", tigerImg);
	
    waitKey(0);
    return 0;
}