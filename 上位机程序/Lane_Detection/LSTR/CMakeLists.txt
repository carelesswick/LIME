cmake_minimum_required(VERSION 3.12)

project(LSTR)

set(CMAKE_CXX_STANDARD 11 )
set(BOOST_THREAD_LIBRARY boost_thread -pthread)
set(ONNXRUNTIME_LIB ${PROJECT_SOURCE_DIR}/lib/libonnxruntime.so)

find_package( OpenCV REQUIRED )
find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
    message("OPENMP FOUND")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
else ()
    message(FATAL_ERROR "OpenMP Not Found!")
endif ()

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
