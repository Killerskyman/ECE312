cmake_minimum_required(VERSION 3.21)
project(Project2)

set(CMAKE_CXX_STANDARD 14)

add_executable(Project2 udpClient.cpp udpClient.h main.cpp RHP.cpp RHP.h RHMP.cpp RHMP.h)
