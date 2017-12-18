cmake_minimum_required(VERSION 3.2)
project(GraphReader)

set(CMAKE_CXX_STANDARD 11)

set(TINY_XML tinystr.h tinystr.cpp tinyxml.h tinyxml.cpp tinyxmlerror.cpp tinyxmlparser.cpp)

set(SOURCE_FILES Graph.h ${TINY_XML} main.cpp)

add_executable(GraphReader ${SOURCE_FILES})