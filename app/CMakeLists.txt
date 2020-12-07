cmake_minimum_required (VERSION 3.0)
cmake_policy(VERSION 3.0)

project(LASBlock C CXX)

find_library(LASLIB libLASlib.a HINTS ${LASlib_LIBRARY})
if(LASLIB)
	message("Found LASlib true.")
endif(LASLIB)
find_package(Boost REQUIRED COMPONENTS system thread)

include_directories(${LASlib_INCLUDE_DIR})
#qt5
find_package(Qt5 COMPONENTS Core REQUIRED)
include_directories(${Qt5Core_INCLUDE_DIRS})

file(GLOB SOURCES "*.cpp" "*.h")
set(SRCS_FILES ${SOURCES})

add_executable (${PROJECT_NAME} ${SRCS_FILES})

target_link_libraries(${PROJECT_NAME} ${LASLIB})
target_link_libraries( ${PROJECT_NAME} Qt5::Core)