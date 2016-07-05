cmake_minimum_required(VERSION 2.8)

project(calibrators)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#Set OpenNI2_DIR in order to find OpenNI2 libs
#set(OpenNI2_DIR "/opt/OpenNI-Linux-x64-2.2/cmake")

#Set CMAKE_MODULE_PATH in order to find OpenNI2 libs
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "/opt/OpenNI-Linux-x64-2.2/cmake")

#Set c++11 standard
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(OpenCV 3.1.0 REQUIRED)
find_package(OpenNI2 REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${OPENNI2_INCLUDE_DIRS})

message(STATUS "OpenCV_Include ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV_libs ${OpenCV_LIBS}")
message(STATUS "OpenCV_install ${OpenCV_INSTALL_PATH}")

#Add_libraries
add_library(openni2_cv_grabber openni2_cv_grabber)
target_link_libraries(openni2_cv_grabber ${OPENNI2_LIBRARIES})
target_include_directories(openni2_cv_grabber PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

#Add_executables
#add_executable(calibration calibration.cpp)
#target_link_libraries(calibration ${OpenCV_LIBS})

#add_executable(openni_capture openni_capture.cpp)
#target_link_libraries(openni_capture ${OpenCV_LIBS})

#add_executable(openni_capture_fork openni_capture_fork.cpp)
#target_link_libraries(openni_capture_fork ${OpenCV_LIBS})

add_executable(main main.cpp)
target_link_libraries(main ${OpenCV_LIBS} openni2_cv_grabber)

#add_executable(test testNICV_get_IR_Stream.cpp)
#target_link_libraries(test ${OpenCV_LIBS} ${OPENNI2_LIBRARIES})

#add_executable(zed_calibrator 3calibration_zed.cpp)
#target_link_libraries(zed_calibrator ${OpenCV_LIBS})
