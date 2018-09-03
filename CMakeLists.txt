cmake_minimum_required(VERSION 2.8)

project(S3)

if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -pthread") 
endif(CMAKE_COMPILER_IS_GNUCXX)

include_directories(src third-party)

file(GLOB SRC
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_executable (${PROJECT_NAME} ${SRC})
