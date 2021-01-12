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

int main(int argc, char** argv )
{
	// ******************** Json Test ***************************************************************************
	/*
	JsonParser jsonParser;
	jsonParser.Practice();	

	std::cout << "print variable : " << Config::pathToVideo << std::endl;
	*/

	// ******************** Test OpenCV import ***************************************************************************

	Mat tigerImg = imread("C:/Users/brene/Documents/CapstoneProjectWithoutQt/images_room/TestOpenCV/tiger.jpg", IMREAD_GRAYSCALE);
	mean(tigerImg);
	std::cout << mean(tigerImg);
	//std::cout << tigerImg.at<Vec3b>(20, 20);
	namedWindow("tiger", WINDOW_NORMAL);
	imshow("tiger", tigerImg);

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