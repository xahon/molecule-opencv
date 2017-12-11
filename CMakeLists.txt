cmake_minimum_required(VERSION 2.8)
project( out )

set(CMAKE_CXX_STANDARD 14)

find_package( OpenCV REQUIRED )
add_executable( out main.cpp includes.h node.h settings.h utils.h )
target_link_libraries( out ${OpenCV_LIBS} )
