#ifndef Room_H_
#define Room_H_
#include "LightSource.h"
#include "ImageProcessor.h"
#include "ImageParser.h"
#include "ImageParserMock.h"
#include "RenderingSimulator.h"
#include "JsonParser.h"

#include <string>

using namespace cv;

class Room
{

public:
    // constructors / destructors
    Room();                    
    Room(std::string filename);
	Room(int numberLightSources, std::shared_ptr<RenderingSimulator> renderingSimulator, Mat refImage);
    ~Room();

	Room& operator=(Room&) = default;
	Room(Room&) = default;

	Room& operator=(Room&&) = default;
	Room(Room&&) = default;

	// Initialization methods for Mock Scenario
	void ModelInitializationLightSources();

	void InitializeMockImages();

	void InitializeRefImageMock();

	void InitializePositionMovingInstancesMock();

	// Initialization methods for Real Scenario
	void InitializeFrameWithoutMotion();

	void InitializeLightSourcesFromRealScene();

	void DefineStopConditionForInitialization(int i, int indexFrameStream, bool &continueStreaming);

	// Loop Over Images Mock Scenario
	void CyclicUpdateLightsFromMockImages();

	// Loop Over Images Real Scenario
	void CyclicUpdateLightsFromImages(const std::string & pathToVideo);

	void getFrame();

	void startRecording(const std::string & pathToVideo);

	void ProcessMovingInstances(std::unique_ptr<Mat>&& prevImageCamera, std::unique_ptr<Mat>&& currentImageFromCamera, Mat& renderingPositions, int index);

	void ComputePositionMovingAndImmobilInstances(std::unique_ptr<Mat>&& prevImageCamera, std::unique_ptr<Mat>&& currentImageFromCamera, Mat& renderingPositions);

	// classify moving instances
	void ClassifyPersonMovingStatus(std::vector<cv::Point> &barycentresContoursMovingInstances, double maxDistancePrevBarycentrePointForSamePoint, double maxDistanceCurrentBarPrevPoint, std::vector<cv::Point> &personsMovingInRoom, cv::Mat & renderingPositions, cv::Mat &iRenderingPositions, std::vector<cv::Point> &leavingPersons, std::vector<cv::Point> &immobilPersons, std::vector<cv::Point> &personsEnteringRoom);

	//helper classification moving instances
	void ClassifyPersonMovingStatusFromDistinctsTracesOnDiffImage(std::vector<cv::Point> &barycentresContoursMovingInstances, std::map<double, cv::Point> &distancesBarycentresToPosition, double maxDistanceCurrentBarPrevPoint, std::vector<cv::Point> &personsMovingInRoom, cv::Mat & renderingPositions, cv::Mat &iRenderingPositions, std::vector<cv::Point> &leavingPersons);

	void ClassifyPersonMovingStatusFromMergedTracesOnDiffImage(std::vector<cv::Point> &personsMovingInRoom, std::map<double, cv::Point> &distancesBarycentresToPosition, std::vector<cv::Point> &barycentresContoursMovingInstances, cv::Mat & renderingPositions, cv::Mat &iRenderingPositions);

	void ClassifyImmobilePerson(std::vector<cv::Point> &immobilPersons, cv::Mat & renderingPositions, cv::Mat &iRenderingPositions);

	void ClassifyEnteringPersons(std::vector<cv::Point> &personsEnteringRoom, std::vector<cv::Point> &barycentresContoursMovingInstances, cv::Mat & renderingPositions, cv::Mat &iRenderingPositions);

	Mat ConvertFrameToGreyChar(Mat & imagesrc);

	Point EraseInBarycentresClosestBarycentreFromPersonInRoom(std::vector<cv::Point> &barycentresContoursMovingInstances, std::map<double, cv::Point> &distancesBarycentresToPosition);

	void DrawCircleOfPosition(cv::Mat & renderingPositions, const  cv::Point &pointDistancesBarycentresToPosition, cv::Mat &iRenderingPositions);

	void AddAllDetectedPersonsToPersonsInRoom(std::vector<cv::Point> &immobilPersons, std::vector<cv::Point> &personsEnteringRoom, std::vector<cv::Point> &personsMovingInRoom);

	void PrintoutPersonsConfigInRoom(std::vector<cv::Point> &leavingPersons, std::vector<cv::Point> &personsEnteringRoom, std::vector<cv::Point> &immobilPersons, std::vector<cv::Point> &personsMovingInRoom);

	//Update the hardware 
	void UpdateLightsIntensity();

	void FinalRendering();

	double ComputeDistance(const Point & pointFrom, const Point & pointTo);

	// static variables
	static int indexDisplay;
	static bool displayAllImages;
	static int heightImage;
	static int widthImage;
	static Size baseSizeImage;

private:
	ImageParser _imageParser; 
	ImageParserMock _imageParserMock;
	ImageProcessor _imageProcessor;
	std::vector<LightSource> _lightSources;
	std::vector<Point> _personsInRoom;
	std::vector<Point> _personInRoomPrevImage;
	Mat _lastImageMovingPerson;
	Mat _currentImage;
	Mat _previousImage;
	Mat _refImageWithoutLightSource;
	Mat _refImageWithoutMotion;
};

#endif /* Room_H_ */