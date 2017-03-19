cmake_minimum_required(VERSION 2.8.12)
project(AndorsCMOS CXX)


add_executable(acquistion acquisition/main.cpp)

include_directories(include/)
