#ifndef RenderingSimulator_H_
#define RenderingSimulator_H_

#include <string>
#include <iostream>
#include <random>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/core.hpp>
#include <string>
#include "RenderingSimulator.h"

using namespace cv;


class RenderingSimulator
{

public:
    // constructors / destructors
    RenderingSimulator();
    RenderingSimulator(std::string filename);
	RenderingSimulator(std::shared_ptr<Mat> hardwareRendering);
    ~RenderingSimulator();

	RenderingSimulator& operator=(RenderingSimulator&) = default;
	RenderingSimulator(RenderingSimulator&) = default;

	RenderingSimulator& operator=(RenderingSimulator&&) = default;
	RenderingSimulator(RenderingSimulator&&) = default;

    // getters / setters
	std::shared_ptr<Mat> GetHWRendering() { return _hardwareRendering; };

	//Display
	void mockDisplayLight(double intensity);
	void mockDisplayImageLight(double intensity, const Point2f & position, int index);
	void mockDisplayImageSingleLightForInitialization(double intensity, const Point2f & position, Mat&  displaySingleLightSource);

private:

	std::shared_ptr<Mat> _hardwareRendering;
	int _numberOfLights = 0;
	int _numberOfSimulator = 0;
};

#endif /* RenderingSimulator_H_ */