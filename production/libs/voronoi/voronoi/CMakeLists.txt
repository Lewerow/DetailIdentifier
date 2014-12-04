
PROJECT( voronoi_proj )
cmake_minimum_required(VERSION 2.6)
set(CMAKE_BUILD_TYPE RelWithDebInfo)
SET(CMAKE_VERBOSE_MAKEFILE ON)
FIND_PACKAGE( OpenCV REQUIRED )

ADD_EXECUTABLE( voronoi test_voronoi.cpp voronoi.h)
TARGET_LINK_LIBRARIES( voronoi ${OpenCV_LIBS} )


