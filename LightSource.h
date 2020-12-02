#ifndef LightSource_H_
#define LightSource_H_
#include <iostream>
#include <random>
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <string>
#include "RenderingSimulator.h"

using namespace cv;

class LightSource
{

public:
    // constructors / destructors
    LightSource();                    
    LightSource(std::string filename); 

	LightSource(std::shared_ptr < RenderingSimulator> hardwareSimulator);

    ~LightSource();

	
	LightSource& operator=(LightSource&) = default;
	LightSource(LightSource&) = default;

	LightSource& operator=(LightSource&&) = default;
	LightSource(LightSource&&) = default;
	
		
	// General
	void GenerateLightPosition(int idxLightSource, int numberLightSources);

	//Access Class Modifiers
	int GetId() { return _id; };
	double GetIntensity() { return _intensity; };
	Point GetPosition() { return _position; };
	std::shared_ptr<RenderingSimulator> GetHardwareSimulator() { return _hardwareSimulator; };

	void Set(int id) { this->_id = id; };
	void Set(double intensity) { this->_intensity = intensity; updateHardware(); };
	void Set(Point position) { this->_position = position; };
	void Set(std::shared_ptr<RenderingSimulator> renderingSimulator) {this->_hardwareSimulator = renderingSimulator; };

private:
	//Display
	void updateHardware();

	double _intensity = 0;
	int _id = 0;
	Point _position;
	std::shared_ptr<RenderingSimulator> _hardwareSimulator;
};

#endif /* LightSource_H_ */