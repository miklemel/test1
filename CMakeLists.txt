cmake_minimum_required(VERSION 3.10)
project(test3)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXE_LINKER_FLAGS "-pthread -lpthread" CACHE INTERNAL "" FORCE)

#set(HEADER_FILES Test1.h )

#CMAKE_EXE_LINKER_FLAGS(LDFLAGS= -pthread -lpthread)
add_executable(test3  main.cpp Test1.cpp Test1.h )
