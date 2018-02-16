cmake_minimum_required(VERSION 3.8)
project(lesson_2)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp)
add_executable(lesson_2 ${SOURCE_FILES})
find_package( OpenCV REQUIRED )
target_link_libraries( lesson_2 ${OpenCV_LIBS} )
target_link_libraries( lesson_2 curl)