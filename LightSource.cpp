#include <iostream>
#include <random>
#include <algorithm>
#include <ctime>
#include "LightSource.h"
#include <stdio.h>      
#include <stdlib.h>     
#include <time.h>   


LightSource::LightSource()
{
}

LightSource::LightSource(std::string filename)
{
    std::cout << "LightSource Constructor" << std::endl;
    
}

LightSource::LightSource(std::shared_ptr<RenderingSimulator> hardwareSimulator) : _hardwareSimulator(hardwareSimulator)
{
}

LightSource::~LightSource()
{
    std::cout << "LightSource Destructor" << std::endl;
}

void LightSource::GenerateLightPosition(int idxLightSource, int numberLightSources)
{
	int widthImageReference = _hardwareSimulator->GetHWRendering()->size().width;
	int heightImageReference = _hardwareSimulator->GetHWRendering()->size().height;

	Point2f randomLightPosition{ static_cast<float>(100*idxLightSource + 150.0f), static_cast<float>(100*idxLightSource + 50.0f) };

	Set(randomLightPosition);

	_hardwareSimulator->mockDisplayImageLight(300, randomLightPosition, idxLightSource);

	std::cout << "random " << randomLightPosition.x << " for width " << widthImageReference << "\n";
	std::cout << "random " << randomLightPosition.y << " for height " << heightImageReference << "\n";
}

void LightSource::updateHardware()
{
	_hardwareSimulator->mockDisplayLight(_intensity);
}
