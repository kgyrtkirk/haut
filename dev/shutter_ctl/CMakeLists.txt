cmake_minimum_required (VERSION 2.6)
project (shutter_ctl)
add_executable(shutter_ctl main.cpp rf433ctl.cpp)
target_link_libraries(shutter_ctl wiringPi)
