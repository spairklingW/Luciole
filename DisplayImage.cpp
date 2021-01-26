#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/core.hpp>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <numeric>
#include <string>
#include <functional>
#include "LightSource.h"
#include "ImageProcessor.h"
#include "ImageParser.h"
#include "RenderingSimulator.h"
#include "Room.h"
#include "DisplayImage.h"
#include "Config.h"
#include <nlohmann/json.hpp>
#include "JsonParser.h"
#include <ctime>
#include <raspicam/raspicam_cv.h>

using namespace std; 

using namespace cv;
using json = nlohmann::json;

void runMockScenario(int numberLightSources)
{
	std::shared_ptr < Mat> imageRefMock = std::make_shared<Mat>(imread(Config::pathImgRef, IMREAD_GRAYSCALE));
	std::shared_ptr<RenderingSimulator>  hardwareSimulatorMock = std::make_shared<RenderingSimulator>(imageRefMock);
	Room roomMock(numberLightSources, hardwareSimulatorMock, *imageRefMock);

	namedWindow("InitializeLightsSources", WINDOW_NORMAL);
	imshow("InitializeLightsSources", *imageRefMock);

	roomMock.ModelInitializationLightSources();
	roomMock.InitializePositionMovingInstancesMock();
	roomMock.InitializeMockImages();

	roomMock.CyclicUpdateLightsFromMockImages();

	waitKey(0);
}

void runRealScenario(int numberLightSources)
{
	std::shared_ptr < Mat> imageRefRoom = std::make_shared<Mat>(ImageParser::InitializeFrameWithoutMotions(Config::pathToVideo));
	std::shared_ptr<RenderingSimulator>  hardwareSimulator = std::make_shared<RenderingSimulator>(imageRefRoom);
	Room room(numberLightSources, hardwareSimulator, *imageRefRoom);

	room.InitializeLightSourcesFromRealScene();
	room.CyclicUpdateLightsFromImages(Config::pathToVideo);

	waitKey(0);
}

void initLightSourcesFromStream()
{
	time_t timer_begin,timer_end;
	raspicam::RaspiCam_Cv Camera;
	cv::Mat image;
	int nCount=10;
	Camera.set(cv::CAP_PROP_FORMAT, CV_8UC1 );
	cout<<"Opening Camera..."<<endl;
	if (!Camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}
	cout<<"Capturing "<<nCount<<" frames ...."<<endl;
	time ( &timer_begin );
	
	/*----------------------------------------------------------------------------------*/
	Mat imageref;
	Camera.grab();
	Camera.retrieve(imageref);
	std::shared_ptr < Mat> imageRefRoom = std::make_shared<Mat>(imageref);
	std::shared_ptr<RenderingSimulator>  hardwareSimulator = std::make_shared<RenderingSimulator>(imageRefRoom);
	int numberLightSources = 4;
	Room room(numberLightSources, hardwareSimulator, *imageRefRoom);

	Mat imagestream;
	room.GetFirstStreamImage(imagestream);

	std::vector<LightSource> lightSources;
	for(int i= 0; i< numberLightSources; i ++)
	{
		lightSources.emplace_back(LightSource(i));
	}
	
	Mat imagestreamlight;
	for (int indexLightSource =0; indexLightSource < numberLightSources; indexLightSource ++)
	{
		//here the id and the position in the vector match: TO BE ADAPTED
		double intensity = 1;
		lightSources.at(i).Set(intensity);
		JsonParser::UpdateLightsIntensityOnJsonFile(lightSources);
		//set the intensity of the light to 1 so that the changes appears on the LED via python script
		for ( int i=0; i<nCount; i++ ) {
			Camera.grab();
			Camera.retrieve ( imagestreamlight);
			if ( i%5==0 )
			{
				room.InitializeLightSourcesFromStream(imagestreamlight);
				cout<<"\r captured "<<i<<" images"<<std::flush;
			}
		}
		intensity =0;
		JsonParser::UpdateLightsIntensityOnJsonFile(lightSources);
	}
	//JsonParser::UpdateLightsIntensityOnJsonFile(_lightSources);
}
 
int main ( int argc,char **argv ) {
   
	time_t timer_begin,timer_end;
	raspicam::RaspiCam_Cv Camera;
	cv::Mat image;
	int nCount=100;
	//set camera params
	Camera.set(cv::CAP_PROP_FORMAT, CV_8UC1 );
	//Open camera
	cout<<"Opening Camera..."<<endl;
	if (!Camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}
	//Start capture
	cout<<"Capturing "<<nCount<<" frames ...."<<endl;
	time ( &timer_begin );
	
	/*----------------------------------------------------------------------------------*/
	Mat imageref;
	Camera.grab();
	Camera.retrieve(imageref);
	std::shared_ptr < Mat> imageRefRoom = std::make_shared<Mat>(imageref);
	std::shared_ptr<RenderingSimulator>  hardwareSimulator = std::make_shared<RenderingSimulator>(imageRefRoom);
	Room room(4, hardwareSimulator, *imageRefRoom);

	room.InitializeLightSourcesFromRealScene();
	Mat imagestream;
	room.GetFirstStreamImage(imagestream);
	
	Mat imagestreamcontinous;
	for ( int i=0; i<nCount; i++ ) {
		Camera.grab();
		Camera.retrieve ( imagestreamcontinous);
		if ( i%5==0 )
		{
			room.UpdateLightsFromStreamImages(imagestreamcontinous);
			cout<<"\r captured "<<i<<" images"<<std::flush;
		}
	}
	
	
	/*----------------------------------------------------------------------------------*/
	
	for ( int i=0; i<nCount; i++ ) {
		Camera.grab();
		Camera.retrieve ( image);
		if ( i%5==0 )  cout<<"\r captured "<<i<<" images"<<std::flush;
	}
	cout<<"Stop camera..."<<endl;
	Camera.release();
	//show time statistics
	time ( &timer_end ); /* get current time; same as: timer = time(NULL)  */
	double secondsElapsed = difftime ( timer_end,timer_begin );
	cout<< secondsElapsed<<" seconds for "<< nCount<<"  frames : FPS = "<<  ( float ) ( ( float ) ( nCount ) /secondsElapsed ) <<endl;
	//save image 
	cv::imwrite("raspicam_cv_image.jpg",image);
	cout<<"Image saved at raspicam_cv_image.jpg"<<endl;


	// ******************** Json Test ***************************************************************************
	
	//JsonParser jsonParser;
	//jsonParser.Practice();	

	//std::cout << "print variable : " << Config::pathToVideo << std::endl;
	

	// ******************** Test OpenCV import ***************************************************************************

	//Mat tigerImg = imread("C:/Users/brene/Documents/CapstoneProjectWithoutQt/images_room/TestOpenCV/tiger.jpg", IMREAD_GRAYSCALE);
	//mean(tigerImg);
	//std::cout << mean(tigerImg);
	//std::cout << tigerImg.at<Vec3b>(20, 20);
	//namedWindow("tiger", WINDOW_NORMAL);
	//imshow("tiger", tigerImg);

	// Use Json parser to update lights
	
	// ******************** Mock Scene ***************************************************************************
	if (Config::mockScenario == true)
	{
		runMockScenario(Config::numberLightSources);
	}
	// ******************** Real Scene ***************************************************************************
	else
	{
		runRealScenario(Config::numberLightSources);
	}
	
	
    	waitKey(0);
}


/*
int main(int argc, char** argv )
{
	// ******************** Json Test ***************************************************************************
	
	//JsonParser jsonParser;
	//jsonParser.Practice();	

	//std::cout << "print variable : " << Config::pathToVideo << std::endl;
	

	// ******************** Test OpenCV import ***************************************************************************

	//Mat tigerImg = imread("C:/Users/brene/Documents/CapstoneProjectWithoutQt/images_room/TestOpenCV/tiger.jpg", IMREAD_GRAYSCALE);
	//mean(tigerImg);
	//std::cout << mean(tigerImg);
	//std::cout << tigerImg.at<Vec3b>(20, 20);
	//namedWindow("tiger", WINDOW_NORMAL);
	//imshow("tiger", tigerImg);

	// Use Json parser to update lights
	
	// ******************** Mock Scene ***************************************************************************
	if (Config::mockScenario == true)
	{
		runMockScenario(Config::numberLightSources);
	}
	// ******************** Real Scene ***************************************************************************
	else
	{
		runRealScenario(Config::numberLightSources);
	}
	
	
    waitKey(0);
    return 0;
}
*/
