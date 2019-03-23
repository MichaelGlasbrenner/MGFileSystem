cmake_minimum_required(VERSION 3.10.2)
project (MGfilesystem)

# add the headers in ./include to the project
include_directories(include)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON) 

# add the cpp files:
file(GLOB SOURCES "*/*.cpp")

# add the header files: (this is needed for Qt, otherwise the moc compiler is not invoked)
file(GLOB HEADERS "include/*.h")

find_package(FUSE REQUIRED)

add_executable(MGfilesystem ${SOURCES} ${HEADERS}) 

SET(CMAKE_CXX_FLAGS "-std=c++0x")
