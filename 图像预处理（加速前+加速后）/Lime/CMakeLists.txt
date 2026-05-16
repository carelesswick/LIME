cmake_minimum_required(VERSION 3.12)
project(lime_processing)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS "-std=c++11")

find_package(OpenCV REQUIRED)

# 关键修复：包含 OpenCV 头文件目录和项目本地 include 目录
include_directories(${OpenCV_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include)

# 添加可执行文件
add_executable(lime lime.cpp)

# 链接 OpenCV 库
target_link_libraries(lime ${OpenCV_LIBS})







