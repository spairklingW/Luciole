cmake_minimum_required(VERSION 2.8)

project( Main_Stream )

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})
set(OpenCV_DIR ${OpenCV_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/Debug)

SET(OpenCV_DIR usr/local/lib/cmake/opencv4)

find_package( OpenCV REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( Main_Stream Main_Stream.cpp)

add_subdirectory(nlohmann)

target_link_libraries( Main_Stream PUBLIC ${OpenCV_LIBS} nlohmann_json::nlohmann_json)


