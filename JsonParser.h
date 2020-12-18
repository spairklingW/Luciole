#ifndef JsonParser_H_
#define JsonParser_H_

#include <string>
#include <iostream>
#include <random>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/core.hpp>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include "JsonParser.h"
#include "Config.h"
#include "LightSource.h"

using namespace cv;
using json = nlohmann::json;


class JsonParser
{

public:
    // constructors / destructors
    JsonParser();
    JsonParser(std::string filename);
    ~JsonParser();

	void ParseJsonFromFile();

	//std::string ParseStringParamFromJsonContainerTest(const std::string paramJson);

	JsonParser& operator=(JsonParser&) = default;
	JsonParser(JsonParser&) = default;

	JsonParser& operator=(JsonParser&&) = default;
	JsonParser(JsonParser&&) = default;

    // getters / setters

	//Display
	static std::string ParseStringParamFromJsonContainer(const std::string paramJson);

	static int ParseIntParamFromJsonContainer(const std::string paramJson);

	static double ParseDoubleParamFromJsonContainer(const std::string paramJson);

	static bool ParseBoolParamFromJsonContainer(const std::string paramJson);

	static float ParseFloatParamFromJsonContainer(const std::string paramJson);

	template<class T>
	static void WriteParamToJsonObject(const std::string paramKey, const T paramValue, json & jsonObject);

	static void UpdateLightsIntensityOnJsonFile(std::vector<LightSource> lightSources);

	void FillLightSourcesContainer(std::vector<LightSource> &lightSources);

	void Practice();

private:
	const std::string _pathToConfigFile = "C:/Users/brene/Documents/CapstoneProjectWithoutQt/Debug/Config.json";
	std::vector<std::string> containerParams;
	json _jsonFromFile;

	static std::string pathToConfigFileStatic;
	static std::string pathToLightValuesFileStatic;
};

#endif /* JsonParser_H_ */