#ifndef ImageParser_H_
#define ImageParser_H_

#include <string>
#include <iostream>
#include <random>
#include <algorithm>
#include <ctime>
#include <chrono>
#include <thread>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/core.hpp>
#include "Config.h"
#include <condition_variable>
//#include "Room.h"

using namespace cv;

template<class T>
class MessageQueue
{
public:
	MessageQueue();

	void send(T&& msg);
	T receive();
	void clear();

private:
	//std::deque<T> _queue;
	std::queue<T> _queue;
	std::condition_variable _cond;
	std::mutex _mutexQueue;
};


class ImageParser
{

public:
    // constructors / destructors
    ImageParser();
    ImageParser(std::string filename); 
    ~ImageParser();

	ImageParser& operator=(ImageParser&) = default;
	ImageParser(ImageParser&) = default;

	ImageParser& operator=(ImageParser&&) = default;
	ImageParser(ImageParser&&) = default;

	// methods for recording

	Mat getFrameFromCameraQueue();

	void startCameraRecord(const std::string & pathToVideo);

	void joinRunningFrame();

	void clearQueue();

	void cameraStream(const std::string & pathToVideo);

	static Mat InitializeFrameWithoutMotions(const std::string & pathToVideo);

	bool IsCameraInActiveState() { return _cameraGetNewFrames; };

	// getters / setters
	std::vector<Mat> Get() { return _imagesFromCamera; };

private:
	MessageQueue<Mat> _cameraQueue;
	std::vector<Mat> _imagesFromCamera;
	Mat _emptyImage;
	Mat _refImageWithoutMotion;
	std::thread _threadRecordFrames;
	bool _cameraGetNewFrames = true;
};

#endif 
