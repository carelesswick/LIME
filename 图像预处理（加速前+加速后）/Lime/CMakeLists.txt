
cmake_minimum_required(VERSION 3.12)
project(lime_processing)

set(CMAKE_CXX_STANDARD 14)

set( CMAKE_BUILD_TYPE "Release" )
set( CMAKE_CXX_FLAGS "-std=c++11")

find_package( OpenCV REQUIRED )


include_directories( 
    ${OpenCV_INCLUDE_DIRS} 
)
include_directories( ${PROJECT_SOURCE_DIR}/include )
add_executable(lime lime.cpp)
target_link_libraries(lime ${OpenCV_LIBS})







