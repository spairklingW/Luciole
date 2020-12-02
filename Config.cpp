#include <iostream>
#include "Config.h"

//Image Parser Mock
int Config::numberLightSources = 4;
int Config::numberImagesParserMock = 11;
std::string Config::basePathImg = "C:/Users/brene/Documents/CapstoneProjectWithoutQt/images_room/scenario_2person_distinctPosition/empty_room";
std::string Config::formatImgFile = ".png";
std::string Config::pathImgRef = "C:/Users/brene/Documents/CapstoneProjectWithoutQt/images_room/scenario_2person_distinctPosition/empty_room0.png";

//DisplayImage
std::string Config::pathToVideo = "C:/Users/brene/Documents/CapstoneProjectWithoutQt/ball3.mp4";
bool Config::mockScenario = false;

//Room
double Config::thresholdInitLightMock{ 50 };
int Config::numberFrameToEvaluateLightPose = 10;
std::string Config::basePathVideoLightInitialization = "C:/Users/brene/Documents/CapstoneProjectWithoutQt/light_";
std::string Config::formatVideoLightInitialization = ".mp4";
int Config::startFrame = 100;
int Config::endFrame = 150;
double Config::thresholdDiffImages{ 10 };
double Config::scaleMaxDistancePrevBarycentrePointForSamePoint{ 1 };
double Config::scaleMaxDistanceCurrentBarPrevPoint{ 8 };
int Config::totalIntensity{ 100 };

//Image Processor
float Config::maxThresholdFilterLightBrightness{ 245 };
int Config::thresholdFilterLightBrightness{ 250 };
float Config::rateFilterContourByShape{ 0.2 };
double Config::eraseRateFilterByPoints{ 0.5 };
double Config::eraseRateFilterByBoundingBoxSize{ 0.8 };