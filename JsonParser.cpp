#include <iostream>
#include <random>
#include <algorithm>
#include <ctime>
#include "JsonParser.h"

std::string JsonParser::pathToConfigFileStatic = "/home/pi/git_repos/Luciole_Main/Luciole/Config/config.json";
std::string JsonParser::pathToLightValuesFileStatic = "/home/pi/git_repos/Luciole_Main/Luciole/Config/lightIntensity.json";

//Mat JsonParser::_hardwareRenderingStatic{imread("C:/Users/brene/Documents/CapstoneProjectWithoutQt/Debug/move1.jpeg")};
// constructor WITHOUT memory allocation
JsonParser::JsonParser()
{
}

// constructor WITH memory allocation
JsonParser::JsonParser(std::string filename)
{
    std::cout << "JsonParser Constructor" << std::endl;
    
}

JsonParser::~JsonParser()
{
    std::cout << "JsonParser Destructor" << std::endl;
}

void JsonParser::ParseJsonFromFile()
{
	std::ifstream i(pathToConfigFileStatic);
	i >> _jsonFromFile;
}

/*
std::string JsonParser::ParseStringParamFromJsonContainerTest(const std::string paramJson)
{
	std::ifstream i(pathToConfigFileStatic);
	json j;
	i >> j;

	std::string param = j[paramJson].get<std::string>();
	std::cout << param << std::endl;

	// write prettified JSON to another file
	std::ofstream o("pretty2.json");
	o << std::setw(4) << j << std::endl;
	return param;
}
*/

std::string JsonParser::ParseStringParamFromJsonContainer(const std::string paramJson)
{
	std::ifstream i(pathToConfigFileStatic);
	json j;
	i >> j;

	std::string param = j[paramJson].get<std::string>();
	std::cout << param << std::endl;

	return param;
}

int JsonParser::ParseIntParamFromJsonContainer(const std::string paramJson)
{
	std::ifstream i(pathToConfigFileStatic);
	json j;
	i >> j;

	int param = j[paramJson].get<int>();
	std::cout << param << std::endl;

	return param;
}

double JsonParser::ParseDoubleParamFromJsonContainer(const std::string paramJson)
{
	std::ifstream i(pathToConfigFileStatic);
	json j;
	i >> j;

	double param = j[paramJson].get<double>();
	std::cout << param << std::endl;

	return param;
}

bool JsonParser::ParseBoolParamFromJsonContainer(const std::string paramJson)
{
	std::ifstream i(pathToConfigFileStatic);
	json j;
	i >> j;

	bool param = j[paramJson].get<bool>();
	std::cout << param << std::endl;

	return param;
}

float JsonParser::ParseFloatParamFromJsonContainer(const std::string paramJson)
{
	std::ifstream i(pathToConfigFileStatic);
	json j;
	i >> j;

	float param = j[paramJson].get<float>();
	std::cout << param << std::endl;

	return param;
}

template<class T>
void JsonParser::WriteParamToJsonObject(const std::string paramKey, T paramValue, json & jsonObject)
{
	jsonObject[paramKey] = paramValue;
}

void JsonParser::UpdateLightsIntensityOnJsonFile(std::vector<LightSource>& lightSources)
{
	json jsonObject;
	for (auto & lightSource : lightSources)
	{
		//auto intensity_parse = lightSource.GetId();
		JsonParser::WriteParamToJsonObject("light " + std::to_string(lightSource.GetId()), lightSource.GetIntensity(), jsonObject);
		//JsonParser::WriteParamToJsonObject("light " + std::to_string(3),2 , jsonObject);
	}

	std::ofstream o(pathToLightValuesFileStatic);
	o << std::setw(4) << jsonObject << std::endl;
}

void JsonParser::FillLightSourcesContainer(std::vector<LightSource> &lightSources)
{
	int max = 4;
	for (int i = 0; i < max; i++)
	{
		LightSource lightSource;
		int id = i+ 3;
		double intensity = 10 + i ;
		lightSource.Set(id);
		lightSource.Set(intensity);
		lightSources.emplace_back(lightSource);
	}
}

void JsonParser::Practice()
{
	std::vector<LightSource> lightSources;

	FillLightSourcesContainer(lightSources);

	//JsonParser::UpdateLightsIntensityOnJsonFile(lightSources);
}

