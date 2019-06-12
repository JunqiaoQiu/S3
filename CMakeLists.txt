cmake_minimum_required(VERSION 2.8)

project(S3)

# Enusre there is the lastest implementation of fsm 
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/fsm")
message("-- FSM implementation Exist")
else()
message("-- No FSM implementation")
#execute_process(COMMAND git clone https://github.com/JunqiaoQiu/MicroSpec.git
#WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()


if(CMAKE_COMPILER_IS_GNUCXX)
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -pthread -mavx2 -msse4") 
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -pthread")
endif(CMAKE_COMPILER_IS_GNUCXX)

# Setting with considering allowing actions for dfa computation
set(ALLOW_ACTION ON)
if(ALLOW_ACTION)
	message("DFA run with action is allowed")
else()
	message("DFA run without any action")
endif()

set(AVX2_SUPPORT OFF)
if(AVX2_SUPPORT)
	message("Allow AVX2")
else()
	message("Not Allow AVX2")
endif()

include_directories(src third-party)
include_directories(fsm/include)

file(GLOB SRC
	${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

add_subdirectory(fsm)

file(GLOB HEADER ${CMAKE_CURRENT_SOURCE_DIR}/src/s3framework.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/s3verifier.cpp)
add_library(S3LIB ${HEADER})

add_executable (${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} S3LIB fsm)

add_subdirectory(test)
