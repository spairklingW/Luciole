#include <iostream>
#include <random>
#include <algorithm>
#include <ctime>
#include "ImageParser.h"


ImageParser::ImageParser()
{
}

ImageParser::ImageParser(std::string filename)
{
    std::cout << "ImageParser Constructor" << std::endl;
    
}

ImageParser::~ImageParser()
{
    std::cout << "ImageParser Destructor" << std::endl;

	_threadRecordFrames.join();
}

Mat ImageParser::getFrameFromCameraQueue()
{
	return _cameraQueue.receive();
}

void ImageParser::startCameraRecord(const std::string & pathToVideo)
{
	_threadRecordFrames = std::thread(&ImageParser::cameraStream, this, pathToVideo);
}

void ImageParser::joinRunningFrame()
{
	_threadRecordFrames.join();
}

void ImageParser::clearQueue()
{
	_cameraQueue.clear();
}

void ImageParser::cameraStream(const std::string & pathToVideo)
{
	VideoCapture cap(pathToVideo);

	// Check if camera opened successfully
	if (!cap.isOpened()) {
		std::cout << "Error opening video stream or file" << std::endl;
	}

	std::vector<cv::Mat> imageContainer;

	int count = 0;

	while (1) {

		Mat frame;
		// Capture frame-by-frame
		cap >> frame;

		// If the frame is empty, break immediately
		if (frame.empty())
			break;

		Mat frameDisplay = frame.clone();
		//imageContainer.emplace_back(frame);

		_cameraQueue.send(std::move(frame));

		count++;
	}
	_cameraGetNewFrames = false;

	// When everything done, release the video capture object
	cap.release();

	//destroyAllWindows();
}

Mat ImageParser::InitializeFrameWithoutMotions(const std::string & pathToVideo)
{
	VideoCapture cap(pathToVideo);

	// Check if camera opened successfully
	if (!cap.isOpened()) {
		std::cout << "Error opening video stream or file" << std::endl;
	}

	Mat frame;
	cap >> frame;

	// If the frame is empty, break immediately
	// TODO : handle the case the frame is empty
	/*
	if (frame.empty())
		return;
	*/

	cap.release();

	return frame;
}


template<class T>
MessageQueue<T>::MessageQueue()
{
}

template <class T>
T MessageQueue<T>::receive()
{
	// perform queue modification under the lock
	std::unique_lock<std::mutex> uLock(_mutexQueue);
	_cond.wait(uLock, [this] { return !_queue.empty(); }); // pass unique lock to condition variable

	// remove last vector element from queue
	//T msg = std::move(_queue.back());
	//_queue.pop_back();

	T msg = std::move(_queue.front());
	_queue.pop();

	std::cout << _queue.size() << "\n";

	return msg; // will not be copied due to return value optimization (RVO) in C++
}

template<class T>
void MessageQueue<T>::clear()
{
	std::queue<T> empty;
	std::swap(_queue, empty);
}

template <class T>
void MessageQueue<T>::send(T &&msg)
{
	// perform vector modification under the lock
	std::lock_guard<std::mutex> uLock(_mutexQueue);

	// add vector to queue
	std::cout << "before send" << _queue.size()<<"\n";
	_queue.push(std::move(msg));
	//_queue.push_back(std::move(msg));
	_cond.notify_one(); // notify client after pushing new Vehicle into vector

	std::cout << "after send" << _queue.size() << "\n";
}


