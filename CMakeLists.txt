cmake_minimum_required (VERSION 3.0)
cmake_policy(VERSION 3.0)

project(Lastile C CXX)

if(MSVC)
	set(CMAKE_CXX_STANDARD 11)
	set(CMAKE_CXX_STANDARD_REQUIRED yes)
    	add_definitions(-DNOMINMAX)
    	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    	string(APPEND CMAKE_EXE_LINKER_FLAGS_DEBUG " /NODEFAULTLIB:LIBCMT /NODEFAULTLIB:MSVCRT")
else()
	add_compile_options(-Wall -Wno-strict-aliasing)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2 -pthread")
	set(CMAKE_BUILD_TYPE "Release")
endif()

add_subdirectory(${CMAKE_SOURCE_DIR}/LASlib)
add_subdirectory(${CMAKE_SOURCE_DIR}/lastile)
