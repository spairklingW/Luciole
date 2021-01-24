#include <iostream>
#include <random>
#include <algorithm>
#include <ctime>
#include "Room.h"
#include <chrono>
#include <thread>
#include <tuple>

using namespace cv;
int Room::indexDisplay = 0;
bool Room::displayAllImages = false;
Size Room::baseSizeImage = Size(10, 10);
int Room::heightImage = 0;
int Room::widthImage = 0;


Room::Room()
{
}

Room::Room(std::string filename)
{
    std::cout << "Room Constructor" << std::endl;
    
}

Room::Room(int numberLightSources, std::shared_ptr<RenderingSimulator> renderingSimulator, Mat refImage)
{
	_lightSources = std::vector<LightSource>(numberLightSources);
	Room::baseSizeImage = refImage.size();

	for (auto & lightSource : _lightSources)
	{
		lightSource.Set(renderingSimulator);
	}
}

Room::~Room()
{
    std::cout << "Room Destructor" << std::endl;
}

void Room::InitializeRefImageMock()
{
	_refImageWithoutLightSource = _imageParserMock.InitializeFrameWithoutMotionsMock();
}

void Room::ModelInitializationLightSources()
{
	InitializeRefImageMock();

	int idxLightSource = 0;

	for (auto & lightSource : _lightSources)
	{
		std::vector<Point> barycentresContours;
		
		_lightSources.at(0).GetHardwareSimulator()->GetHWRendering() = std::make_shared<Mat>(_refImageWithoutLightSource.clone());
		lightSource.GenerateLightPosition(idxLightSource, _lightSources.size());
		lightSource.Set(idxLightSource);
		
		Mat imageLigthOn = *(_lightSources.at(idxLightSource).GetHardwareSimulator()->GetHWRendering());
		
		_imageProcessor.GetPositionObjectsFromDiffImage(_refImageWithoutLightSource.clone(), imageLigthOn, Config::thresholdInitLightMock, barycentresContours);
		
		if (Room::displayAllImages == true)
		{
			namedWindow("HWRender" + std::to_string(idxLightSource + 1), WINDOW_NORMAL);
			imshow("HWRender" + std::to_string(idxLightSource + 1), *(_lightSources.at(idxLightSource).GetHardwareSimulator()->GetHWRendering()));
		}
		
		if (barycentresContours.size() > 0)
		{
			_lightSources.at(idxLightSource).Set(barycentresContours.at(0));
		}
		else
		{
			std::cout << "no contours detected for specific light source, impossible to initialize position";
		}

		idxLightSource++;
	}

	JsonParser::UpdateLightsIntensityOnJsonFile(_lightSources);
}

void Room::InitializeMockImages()
{
	_imageParserMock.FillContainerWithImages();
}

void Room::InitializeFrameWithoutMotion()
{
	_imageParserMock.FillContainerWithImages();
}

void Room::InitializePositionMovingInstancesMock()
{
	//_personInRoomPrevImage.emplace_back(1099, 1044);
}

void Room::ProcessMovingInstances(std::unique_ptr<Mat>&& prevImageCamera, std::unique_ptr<Mat>&& currentImageFromCamera, Mat& renderingPositions, int index)
{
	_personsInRoom.clear();
	ComputePositionMovingAndImmobilInstances(std::move(prevImageCamera), std::move(currentImageFromCamera), renderingPositions);
	UpdateLightsIntensity();
	_personInRoomPrevImage = _personsInRoom;
}

void Room::DefineStopConditionForInitialization(int i, int indexFrameStream, bool &continueStreaming)
{
	if (i == 0 && indexFrameStream == Config::numberFrameToEvaluateLightPose)
	{
		continueStreaming = false;
	}
	if (i == 1 && indexFrameStream == Config::numberFrameToEvaluateLightPose)
	{
		continueStreaming = false;
	}
	if (i == 2 && indexFrameStream == Config::numberFrameToEvaluateLightPose)
	{
		continueStreaming = false;
	}
	if (i == 3 && indexFrameStream == Config::numberFrameToEvaluateLightPose)
	{
		continueStreaming = false;
	}
}

void Room::InitializeLightSourcesFromRealScene()
{

	for (int i = 0; i < _lightSources.size(); i++)
	{
		const std::string pathToVideo = Config::basePathVideoLightInitialization + std::to_string(i) + Config::formatVideoLightInitialization;
		_imageParser.startCameraRecord(pathToVideo);

		int indexFrameStream = 0;

		bool continueStreaming = true;

		while (continueStreaming)
		{
			indexFrameStream++;
			if (indexFrameStream < 1)
			{
				Mat currentFrame = _imageParser.getFrameFromCameraQueue();
			}
			else/* if (indexFrameStream > 49 && indexFrameStream < 120)*/ {
				Mat currentFrame = _imageParser.getFrameFromCameraQueue();

				Mat curgrayscaleFrame;
				Mat curgrayscaleFrameChar;
				cvtColor(currentFrame.clone(), curgrayscaleFrame, COLOR_BGR2GRAY);
				curgrayscaleFrame.convertTo(curgrayscaleFrameChar, CV_8UC1);

				std::unique_ptr<Mat> currentImage = std::make_unique<Mat>(curgrayscaleFrameChar.clone());

				namedWindow("current Image Cyclic" + std::to_string(indexFrameStream)+ "i" + std::to_string(i), WINDOW_NORMAL);
				imshow("current Image Cyclic" + std::to_string(indexFrameStream) + "i" + std::to_string(i), *currentImage);

				Mat imageThreasholded;
				Point lightPose;
				_imageProcessor.GetLightPositionFromHighestPixelsI(*currentImage, imageThreasholded, indexFrameStream, i, lightPose);

				_lightSources.at(i).Set(i);
				_lightSources.at(i).Set(lightPose);

				DefineStopConditionForInitialization(i, indexFrameStream, continueStreaming);
			}
		}
		_imageParser.joinRunningFrame();
		_imageParser.clearQueue();
	}

	JsonParser::UpdateLightsIntensityOnJsonFile(_lightSources);
}

Mat Room::ConvertFrameToGreyChar(Mat & imagesrc)
{
	Mat curgrayscaleFrame;
	Mat curgrayscaleFrame32F;

	cvtColor(imagesrc, curgrayscaleFrame, COLOR_BGR2GRAY);
	curgrayscaleFrame.convertTo(curgrayscaleFrame32F, CV_8UC1);

	return curgrayscaleFrame32F;
}

void Room::CyclicUpdateLightsFromImages(const std::string & pathToVideo)
{
	_refImageWithoutMotion = ImageParser::InitializeFrameWithoutMotions(pathToVideo);

	Room::heightImage = _refImageWithoutMotion.size().height;
	Room::widthImage = _refImageWithoutMotion.size().width;

	startRecording(pathToVideo);

	Mat renderingPositions = Mat::zeros(_refImageWithoutMotion.size(), CV_32F);
	_previousImage = _refImageWithoutMotion.clone();
	
	while (true /*_imageParser.IsCameraInActiveState() == true*//* Room::indexDisplay < 10*/)
	{
		//if (_imageParser.IsCameraInActiveState() == true)
		//{
		Room::indexDisplay++;
		if (Room::indexDisplay < Config::startFrame)
		{
			getFrame();
		}
		else{
			getFrame();
			std::unique_ptr<Mat> prevImage = std::make_unique<Mat>(ConvertFrameToGreyChar(_previousImage));
			std::unique_ptr<Mat> currentImage = std::make_unique<Mat>(ConvertFrameToGreyChar(_currentImage));

			if (Room::displayAllImages == true)
			{
				namedWindow("prev Image Cyclic" + std::to_string(indexDisplay), WINDOW_NORMAL);
				imshow("prev Image Cyclic" + std::to_string(indexDisplay), *prevImage);

				namedWindow("current Image Cyclic" + std::to_string(indexDisplay), WINDOW_NORMAL);
				imshow("current Image Cyclic" + std::to_string(indexDisplay), *currentImage);
			}

			ProcessMovingInstances(std::move(prevImage), std::move(currentImage), renderingPositions, Room::indexDisplay);

			_previousImage = _currentImage;

			if (Room::indexDisplay == Config::endFrame)
			{
				break;
			}
		}
	}
	if (Config::useQueueBufferFrame == false)
	{
		_imageParser.releaseCap();
	}
}

void Room::getFrame()
{
	if (Config::useQueueBufferFrame == true)
	{
		_currentImage = _imageParser.getFrameFromCameraQueue();
	}
	else
	{
		_imageParser.getLastImageFromCameraStream(_currentImage);
	}
}

void Room::startRecording(const std::string & pathToVideo)
{
	if (Config::useQueueBufferFrame == true)
	{
		_imageParser.startCameraRecord(pathToVideo);
	}
	else
	{
		_imageParser.startCapture(pathToVideo);
	}
}

void Room::CyclicUpdateLightsFromMockImages()
{
	auto containerImages = _imageParserMock.Get();
	Mat renderingPositions = Mat::zeros(containerImages.begin()->size(), CV_32F);

	Room::heightImage = renderingPositions.size().height;
	Room::widthImage = renderingPositions.size().width;

	for (int i = 0; i < containerImages.size() - 1; i++)
	{
		indexDisplay++;
		_currentImage = Mat::zeros(containerImages.at(i + 1).size(), CV_32F);
		_currentImage = containerImages.at(i + 1);
		  
		std::unique_ptr<Mat> prevImage = std::make_unique<Mat>(containerImages.at(i));
		std::unique_ptr<Mat> currentImage = std::make_unique<Mat>(containerImages.at(i + 1));

		if (Room::displayAllImages == true)
		{
			namedWindow("prev Image Cyclic" + std::to_string(indexDisplay), WINDOW_NORMAL);
			imshow("prev Image Cyclic" + std::to_string(indexDisplay), *prevImage);

			namedWindow("current Image Cyclic" + std::to_string(indexDisplay), WINDOW_NORMAL);
			imshow("current Image Cyclic" + std::to_string(indexDisplay), *currentImage);
		}

		ProcessMovingInstances(std::move(prevImage), std::move(currentImage), renderingPositions, i);
	}
}

Point Room::EraseInBarycentresClosestBarycentreFromPersonInRoom(std::vector<cv::Point> &barycentresContoursMovingInstances, std::map<double, cv::Point> &distancesBarycentresToPosition)
{
	auto it1 = std::find(barycentresContoursMovingInstances.begin(), barycentresContoursMovingInstances.end(), distancesBarycentresToPosition.begin()->second);
	int index1 = std::distance(barycentresContoursMovingInstances.begin(), it1);

	Point barycentrePotentialLeavingPerson = barycentresContoursMovingInstances.at(index1);

	barycentresContoursMovingInstances.erase(barycentresContoursMovingInstances.begin() + index1);

	return barycentrePotentialLeavingPerson;
}

void Room::DrawCircleOfPosition(cv::Mat & renderingPositions, const cv::Point & pointDistancesBarycentresToPosition, cv::Mat &iRenderingPositions)
{
	int radiuscircle = 10;
	circle(renderingPositions, pointDistancesBarycentresToPosition, radiuscircle, Scalar(150, 140, 150), 1, 8, 0);
	circle(iRenderingPositions, pointDistancesBarycentresToPosition, radiuscircle, Scalar(150, 140, 150), 1, 8, 0);
}

void Room::AddAllDetectedPersonsToPersonsInRoom(std::vector<cv::Point> &immobilPersons, std::vector<cv::Point> &personsEnteringRoom, std::vector<cv::Point> &personsMovingInRoom)
{
	_personsInRoom.clear();
	_personsInRoom.insert(_personsInRoom.end(), immobilPersons.begin(), immobilPersons.end());
	_personsInRoom.insert(_personsInRoom.end(), personsEnteringRoom.begin(), personsEnteringRoom.end());
	_personsInRoom.insert(_personsInRoom.end(), personsMovingInRoom.begin(), personsMovingInRoom.end());
}

void Room::ClassifyPersonMovingStatusFromDistinctsTracesOnDiffImage(std::vector<cv::Point> &barycentresContoursMovingInstances, std::map<double, cv::Point> &distancesBarycentresToPosition, double maxDistanceCurrentBarPrevPoint, std::vector<cv::Point> &personsMovingInRoom, cv::Mat & renderingPositions, cv::Mat &iRenderingPositions, std::vector<cv::Point> &leavingPersons)
{
	//the min distance between all baycentres and a prevPosition is very likely to represent a move from the same person
	// the first element of map distancesBarycentresToPosition is the barycentrePoint = prevPosition of the person itself
	// The second minimum distance is expected to come from the current position of the person
	//if the second min distance is higher than the tolerated rank, while the first min distance is lower -> person leaving the room
	Point barycentrePotentialLeavingPerson = EraseInBarycentresClosestBarycentreFromPersonInRoom(barycentresContoursMovingInstances, distancesBarycentresToPosition);
	distancesBarycentresToPosition.erase(distancesBarycentresToPosition.begin());

	// CASE :: MOVING PERSON SEPARATE CONTOURS ON DIFF IMAGE
	// the second closest barycentre can be the position of a moving person that moved enough between two frames to leave separate traces on diff image but still close enough to 
	// the position of the previous frame. This depends on the fps of the camera and the speed of the person
	if (!distancesBarycentresToPosition.empty() && distancesBarycentresToPosition.begin()->first < maxDistanceCurrentBarPrevPoint)
	{
		// found the position of the moving person on current image, feel in the vector
		personsMovingInRoom.emplace_back(distancesBarycentresToPosition.begin()->second);

		//erase the currentBarycentre matching the prevPosition as not needed anymore, save computation time
		EraseInBarycentresClosestBarycentreFromPersonInRoom(barycentresContoursMovingInstances, distancesBarycentresToPosition);

		//erase the point from the _prevPosition as current Position found successfully
		_personInRoomPrevImage.erase(_personInRoomPrevImage.begin());
		DrawCircleOfPosition(renderingPositions, distancesBarycentresToPosition.begin()->second, iRenderingPositions);

		//optional
		distancesBarycentresToPosition.erase(distancesBarycentresToPosition.begin());
	}
	// CASE :: LEAVING PERSON SEPARATE CONTOURS ON DIFF IMAGE
	else
	{
		// the second closest barycentres expected to account for the position of the person of the current frame is not detected, while the previous barycentre is:
		// the person left the room
		leavingPersons.emplace_back(barycentrePotentialLeavingPerson);
		DrawCircleOfPosition(renderingPositions, *_personInRoomPrevImage.begin(), iRenderingPositions);
		_personInRoomPrevImage.erase(_personInRoomPrevImage.begin());
	}
}

void Room::ClassifyPersonMovingStatusFromMergedTracesOnDiffImage(std::vector<cv::Point> &personsMovingInRoom, std::map<double, cv::Point> &distancesBarycentresToPosition, std::vector<cv::Point> &barycentresContoursMovingInstances, cv::Mat & renderingPositions, cv::Mat &iRenderingPositions)
{
	personsMovingInRoom.emplace_back(distancesBarycentresToPosition.begin()->second);

	//erase the currentBarycentre matching the prevPosition as not needed anymore, save computation time
	EraseInBarycentresClosestBarycentreFromPersonInRoom(barycentresContoursMovingInstances, distancesBarycentresToPosition);

	//erase the point from the _prevPosition as current Position found successfully
	_personInRoomPrevImage.erase(_personInRoomPrevImage.begin());
	DrawCircleOfPosition(renderingPositions, distancesBarycentresToPosition.begin()->second, iRenderingPositions);

	//optional
	distancesBarycentresToPosition.erase(distancesBarycentresToPosition.begin());
}

void Room::ClassifyImmobilePerson(std::vector<cv::Point> &immobilPersons, cv::Mat & renderingPositions, cv::Mat &iRenderingPositions)
{
	immobilPersons.emplace_back(*_personInRoomPrevImage.begin());
	DrawCircleOfPosition(renderingPositions, *_personInRoomPrevImage.begin(), iRenderingPositions);
	_personInRoomPrevImage.erase(_personInRoomPrevImage.begin());
}

void Room::ClassifyEnteringPersons(std::vector<cv::Point> &personsEnteringRoom, std::vector<cv::Point> &barycentresContoursMovingInstances, cv::Mat & renderingPositions, cv::Mat &iRenderingPositions)
{
	//the baycentres that did not match any prevPosition are to far from all those positions, so they are entering persons
	personsEnteringRoom = barycentresContoursMovingInstances;

	for (const auto & singlePersonEnteringRoom : personsEnteringRoom)
	{
		DrawCircleOfPosition(renderingPositions, singlePersonEnteringRoom, iRenderingPositions);
	}
}

void Room::ClassifyPersonMovingStatus(std::vector<cv::Point> &barycentresContoursMovingInstances, double maxDistancePrevBarycentrePointForSamePoint, double maxDistanceCurrentBarPrevPoint, std::vector<cv::Point> &personsMovingInRoom, cv::Mat & renderingPositions, cv::Mat &iRenderingPositions, std::vector<cv::Point> &leavingPersons, std::vector<cv::Point> &immobilPersons, std::vector<cv::Point> &personsEnteringRoom)
{
	while (!_personInRoomPrevImage.empty())
	{
		std::map<double, Point> distancesBarycentresToPosition;
		if (!barycentresContoursMovingInstances.empty())
		{
			for (int i = 0; i < barycentresContoursMovingInstances.size(); i++)
			{
				distancesBarycentresToPosition.emplace(ComputeDistance(barycentresContoursMovingInstances.at(i), _personInRoomPrevImage.front()), barycentresContoursMovingInstances.at(i));
			}

			// case person move enough to have 2 patterns for both the previous and the current image (not immobil so that the previous barycentre appears and the position of the barycentre 
			//detected matches almost perfectly the position of the barycentre computed one loop iteration before
			// CASE :: MOVING PERSON SEPARATE CONTOURS ON DIFF IMAGE
			if (distancesBarycentresToPosition.begin()->first < maxDistancePrevBarycentrePointForSamePoint)
			{
				ClassifyPersonMovingStatusFromDistinctsTracesOnDiffImage(barycentresContoursMovingInstances, distancesBarycentresToPosition, maxDistanceCurrentBarPrevPoint, personsMovingInRoom, renderingPositions, iRenderingPositions, leavingPersons);
			}
			// CASE :: PERSON MOVES BUT NOT ENOUGH TO GENERATE TWO DISTINCTS TRACES ON THE DIFF IMAGE
			// the person did not have the time move enough between two consecutives frames so that the diff images does not show two distincts patterns, but one bigger resulting from the merge of the two contours
			else if (distancesBarycentresToPosition.begin()->first > maxDistancePrevBarycentrePointForSamePoint &&
				distancesBarycentresToPosition.begin()->first < maxDistanceCurrentBarPrevPoint)
			{
				ClassifyPersonMovingStatusFromMergedTracesOnDiffImage(personsMovingInRoom, distancesBarycentresToPosition, barycentresContoursMovingInstances, renderingPositions, iRenderingPositions);
			}
			// CASE :: THE BARYCENTRE OF PERSON PREVIOUS IMAGES IS TOO FAR FROM ALL BARYCENTRES : IMMOBIL PERSON
			else
			{
				// none of the barycentres if close enough within a given range to a prevPoint, then the prevPoint is now an immobil Person
				//the distance is too big for accounting for a move of a person -> prevPosition element is a candidate for position to immobil person
				// if the minimal distance is bigger than expected, that means also the second closer is bigger than expected so that the too point clouds resulting from a diff of a moving person are too far : immobil person
				ClassifyImmobilePerson(immobilPersons, renderingPositions, iRenderingPositions);
			}
		}
		// CASE :: NO BARYCENTRES BUT PERSON IN ROOM PREVIOUSLY : IMMOBIL PERSON
		else
		{
			//all barycentre for moving persons matched but still some immobil position were not inspected. It is assumed that those are immobil persons, as they are leftovers from the prevImage records
			ClassifyImmobilePerson(immobilPersons, renderingPositions, iRenderingPositions);
		}
	}

	// CASE :: ENTERING PERSON
	ClassifyEnteringPersons(personsEnteringRoom, barycentresContoursMovingInstances, renderingPositions, iRenderingPositions);
}

void Room::ComputePositionMovingAndImmobilInstances(std::unique_ptr<Mat>&& prevImageCamera, std::unique_ptr<Mat>&& currentImageFromCamera, Mat& renderingPositions)
{
	std::vector<Point> barycentresContoursMovingInstances;

	_imageProcessor.GetPositionObjectsFromDiffImage(std::move(*prevImageCamera), *currentImageFromCamera, Config::thresholdDiffImages, barycentresContoursMovingInstances);
	
	if (Room::displayAllImages == true)
	{
		namedWindow("current Image after Get Pos" + std::to_string(indexDisplay), WINDOW_NORMAL);
		imshow("current Image after Get Pos" + std::to_string(indexDisplay), *currentImageFromCamera);
	}

	int numberPrevPersonsInRoom = static_cast<int>(_personInRoomPrevImage.size());
	std::vector<Point> immobilPersons;
	std::vector<Point> leavingPersons;
	std::vector<Point> personsMovingInRoom;
	std::vector<Point> personsEnteringRoom;
	Mat iRenderingPositions = Mat::zeros(prevImageCamera->size(), CV_32F);

	// distance previous barycentre and Prev Point must be very close, actually 0 or almost as the same contour is taken
	//double maxDistancePrevBarPrevPoint = prevImageCamera->size().height / 1;
	double sizeImage = static_cast<double>(prevImageCamera->size().height);
	double maxDistancePrevBarycentrePointForSamePoint = sizeImage * Config::scaleMaxDistancePrevBarycentrePointForSamePoint/ (sizeImage);
	double maxDistanceCurrentBarPrevPoint = sizeImage / Config::scaleMaxDistanceCurrentBarPrevPoint;  //1.5 before

	ClassifyPersonMovingStatus(barycentresContoursMovingInstances, maxDistancePrevBarycentrePointForSamePoint, maxDistanceCurrentBarPrevPoint, 
		personsMovingInRoom, renderingPositions, iRenderingPositions, leavingPersons, immobilPersons, personsEnteringRoom);

	AddAllDetectedPersonsToPersonsInRoom(immobilPersons, personsEnteringRoom, personsMovingInRoom);
	PrintoutPersonsConfigInRoom(leavingPersons, personsEnteringRoom, immobilPersons, personsMovingInRoom);

	if (Room::displayAllImages == true)
	{
		namedWindow("currentPositions", WINDOW_NORMAL);
		imshow("currentPositions", renderingPositions);

		namedWindow("currentPositions" + std::to_string(indexDisplay), WINDOW_NORMAL);
		imshow("currentPositions" + std::to_string(indexDisplay), iRenderingPositions);
	}
}

void Room::PrintoutPersonsConfigInRoom(std::vector<cv::Point> &leavingPersons, std::vector<cv::Point> &personsEnteringRoom, std::vector<cv::Point> &immobilPersons, std::vector<cv::Point> &personsMovingInRoom)
{
	int numberPersonsLeavingRoom = static_cast<int>(leavingPersons.size());
	int numberPersonsEnteringRoom = static_cast<int>(personsEnteringRoom.size());
	int numberImmobilPersons = static_cast<int>(immobilPersons.size());
	int numberPersonsMovingInRoom = static_cast<int>(personsMovingInRoom.size());

	int numberPersonInRoom = static_cast<int>(_personsInRoom.size());

	std::cout << "There is " << numberPersonInRoom << " persons in the room" << "\n";
	std::cout << "There is " << numberPersonsMovingInRoom << " persons moving in the room" << "\n";
	std::cout << "There is " << numberPersonsLeavingRoom << " persons leaving the room" << "\n";
	std::cout << "There is " << numberPersonsEnteringRoom << " persons entering the room" << "\n";
	std::cout << "There is " << numberImmobilPersons << " persons immobil in the room" << "\n";
	std::cout << "\n";
}

void Room::UpdateLightsIntensity()
{
	if (!_personsInRoom.empty())
	{
		double intensityPerPerson = static_cast<double>(Config::totalIntensity / static_cast<int>(_personsInRoom.size()));
		std::vector<double> lightsIntensityDistributedPerPerson(_lightSources.size());
		std::vector<std::tuple<int, double>> lightsIntensityUpdates;

		//Compute Intensity to be set per Light and for all the persons in the room
		for (const auto & person : _personsInRoom)
		{
			double totalDistancesToLightsForOnePerson = 0;

			for (auto & lightSource : _lightSources)
			{
				totalDistancesToLightsForOnePerson += ComputeDistance(person, lightSource.GetPosition());
				lightsIntensityUpdates.emplace_back(lightSource.GetId(), 0);
			}

			for (auto & lightSource : _lightSources)
			{
				auto it = std::find_if(lightsIntensityUpdates.begin(), lightsIntensityUpdates.end(), [&lightSource](const std::tuple<int, double>& e) {return std::get<0>(e) == lightSource.GetId(); });
				std::get<1>(*it) += ComputeDistance(person, lightSource.GetPosition()) / totalDistancesToLightsForOnePerson / static_cast<double>(_personsInRoom.size());
			}
		}

		// Set Intensity for all the lights
		for (auto & lightSource : _lightSources)
		{
			lightSource.Set(1 - std::get<1>(*std::find_if(lightsIntensityUpdates.begin(), lightsIntensityUpdates.end(), [&lightSource](const std::tuple<int, double>& e) {return std::get<0>(e) == lightSource.GetId(); })));
		}

		FinalRendering();
	}

	JsonParser::UpdateLightsIntensityOnJsonFile(_lightSources);
}

void Room::FinalRendering()
{
	Mat img = _currentImage;
	Mat imageDisplay = _currentImage.clone();

	for (auto & lightSource : _lightSources)
	{
		Point point(static_cast<int>(lightSource.GetIntensity() *100), static_cast<int>(lightSource.GetPosition().y*50));
		circle(imageDisplay, lightSource.GetPosition(), static_cast<int>(lightSource.GetIntensity()*50), lightSource.GetIntensity() * 255, -1, 8, 0);
	}
	for (auto & person : _personsInRoom)
	{
		circle(imageDisplay, person, 5, Scalar(150, 140, 150), 1, 8, 0);
	}

	auto boundingBoxes = _imageProcessor.GetBoundingBoxes();
	RNG rng(12345);

	for (int i = 0; i < boundingBoxes.size(); i++)
	{
		Scalar color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
		rectangle(imageDisplay, boundingBoxes[i].tl(), boundingBoxes[i].br(), color, 2);
	}

	
	namedWindow("Current Image" + std::to_string(Room::indexDisplay), WINDOW_NORMAL);
	imshow("Current Image" + std::to_string(Room::indexDisplay), _currentImage.clone());

	namedWindow("Final Rendering" + std::to_string(Room::indexDisplay), WINDOW_NORMAL);
	imshow("Final Rendering" + std::to_string(Room::indexDisplay), imageDisplay);
}

double Room::ComputeDistance(const Point & pointFrom, const Point & pointTo)
{
	return cv::norm(pointFrom - pointTo);
}
