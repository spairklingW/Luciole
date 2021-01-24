#include <iostream>
#include "Config.h"
#include "JsonParser.h"

//Image Parser Mock
int Config::numberLightSources = JsonParser::ParseIntParamFromJsonContainer("numberLightSources");
int Config::numberImagesParserMock = JsonParser::ParseIntParamFromJsonContainer("numberImagesParserMock");
std::string Config::basePathImg = JsonParser::ParseStringParamFromJsonContainer("basePathImg");
std::string Config::formatImgFile = JsonParser::ParseStringParamFromJsonContainer("formatImgFile");
std::string Config::pathImgRef = JsonParser::ParseStringParamFromJsonContainer("pathImgRef");

//DisplayImage
std::string Config::pathToVideo = JsonParser::ParseStringParamFromJsonContainer("pathToVideo");
bool Config::mockScenario = JsonParser::ParseBoolParamFromJsonContainer("mockScenario");

//Room
double Config::thresholdInitLightMock{ JsonParser::ParseDoubleParamFromJsonContainer("thresholdInitLightMock")};
int Config::numberFrameToEvaluateLightPose{ JsonParser::ParseIntParamFromJsonContainer("numberFrameToEvaluateLightPose") };
std::string Config::basePathVideoLightInitialization = JsonParser::ParseStringParamFromJsonContainer("basePathVideoLightInitialization");
std::string Config::formatVideoLightInitialization = JsonParser::ParseStringParamFromJsonContainer("formatVideoLightInitialization");
int Config::startFrame{ JsonParser::ParseIntParamFromJsonContainer("startFrame") };
int Config::endFrame{ JsonParser::ParseIntParamFromJsonContainer("endFrame") };
double Config::thresholdDiffImages{ JsonParser::ParseDoubleParamFromJsonContainer("thresholdDiffImages") };
double Config::scaleMaxDistancePrevBarycentrePointForSamePoint{ JsonParser::ParseDoubleParamFromJsonContainer("scaleMaxDistancePrevBarycentrePointForSamePoint") };
double Config::scaleMaxDistanceCurrentBarPrevPoint{ JsonParser::ParseDoubleParamFromJsonContainer("scaleMaxDistanceCurrentBarPrevPoint") };
int Config::totalIntensity{ JsonParser::ParseIntParamFromJsonContainer("totalIntensity") };
bool Config::useQueueBufferFrame{ JsonParser::ParseBoolParamFromJsonContainer("useQueueBufferFrame") };

//Image Processor
float Config::maxThresholdFilterLightBrightness{ JsonParser::ParseFloatParamFromJsonContainer("maxThresholdFilterLightBrightness") };
int Config::thresholdFilterLightBrightness{ JsonParser::ParseIntParamFromJsonContainer("thresholdFilterLightBrightness") };
float Config::rateFilterContourByShape{ JsonParser::ParseFloatParamFromJsonContainer("rateFilterContourByShape") };
double Config::eraseRateFilterByPoints{ JsonParser::ParseDoubleParamFromJsonContainer("eraseRateFilterByPoints") };
double Config::eraseRateFilterByBoundingBoxSize{ JsonParser::ParseDoubleParamFromJsonContainer("eraseRateFilterByBoundingBoxSize") };