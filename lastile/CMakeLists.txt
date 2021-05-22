cmake_minimum_required (VERSION 3.0)
cmake_policy(VERSION 3.0)

project(LASBlock C CXX)

#qt5
find_package(Qt5 COMPONENTS Core REQUIRED)
include_directories(${Qt5Core_INCLUDE_DIRS})

file(GLOB SOURCES "*.cpp" "*.h")
set(SRCS_FILES ${SOURCES})

add_executable (${PROJECT_NAME} ${SRCS_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC LASlib Qt5::Core)
target_include_directories(${PROJECT_NAME} PUBLIC 
	${CMAKE_SOURCE_DIR}/LASlib/include
	)