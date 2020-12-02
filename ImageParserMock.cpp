#include <iostream>
#include <random>
#include <algorithm>
#include <ctime>
#include "ImageParserMock.h"
#include "Config.h"

ImageParserMock::ImageParserMock()
{
}

ImageParserMock::ImageParserMock(std::string filename)
{
    std::cout << "ImageParserMock Constructor" << std::endl;
}

ImageParserMock::~ImageParserMock()
{
    std::cout << "ImageParserMock Destructor" << std::endl;

	_threadRecordFrames.join();
}

void ImageParserMock::FillContainerWithImages()
{
	_imagesFromCamera.emplace_back(imread(Config::pathImgRef, IMREAD_GRAYSCALE));

	for (int i = 1; i < Config::numberImagesParserMock; i++)
	{
		std::string pathImageToLoad = Config::basePathImg + std::to_string(i) + Config::formatImgFile;
		std::cout << " image" << pathImageToLoad << "\n";
		_imagesFromCamera.emplace_back(imread(pathImageToLoad, IMREAD_GRAYSCALE));

		namedWindow("image" + std::to_string(i), WINDOW_NORMAL);
		imshow("image" + std::to_string(i), imread(pathImageToLoad));
	}
}


Mat ImageParserMock::InitializeFrameWithoutMotionsMock()
{
	return imread(Config::pathImgRef, IMREAD_GRAYSCALE);
}



