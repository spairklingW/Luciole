cmake_minimum_required(VERSION 2.8)

project( DisplayImage )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})
set(OpenCV_DIR ${OpenCV_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/Debug)

find_package( OpenCV REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( DisplayImage DisplayImage.cpp JsonParser.cpp JsonParser.h ImageParser.cpp ImageParser.h Config.cpp Config.h ImageParserMock.cpp ImageParserMock.h ImageProcessor.cpp ImageProcessor.h LightSource.cpp LightSource.h RenderingSimulator.cpp RenderingSimulator.h Room.cpp Room.h)

add_subdirectory(nlohmann)

target_link_libraries( DisplayImage PUBLIC ${OpenCV_LIBS} nlohmann_json::nlohmann_json)


