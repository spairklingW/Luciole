#ifndef ImageProcessor_H_
#define ImageProcessor_H_

#include <string>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/core.hpp>
#include <iostream>
#include <map>
#include <vector>
#include <numeric>
#include <string>
#include <functional>

using namespace cv;


class ImageProcessor
{

public:
    // constructors / destructors
    ImageProcessor();
    ImageProcessor(std::string filename);
    ~ImageProcessor();

	ImageProcessor& operator=(ImageProcessor&) = default;
	ImageProcessor(ImageProcessor&) = default;

	ImageProcessor& operator=(ImageProcessor&&) = default;
	ImageProcessor(ImageProcessor&&) = default;

	//methods

	void GetLightPositionFromHighestPixelsI(const Mat & srcImg, Mat& destImg, int indexFrameStream, int i, Point& lightPose);

	std::vector<Rect> GetBoundingBoxes() { return _biggestBoundingBoxes; };

	void ComputeDiffImages(const Mat & prevImage, const Mat & currentImage, Mat & diffImageFloat, Mat & diffImageFloatSquare);

	void ApplyThreshold(Mat & diffImageFloat, double thresh, Mat & diffImageThresholded);
	
	void GetContours(Mat & diffImageFloat, double thresh, Mat & diffImageThresholded, std::map<size_t, std::vector<Point>>& contoursSorted, std::map <size_t, std::vector<Point>, std::greater <int >>& contoursSortedDescendingOrder, std::vector<std::vector<Point> > &contours);

	void ComputeBoundingBoxFromContour(const std::vector<Point>& contour, Rect & boundRect);

	void GetContoursPeopleInRoom(std::map<size_t, std::vector<Point>>& contoursSorted, std::vector<std::vector<Point>>& contoursMovingInstances, std::vector<Rect>& biggestBoundingBoxes, std::map<size_t, std::vector<Point>, std::greater<int>>& contoursSortedDescendingOrder, const std::vector<std::vector<Point> >& contours);

	void GetPositionObjectsFromDiffImage(Mat && prevImage, Mat & currentImage, double threshold, std::vector<Point>& barycentresContours);


private:
	//private methods

	cv::Mat FilterBrightestAboveThreshold(const cv::Mat & src, float max_threshold, float& mean_X, float& mean_Y);

	cv::Mat FilterBrightest(const cv::Mat & src, int n, int & max_threshold);

	void FilterContoursByShape(std::map<size_t, std::vector<Point>>& biggestContoursSorted, float filterThresh, std::map<int, std::vector<Point>>& biggestContoursFiltered, std::vector<Rect>& biggestBBoxesFiltered);

	void ComputeBarycentresOfPeopleBoundingBoxes(const Mat & diffImage, const std::vector<Rect> & biggestBoundingBoxes, std::vector<Point>& barycentresContours);

	void ComputeBarycentresOfPeopleContours(const Mat & diffImage, std::vector<std::vector<Point>>& contoursMovingInstances, std::vector<Point>& barycentresContours);

	void DiscardSmallContoursByNumberOfPoint(std::map<size_t, std::vector<Point>, std::greater<int>>& contoursSorted, double eraseRate, std::map<size_t, std::vector<Point>>& BiggestContours);
	
	void DiscardSmallContoursByBoundingBoxSizeAfter(double eraseRate, std::map<int, std::vector<Point>>& BiggestContours, const std::vector<std::vector<Point>>& contours, std::vector<Rect>& boundingBoxes);
	
	void DiscardSmallContoursByBoundingBoxSize(double eraseRate, std::map<size_t, std::vector<Point>> & BiggestContours, const std::vector<std::vector<Point> >& contours, std::vector<Rect> & boundingBoxes);

	//private attributes
	std::vector<Rect> _biggestBoundingBoxes;
	double _meanImgLightOn = 0;
};

#endif /* ImageProcessor_H_ */