cmake_minimum_required(VERSION 3.10.2)
project (MGfilesystem)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# add the headers in ./include to the project
include_directories(${FUSE_INCLUDE_DIR})
include_directories(include)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON) 

# add the cpp files:
file(GLOB SOURCES "*/*.cpp")

# add the header files
file(GLOB HEADERS "include/*.h")

find_package(FUSE REQUIRED)

add_executable(MGfilesystem ${SOURCES} ${HEADERS}) 

SET(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++0x")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -D_FILE_OFFSET_BITS=64")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall --pedantic -g")
target_link_libraries(MGfilesystem ${FUSE_LIBRARIES})

