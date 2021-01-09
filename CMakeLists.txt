cmake_minimum_required(VERSION 2.8)

project( DisplayImage )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})
set(OpenCV_DIR ${OpenCV_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/Debug)

find_package( OpenCV REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( DisplayImage DisplayImage.cpp)

target_link_libraries( DisplayImage PUBLIC ${OpenCV_LIBS})


