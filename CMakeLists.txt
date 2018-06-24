cmake_minimum_required(VERSION 2.8.3)
project(meshToolbox)

## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++11)

# Required libraries

#set(CMAKE_BUILD_TYPE Debug

# Source files
set(MT_SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/src/meshAnalyze.cpp
#    ${PROJECT_SOURCE_DIR}/src/System.cpp
#    ${PROJECT_SOURCE_DIR}/src/Tracker.cpp
)

include_directories(
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/3rdparty
)

# create basic library
#$add_library(mt_library ${meshToolbox_SOURCE_FILES})

# add an executable per target
add_executable(meshAnalyze ${MT_SOURCE_FILES})

# link required libraries
#target_link_libraries(objLoad mt_library)
