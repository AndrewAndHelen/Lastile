cmake_minimum_required (VERSION 3.0)
cmake_policy(VERSION 3.0)

project(LASBlock C CXX)
if(MSVC)
set(BOOST_INCLUDEDIR $ENV{Boost_INCLUDE_DIRS} CACHE PATH "BOOST_INCLUDEDIR")
set(BOOST_LIBRARYDIR $ENV{BOOST_LIBRARY_DIR} CACHE PATH "BOOST_LIBRARYDIR")
endif()

#boost
if(MSVC)
  set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
  set(Boost_USE_DEBUG_LIBS         ON) # ignore debug libs and
  set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
  set(Boost_USE_MULTITHREADED      ON)
  set(Boost_USE_STATIC_RUNTIME    OFF)
endif()
find_package(Boost COMPONENTS system filesystem REQUIRED)

#qt5
find_package(Qt5 COMPONENTS Core REQUIRED)
include_directories(${Qt5Core_INCLUDE_DIRS})

file(GLOB SOURCES "*.cpp" "*.h")
set(SRCS_FILES ${SOURCES})

add_executable (${PROJECT_NAME} ${SRCS_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC LASlib Qt5::Core ${Boost_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC 
	${CMAKE_SOURCE_DIR}/LASlib/include
	${Boost_INCLUDE_DIRS}
	)