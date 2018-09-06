cmake_minimum_required(VERSION 2.8)

project(S3)

if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -pthread") 
endif(CMAKE_COMPILER_IS_GNUCXX)

include_directories(src third-party)
include_directories(MicroSpecRepo/include)

file(GLOB SRC
	${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

add_subdirectory(MicroSpecRepo)

file(GLOB HEADER ${CMAKE_CURRENT_SOURCE_DIR}/src/s3framework.cpp)
add_library(S3LIB ${HEADER})

add_executable (${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} S3LIB MicroSpec)

add_subdirectory(test)
