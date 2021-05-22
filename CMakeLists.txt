cmake_minimum_required (VERSION 3.0)
cmake_policy(VERSION 3.0)

project(LASBlock C CXX)

if(MSVC)
	set(CMAKE_CXX_STANDARD 11)
	set(CMAKE_CXX_STANDARD_REQUIRED yes)
    	add_definitions(-DNOMINMAX)
    	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    	string(APPEND CMAKE_EXE_LINKER_FLAGS_DEBUG " /NODEFAULTLIB:LIBCMT /NODEFAULTLIB:MSVCRT")
else()
	add_compile_options(-Wall -Wno-strict-aliasing)
endif()

add_subdirectory(${CMAKE_SOURCE_DIR}/LASlib)
add_subdirectory(${CMAKE_SOURCE_DIR}/lastile)
