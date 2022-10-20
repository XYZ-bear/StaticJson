# cmake needs this line
cmake_minimum_required(VERSION 2.8)

# Define project name
project(StaticJson CXX C)

add_definitions(-DIEEE_8087)

add_definitions(-w)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_COMPILER g++)

set(CMAKE_CXX_FLAGS -pthread)

SET(CMAKE_BUILD_TYPE Release)

aux_source_directory(. DIR_SRCS)

aux_source_directory(./test DIR_SRCS)

file(GLOB_RECURSE HEAD_SRCS *.h)

message(${HEAD_SRCS})

source_group("Header Files" FILES ${HEAD_SRCS})
source_group("Source Files" FILES ${DIR_SRCS})

include_directories(./ ./include)

add_executable(StaticJson ${DIR_SRCS} ${HEAD_SRCS})
