#include <iostream>
#include <random>
#include <algorithm>
#include <ctime>
#include "ImageProcessor.h"
#include "Room.h"


using namespace cv;


ImageProcessor::ImageProcessor()
{
}


ImageProcessor::ImageProcessor(std::string filename)
{
    std::cout << "ImageProcessor Constructor" << std::endl;
    
}

ImageProcessor::~ImageProcessor()
{
    std::cout << "ImageProcessor Destructor" << std::endl;
}


void ImageProcessor::GetLightPositionFromHighestPixelsI(const Mat & srcImg, Mat& destImg, int indexFrameStream, int i, Point& lightPose)
{
	int brightestpoint = 0;

	float meanX = 0;
	float meanY = 0;
	auto imageBright = FilterBrightestAboveThreshold(srcImg, Config::maxThresholdFilterLightBrightness, meanX, meanY);
	lightPose.x = meanY;
	lightPose.y = meanX;

	circle(imageBright, Point{ static_cast<int>(meanY), static_cast<int>(meanX) }, 10, Scalar(150, 140, 150), 1, 8, 0);
	namedWindow("Binary Image" + std::to_string(0), WINDOW_NORMAL);
	imshow("Binary Image" + std::to_string(0), imageBright);
}

cv::Mat ImageProcessor::FilterBrightestAboveThreshold(const cv::Mat& src, float max_threshold, float& mean_X, float& mean_Y) {

	// the light must be on before starting the video
	int maxValue;
	cv::Mat imageThresholded;
	imageThresholded = FilterBrightest(src, int((src.rows*src.cols) * .01), maxValue);

	maxValue = Config::thresholdFilterLightBrightness;
	std::vector<std::tuple<float, float>> pixelLocations;

	for (int x = 0; x < src.rows; x++) {
		for (int y = 0; y < src.cols; y++) {

			if (src.at<uchar>(x, y) >= static_cast<float>(maxValue)) {

				pixelLocations.emplace_back(std::make_pair(x, y));
			}
		}
	}

	float meanX = 0;
	float meanY = 0;

	for (int i = 0; i < pixelLocations.size(); i++) {
		meanX += std::get<0>(pixelLocations.at(i));
		meanY += std::get<1>(pixelLocations.at(i));
	}

	mean_X = meanX / pixelLocations.size();
	mean_Y = meanY / pixelLocations.size();

	return imageThresholded;
}

cv::Mat ImageProcessor::FilterBrightest(const cv::Mat& src, int n, int & max_threshold) {

	CV_Assert(src.channels() == 1);
	CV_Assert(src.type() == CV_8UC1);

	cv::Mat result = {};

	// simple histogram
	std::vector<int> histogram(256, 0);
	for (int i = 0; i< int(src.rows*src.cols); ++i)
		histogram[src.at<uchar>(i)]++;

	// find max threshold value (pixels from [0-max_threshold] will be removed)
	max_threshold = (int)histogram.size() - 1;
	for (; max_threshold >= 0 && n > 0; --max_threshold) {
		n -= histogram[max_threshold];
	}

	if (max_threshold < 0)  // nothing to do
		src.copyTo(result);
	else
		cv::threshold(src, result, max_threshold, 0., cv::THRESH_TOZERO);

	return result;
}

void ImageProcessor::ComputeDiffImages(const Mat & prevImage, const Mat & currentImage, Mat & diffImageFloat, Mat & diffImageFloatSquare)
{
	if (Room::displayAllImages == true)
	{
		namedWindow("image curr BC" + std::to_string(Room::indexDisplay), WINDOW_NORMAL);
		imshow("image curr BC" + std::to_string(Room::indexDisplay), currentImage);

		namedWindow("image prev BC" + std::to_string(Room::indexDisplay), WINDOW_NORMAL);
		imshow("image prev BC" + std::to_string(Room::indexDisplay), prevImage);
	}

	//I assume that all negative pixel are set to 0 : there is no negative value accepted for a Mat -> workaround : repeat the operation
	Mat diffImage1 = prevImage - currentImage;
	Mat diffImage2 = currentImage - prevImage;
	Mat diffImage = (diffImage1 + diffImage2)/2;

	if (Room::displayAllImages == true)
	{
		namedWindow("diffImageBC" + std::to_string(Room::indexDisplay), WINDOW_NORMAL);
		imshow("diffImageBC" + std::to_string(Room::indexDisplay), diffImage);
	}
	
	//Mat diffImageFiltered;
	//fastNlMeansDenoising(diffImage, diffImageFiltered, 3, 7, 21);
	
	diffImage.convertTo(diffImageFloat, CV_32F);

	diffImageFloatSquare = cv::Mat::zeros(diffImage.size(), CV_32F);

	for (int i = 0; i < diffImage.size().height; i++)
	{
		for (int j = 0; j < diffImage.size().width; j++)
		{
			diffImageFloatSquare.at<float>(i, j) = diffImageFloat.at<float>(i, j)*diffImageFloat.at<float>(i, j);
			//below not needed, nevertheless the square could help to enhance the contrasts
			diffImageFloat.at<float>(i, j) = sqrt(diffImageFloat.at<float>(i, j)*diffImageFloat.at<float>(i, j));
		}
	}
}

void ImageProcessor::ApplyThreshold(Mat & diffImageFloat, double thresh, Mat & diffImageThresholded)
{
	double thres = threshold(diffImageFloat, diffImageThresholded, thresh, 255, 0);
	if (Room::displayAllImages == true)
	{
		namedWindow("Image before thresholded" + std::to_string(Room::indexDisplay), WINDOW_NORMAL);
		imshow("Image before thresholded" + std::to_string(Room::indexDisplay), diffImageFloat);
		namedWindow("Image thresholded" + std::to_string(Room::indexDisplay), WINDOW_NORMAL);
		imshow("Image thresholded" + std::to_string(Room::indexDisplay), diffImageThresholded);
	}
}

void ImageProcessor::GetContours(Mat & diffImageFloat, double thresh, Mat & diffImageThresholded, std::map<size_t, std::vector<Point>> & contoursSorted, std::map <size_t, std::vector<Point>, std::greater <int >> & contoursSortedDescendingOrder, std::vector<std::vector<Point> >& contourss)
{
	Mat diffImageChar;
	diffImageFloat.convertTo(diffImageChar, CV_8UC1);

	Mat diffImageThresholdedChar;
	diffImageThresholded.convertTo(diffImageThresholdedChar, CV_8UC1);

	if (Room::displayAllImages == true)
	{
		namedWindow("diffImageT" + std::to_string(Room::indexDisplay), WINDOW_NORMAL);
		imshow("diffImageT" + std::to_string(Room::indexDisplay), diffImageThresholded);
	}

	Mat canny_output;
	Canny(diffImageChar, canny_output, thresh, thresh * 2);
	std::vector<std::vector<Point> > rawContours;
	std::vector<Vec4i> hierarchy;

	findContours(diffImageThresholdedChar, rawContours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

	Mat filledContours = Mat::zeros(diffImageFloat.size(), CV_8UC1);;
	RNG rng(12345);
	Scalar color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
	std::vector<std::vector<Point> > singleContourContainer;

	for (auto contour : rawContours)
	{
		singleContourContainer.emplace_back(contour);
		drawContours(filledContours, singleContourContainer, -1, 255, -1);
		singleContourContainer.clear();
	}

	//loop over the contour to draw the image one by one and then add all images and then use threashold
	if (Room::displayAllImages == true)
	{
		namedWindow("Filled Contours" + std::to_string(Room::indexDisplay), WINDOW_NORMAL);
		imshow("Filled Contours" + std::to_string(Room::indexDisplay), filledContours);
	}

	std::vector<std::vector<Point> > contoursFromFilledShapes;
	std::vector<Vec4i> hierarchy2;
	findContours(filledContours, contoursFromFilledShapes, hierarchy2, RETR_TREE, CHAIN_APPROX_SIMPLE);

	Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);

	for (int i = 0; i < contoursFromFilledShapes.size(); i++)
	{
		//TODO :: more robust algo, insert does not perform if the key is not unique, in case of contour of same size, the latter is not inserted in the map
		contoursSorted.emplace(contoursFromFilledShapes.at(i).size(), contoursFromFilledShapes.at(i));
		contoursSortedDescendingOrder.emplace(contoursFromFilledShapes.at(i).size(), contoursFromFilledShapes.at(i));
		contourss.emplace_back(contoursFromFilledShapes.at(i));
	}
}

void ImageProcessor::DiscardSmallContoursByNumberOfPoint(std::map<size_t, std::vector<Point>, std::greater<int>> & contoursSorted, double eraseRate, std::map<size_t, std::vector<Point>> & BiggestContours)
{
	std::map <size_t, std::vector<Point>, std::greater <int >> contoursSortedDescendingOrder;

	//auto sizeBiggestContour = prev(contoursSorted.end())->first;
	//problem : some contours even small in size have a lot of point !! Sort differently
	if (!contoursSorted.empty())
	{
		auto sizeBiggestContour = contoursSorted.begin()->first;
		for (auto contourSorted : contoursSorted)
		{
			if (static_cast<double>(contourSorted.first) > eraseRate*static_cast<double>(sizeBiggestContour))
			{
				BiggestContours.emplace(contourSorted);
			}
			else
			{
				break;
			}
		}
	}
}

void ImageProcessor::DiscardSmallContoursByBoundingBoxSizeAfter(double eraseRate, std::map<int, std::vector<Point>> & BiggestContours, const std::vector<std::vector<Point> >& contours, std::vector<Rect> & boundingBoxes)
{
	std::map<size_t, std::vector<Point>, std::greater<int>> contoursSorted;
	std::map<size_t, Rect, std::greater<int>> boundingBoxSorted;
	std::vector<std::vector<Point> > contours_poly(contours.size());
	std::vector<Rect> boundRect(contours.size());
	std::vector<Point2f>centers(contours.size());
	std::vector<float>radius(contours.size());
	RNG rng(12345);

	for (size_t i = 0; i < contours.size(); i++)
	{
		approxPolyDP(contours[i], contours_poly[i], 3, true);
		boundRect[i] = boundingRect(contours_poly[i]);
		minEnclosingCircle(contours_poly[i], centers[i], radius[i]);

		contoursSorted.emplace(static_cast<size_t>(boundRect[i].height) + static_cast<size_t>(boundRect[i].width), contours.at(i));
		boundingBoxSorted.emplace(static_cast<size_t>(boundRect[i].height) + static_cast<size_t>(boundRect[i].width), boundRect[i]);
	}

	Mat drawing = Mat::zeros(Room::baseSizeImage, CV_8UC3);

	for (size_t i = 0; i < contours.size(); i++)
	{
		Scalar color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
		drawContours(drawing, contours_poly, (int)i, color);
		rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2);
		//circle(drawing, centers[i], (int)radius[i], color, 2);
	}

	if (Room::displayAllImages == true)
	{
		namedWindow("Bounding Boxes" + std::to_string(Room::indexDisplay), WINDOW_NORMAL);
		imshow("Bounding Boxes" + std::to_string(Room::indexDisplay), drawing);
	}

	//case the first boudning box is the frame itself
	if (!boundingBoxSorted.empty() && boundingBoxSorted.begin()->first > 0.9*(Room::heightImage + Room::widthImage))
	{
		boundingBoxSorted.erase(boundingBoxSorted.begin());
		contoursSorted.erase(contoursSorted.begin());
	}

	int i = 0;
	if (!contoursSorted.empty())
	{
		auto sizeBiggestContour = contoursSorted.begin()->first;
		for (auto contourSorted : contoursSorted)
		{
			if (static_cast<double>(contourSorted.first) > eraseRate*static_cast<double>(sizeBiggestContour))
			{
				BiggestContours.emplace(i, contourSorted.second);
			}
			else
			{
				break;
			}
		}
		i++;
	}

	if (!boundingBoxSorted.empty())
	{
		auto sizeBiggestContour = boundingBoxSorted.begin()->first;
		for (auto boundingBox : boundingBoxSorted)
		{
			if (static_cast<double>(boundingBox.first) > eraseRate*static_cast<double>(sizeBiggestContour))
			{
				boundingBoxes.emplace_back(boundingBox.second);
			}
			else
			{
				break;
			}
		}
	}
}

void ImageProcessor::DiscardSmallContoursByBoundingBoxSize(double eraseRate, std::map<size_t, std::vector<Point>> & BiggestContours, const std::vector<std::vector<Point> >& contours, std::vector<Rect> & boundingBoxes)
{
	std::map<size_t, std::vector<Point>, std::greater<int>> contoursSorted;
	std::map<size_t, Rect, std::greater<int>> boundingBoxSorted;
	std::vector<std::vector<Point> > contours_poly(contours.size());
	std::vector<Rect> boundRect(contours.size());
	std::vector<Point2f>centers(contours.size());
	std::vector<float>radius(contours.size());
	RNG rng(12345);

	for (size_t i = 0; i < contours.size(); i++)
	{
		approxPolyDP(contours[i], contours_poly[i], 3, true);
		boundRect[i] = boundingRect(contours_poly[i]);
		minEnclosingCircle(contours_poly[i], centers[i], radius[i]);

		contoursSorted.emplace(static_cast<size_t>(boundRect[i].height) + static_cast<size_t>(boundRect[i].width), contours.at(i));
		boundingBoxSorted.emplace(static_cast<size_t>(boundRect[i].height) + static_cast<size_t>(boundRect[i].width), boundRect[i]);
	}

	Mat drawing = Mat::zeros(Room::baseSizeImage, CV_8UC3);

	for (size_t i = 0; i < contours.size(); i++)
	{
		Scalar color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
		drawContours(drawing, contours_poly, (int)i, color);
		rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2);
	}

	if (Room::displayAllImages == true)
	{
		namedWindow("Bounding Boxes" + std::to_string(Room::indexDisplay), WINDOW_NORMAL);
		imshow("Bounding Boxes" + std::to_string(Room::indexDisplay), drawing);
	}
	
	//case the first boudning box is the frame itself
	if (!boundingBoxSorted.empty() && boundingBoxSorted.begin()->first > 0.9*(Room::heightImage + Room::widthImage))
	{
		boundingBoxSorted.erase(boundingBoxSorted.begin());
		contoursSorted.erase(contoursSorted.begin());
	}

	if (!contoursSorted.empty())
	{
		auto sizeBiggestContour = contoursSorted.begin()->first;
		for (auto contourSorted : contoursSorted)
		{
			if (static_cast<double>(contourSorted.first) > eraseRate*static_cast<double>(sizeBiggestContour))
			{
				BiggestContours.emplace(contourSorted);
			}
			else
			{
				break;
			}
		}
	}

	if (!boundingBoxSorted.empty())
	{
		auto sizeBiggestContour = boundingBoxSorted.begin()->first;
		for (auto boundingBox : boundingBoxSorted)
		{
			if (static_cast<double>(boundingBox.first) > eraseRate*static_cast<double>(sizeBiggestContour))
			{
				boundingBoxes.emplace_back(boundingBox.second);
			}
			else
			{
				break;
			}
		}
	}
}

void ImageProcessor::ComputeBoundingBoxFromContour(const std::vector<Point> & contour, Rect& boundRect)
{
	std::vector<Point> contours_poly;
	approxPolyDP(contour, contours_poly, 3, true);
	boundRect = boundingRect(contours_poly);
}

//https://gieseanw.wordpress.com/2017/05/03/a-true-heterogeneous-container-in-c/
void ImageProcessor::FilterContoursByShape(std::map<size_t, std::vector<Point>> & biggestContoursSorted, float filterThresh, std::map<int, std::vector<Point>>& biggestContoursFiltered, std::vector<Rect>& biggestBBoxesFiltered)
{
	
	Mat filledContours = Mat::zeros(Room::baseSizeImage, CV_8UC1);;
	std::vector<std::vector<Point> > singleContourContainer;
	int i = 0;

	for (const auto & contour : biggestContoursSorted)
	{
		singleContourContainer.emplace_back(contour.second);
		drawContours(filledContours, singleContourContainer, -1, 255, -1);
		int sizeOfFilledContour = countNonZero(filledContours);
		Rect boundingBox;
		
		ComputeBoundingBoxFromContour(contour.second, boundingBox);

		if ((boundingBox.height + boundingBox.width) < 0.9*(Room::heightImage + Room::widthImage))
		{
			if (static_cast<float>(sizeOfFilledContour) > static_cast<float>(filterThresh*boundingBox.area()))
			{
				biggestContoursFiltered.emplace(i, contour.second);
				biggestBBoxesFiltered.emplace_back(boundingBox);
			};
		}

		singleContourContainer.clear();
		i++;
		
	}
}

void ImageProcessor::GetContoursPeopleInRoom(std::map<size_t, std::vector<Point>> & contoursSorted, std::vector<std::vector<Point>> & contoursMovingInstances, std::vector<Rect>& biggestBBoxesFiltered, std::map<size_t, std::vector<Point>, std::greater<int>>& contoursSortedDescendingOrder, const std::vector<std::vector<Point> >& contours)
{
	int numberMovingInstancesLoop;

	numberMovingInstancesLoop = contoursSortedDescendingOrder.size();

	double eraseRateFilterByPoints = 0.5;
	std::map<size_t, std::vector<Point>> biggestContours;
	
	std::vector<Rect> biggestBoundingBoxes;
	DiscardSmallContoursByBoundingBoxSize(Config::eraseRateFilterByBoundingBoxSize, biggestContours, contours, biggestBoundingBoxes);

	std::map<int, std::vector<Point>> biggestContoursFiltered;
	//biggestBBoxesFiltered = biggestBoundingBoxes;
	//std::vector<Rect> biggestBBoxesFiltered;

	FilterContoursByShape(biggestContours, Config::rateFilterContourByShape, biggestContoursFiltered, biggestBBoxesFiltered);
	
	/*
	float filterThresh = 0.3;
	std::vector<Rect> biggestBoundingBoxes;
	std::map<int, std::vector<Point>> biggestContoursFiltered;
	FilterContoursByShape(contoursSorted, filterThresh, biggestContoursFiltered, biggestBoundingBoxes);

	DiscardSmallContoursByBoundingBoxSizeAfter(eraseRateFilterByBoundingBoxSize, biggestContoursFiltered, contours, biggestBBoxesFiltered);

	for (auto biggestContour : biggestContoursFiltered)
	{
		contoursMovingInstances.emplace_back(biggestContour.second);
	}
	*/
	//DiscardSmallContoursByNumberOfPoint(contoursSortedDescendingOrder, eraseRateFilterByPoints, biggestContours);

	for (auto biggestContour : biggestContours)
	{
		contoursMovingInstances.emplace_back(biggestContour.second);
	}
	
}

void ImageProcessor::ComputeBarycentresOfPeopleBoundingBoxes(const Mat & diffImage, const std::vector<Rect> & biggestBoundingBoxes, std::vector<Point>& barycentresContours)
{
	RNG rng(12345);

	Mat drawing = Mat::zeros(diffImage.size(), CV_8UC3);
	std::vector<Vec4i> hierarchy;

	for (int i = 0; i < biggestBoundingBoxes.size(); i++)
	{
		Scalar color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
		rectangle(drawing, biggestBoundingBoxes[i].tl(), biggestBoundingBoxes[i].br(), color, 2);
		cv::Point zero(0, 0);
		auto boundingBox = biggestBoundingBoxes.at(i);
		Point centerBoundingBox = Point(boundingBox.x + boundingBox.width / 2, boundingBox.y + boundingBox.height / 2);
		barycentresContours.emplace_back(centerBoundingBox);
		circle(drawing, centerBoundingBox, 10, Scalar(150, 140, 150), 1, 8, 0);
	}

	if (Room::displayAllImages == true)
	{
		namedWindow("Bounding Boxes centers" + std::to_string(Room::indexDisplay), WINDOW_NORMAL);
		imshow("Bounding Boxes centers" + std::to_string(Room::indexDisplay), drawing);
	}
}

void ImageProcessor::ComputeBarycentresOfPeopleContours(const Mat & diffImage, std::vector<std::vector<Point>> &contoursMovingInstances, std::vector<Point>& barycentresContours)
{
	RNG rng(12345);

	Mat drawing = Mat::zeros(diffImage.size(), CV_8UC3);
	std::vector<Vec4i> hierarchy;

	for (int i = 0; i < contoursMovingInstances.size(); i++)
	{
		Scalar color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
		drawContours(drawing, contoursMovingInstances, (int)i, color, 2, LINE_8, hierarchy, 0);
		cv::Point zero(0, 0);
		Point sum = std::accumulate(contoursMovingInstances.at(i).begin(), contoursMovingInstances.at(i).end(), zero) / (static_cast<int>(contoursMovingInstances.at(i).size()));
		barycentresContours.emplace_back(sum);
		circle(drawing, sum, 10, Scalar(150, 140, 150), 1, 8, 0);
	}

	if (Room::displayAllImages == true)
	{
		namedWindow("Contours" + std::to_string(Room::indexDisplay), WINDOW_NORMAL);
		imshow("Contours" + std::to_string(Room::indexDisplay), drawing);
	}
}

void ImageProcessor::GetPositionObjectsFromDiffImage(Mat && prevImage, Mat & currentImage, double threshold, std::vector<Point>& barycentresContours)
{
	Mat diffImage;
	Mat diffImageSquare;
	ComputeDiffImages(prevImage, currentImage, diffImage, diffImageSquare);

	if (Room::displayAllImages == true)
	{
		namedWindow("diffImageBT" + std::to_string(Room::indexDisplay), WINDOW_NORMAL);
		imshow("diffImageBT" + std::to_string(Room::indexDisplay), diffImage);
	}

	Mat imageThresholded;

	ApplyThreshold(diffImage, threshold, imageThresholded);

	std::map<size_t, std::vector<Point>> contoursSorted;
	std::map<size_t, std::vector<Point>, std::greater<int>> contoursSortedDescendingOrder;
	std::vector<std::vector<Point> > contours;

	if (Room::displayAllImages == true)
	{
		namedWindow("img Th" + std::to_string(Room::indexDisplay), WINDOW_NORMAL);
		imshow("img Th" + std::to_string(Room::indexDisplay), imageThresholded);
	}

	GetContours(diffImage, threshold, imageThresholded, contoursSorted, contoursSortedDescendingOrder, contours);
	std::vector<std::vector<Point>> contoursMovingInstances;
	std::vector<Rect> biggestBoundingBoxes;

	GetContoursPeopleInRoom(contoursSorted, contoursMovingInstances, biggestBoundingBoxes, contoursSortedDescendingOrder, contours);

	//ComputeBarycentresOfPeopleContours(diffImage, contoursMovingInstances, barycentresContours, index);
	
	ComputeBarycentresOfPeopleBoundingBoxes(diffImage, biggestBoundingBoxes, barycentresContours);
	_biggestBoundingBoxes = biggestBoundingBoxes;
}

