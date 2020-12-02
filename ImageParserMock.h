#ifndef ImageParserMock_H_
#define ImageParserMock_H_

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
//#include "Room.h"

using namespace cv;


class ImageParserMock
{

public:
    // constructors / destructors
    ImageParserMock();
    ImageParserMock(std::string filename);
    ~ImageParserMock();

	ImageParserMock& operator=(ImageParserMock&) = default;
	ImageParserMock(ImageParserMock&) = default;

	ImageParserMock& operator=(ImageParserMock&&) = default;
	ImageParserMock(ImageParserMock&&) = default;

    // getters / setters
	std::vector<Mat> Get() { return _imagesFromCamera; };

	// methods for mocking camera recording
	void FillContainerWithImages();

	Mat InitializeFrameWithoutMotionsMock();

	bool IsCameraInActiveState() { return _cameraGetNewFrames; };

private:
	std::vector<Mat> _imagesFromCamera;
	Mat _emptyImage;
	Mat _refImageWithoutMotion;
	std::thread _threadRecordFrames;
	bool _cameraGetNewFrames = true;
};

#endif 