cmake_minimum_required(VERSION 2.8.12)
project(AndorsCMOS CXX)

include_directories(include/)
link_directories(lib/)

add_executable(serialnum serialnumber/serialnumber.cpp)
target_link_libraries(serialnum atcore)

add_executable(listcam listdevices/main.cpp)
target_link_libraries(listcam atcore)

