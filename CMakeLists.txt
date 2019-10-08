cmake_minimum_required(VERSION 3.10)
project(helloworld)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXE_LINKER_FLAGS "-pthread -lpthread" CACHE INTERNAL "" FORCE)
#CMAKE_EXE_LINKER_FLAGS(LDFLAGS= -pthread -lpthread)
add_executable(helloworld main.cpp)
