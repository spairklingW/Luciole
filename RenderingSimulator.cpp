#include <iostream>
#include <random>
#include <algorithm>
#include <ctime>
#include "RenderingSimulator.h"

//Mat RenderingSimulator::_hardwareRenderingStatic{imread("C:/Users/brene/Documents/CapstoneProjectWithoutQt/Debug/move1.jpeg")};
// constructor WITHOUT memory allocation
RenderingSimulator::RenderingSimulator()
{
}

// constructor WITH memory allocation
RenderingSimulator::RenderingSimulator(std::string filename)
{
    std::cout << "RenderingSimulator Constructor" << std::endl;
    
}

RenderingSimulator::RenderingSimulator(std::shared_ptr<Mat> hardwareRendering) : _hardwareRendering(hardwareRendering)
{
	_numberOfSimulator ++;
}

RenderingSimulator::~RenderingSimulator()
{
    std::cout << "RenderingSimulator Destructor" << std::endl;
}

void RenderingSimulator::mockDisplayLight(double intensity)
{
	//in the future link it to real hardware somehow// or update specific file
	std::cout << "light is being displayed at level " << intensity << " LUM" << "\n";
}

void RenderingSimulator::mockDisplayImageLight(double intensity, const Point2f & position, int index)
{
	std::cout << "light is being displayed at level " << intensity << " LUM" << "\n";

	circle(*_hardwareRendering, position, 50, Scalar(230, 280, 230), -3, 8, 0);
	_numberOfLights ++;

	namedWindow("Hardware Rendering Light init" + std::to_string(index), WINDOW_NORMAL);
	imshow("Hardware Rendering Light init" + std::to_string(index), *_hardwareRendering);

}

void RenderingSimulator::mockDisplayImageSingleLightForInitialization(double intensity, const Point2f & position, Mat&  displaySingleLightSource)
{
	std::cout << "light is being displayed at level " << intensity << " LUM" << "\n";

	circle(displaySingleLightSource, position, 50, Scalar(230, 280, 230), -3, 8, 0);

	namedWindow("Hardware Rendering", WINDOW_NORMAL);
	imshow("Hardware Rendering", displaySingleLightSource);

}
