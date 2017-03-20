cmake_minimum_required(VERSION 2.8.12)
project(AndorsCMOS CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include/)
link_directories(lib/)

add_executable(serialnum serialnumber/serialnumber.cpp)
target_link_libraries(serialnum atcore)

add_executable(listcam listdevices/main.cpp)
target_link_libraries(listcam atcore)

add_executable(acqcam acquisition/main.cpp)
target_link_libraries(acqcam atcore)

add_executable(imgcam image/main.cpp image/saveAsBmp.cpp common.cpp)
target_link_libraries(imgcam atcore)

add_executable(convbuff convertbuffer/main.cpp)
target_link_libraries(convbuff atcore atutility)

add_executable(reconnectcam reconnect/main.cpp)
target_link_libraries(reconnectcam atcore)

add_executable(livecam live/live.cpp)
target_link_libraries(livecam atcore)
