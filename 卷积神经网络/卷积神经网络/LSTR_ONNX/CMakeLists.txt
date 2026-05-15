cmake_minimum_required(VERSION 3.12)

project(LSTR)

set(CMAKE_CXX_STANDARD 11 )
set(BOOST_THREAD_LIBRARY boost_thread -pthread)
set(ONNXRUNTIME_LIB ${PROJECT_SOURCE_DIR}/lib/libonnxruntime.so)

find_package( OpenCV REQUIRED )
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/include)
#include_directories(${ONNXRUNTIME_INCLUDE_DIRS})
add_executable(LSTR main.cpp)
target_link_libraries( LSTR 
							libpthread.so
							libncurses.so
							${OpenCV_LIBS}
							${ONNXRUNTIME_LIB}
 )
