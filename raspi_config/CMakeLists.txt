cmake_minimum_required(VERSION 3.1)

# Enable C++11

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

SET(OpenCV_DIR <OpenCV_Home_Dir>/installation/OpenCV-master/lib/cmake/opencv4)

#In my case the installation is on 

SET(OpenCV_DIR usr/local/lib/cmake/opencv4)

#this was defined in the step CMAKE_INSTALL_PREFIX=/usr/local