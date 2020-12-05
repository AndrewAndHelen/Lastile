cmake_minimum_required (VERSION 3.0)
cmake_policy(VERSION 3.0)

project(LASBlock C CXX)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#set path
set(LASlib_INCLUDE_DIR "/usr/local/include/LASlib")
set(LASlib_LIBRARY "/usr/local/lib/LASlib")
set(CMAKE_PREFIX_PATH "/home/andrew/Qt5.12.9/5.12.9/gcc_64" cache path "Qt5_DIR")

if(CMAKE_CL_64)  
    	set(PLATFORM "x64")
else(CMAKE_CL_64)  
    	set(PLATFORM "WIN32")
endif(CMAKE_CL_64)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	set(PLATFORM "Linux")
endif(CMAKE_SYSTEM_NAME MATCHES "Linux")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${PLATFORM})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/${PLATFORM})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/${PLATFORM})

if(MSVC)
	set(CMAKE_CXX_STANDARD 11)
	set(CMAKE_CXX_STANDARD_REQUIRED yes)
    	add_definitions(-DNOMINMAX)
    	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    	string(APPEND CMAKE_EXE_LINKER_FLAGS_DEBUG " /NODEFAULTLIB:LIBCMT /NODEFAULTLIB:MSVCRT")
else()
	add_compile_options(-Wall -Wno-strict-aliasing)
endif()

# set MD to MT
if(OPTION_BUILD_MT)
set(CompilerFlags
    CMAKE_CXX_FLAGS
    CMAKE_CXX_FLAGS_DEBUG
    CMAKE_CXX_FLAGS_RELEASE
    CMAKE_C_FLAGS
    CMAKE_C_FLAGS_DEBUG
    CMAKE_C_FLAGS_RELEASE
  )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -lboost_system")
foreach(CompilerFlags ${CompilerFlags})
  #string(REPLACE "/MD" "/MT" ${CompilerFlags} "${${CompilerFlags}}")
  string(REPLACE "/Od" "/O2" ${CompilerFlags} "${${CompilerFlags}}")
endforeach()
endif()

add_subdirectory(${CMAKE_SOURCE_DIR}/app)
