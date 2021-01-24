#ifndef Config_H_
#define Config_H_

#include <string>
#include <iostream>
#include <random>
#include <algorithm>
#include <ctime>
#include <chrono>
#include <thread>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/core.hpp>

using namespace cv;

class Config
{

public:
    // constructors / destructors
    Config() = default;
    ~Config() = default;

	// static variables

	//Image Parser Mock
	static int numberLightSources;
	static int numberImagesParserMock;
	static std::string basePathImg;
	static std::string formatImgFile;
	static std::string pathImgRef;

	//DisplayImage
	static std::string pathToVideo;
	static bool mockScenario;

	//Room
	static double thresholdInitLightMock;
	static int numberFrameToEvaluateLightPose;
	static std::string basePathVideoLightInitialization;
	static std::string formatVideoLightInitialization;
	static int startFrame;
	static int endFrame;
	static double thresholdDiffImages;
	static double scaleMaxDistancePrevBarycentrePointForSamePoint;
	static double scaleMaxDistanceCurrentBarPrevPoint;
	static int totalIntensity;
	static bool useQueueBufferFrame;

	//Image Processor
	static float maxThresholdFilterLightBrightness;
	static int thresholdFilterLightBrightness;
	static float rateFilterContourByShape;
	static double eraseRateFilterByPoints;
	static double eraseRateFilterByBoundingBoxSize;
};

#endif 