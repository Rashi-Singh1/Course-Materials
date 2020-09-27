cmake_minimum_required(VERSION 3.15)
project(Practice)

set(CMAKE_CXX_STANDARD 14)

add_executable(Practice way2.cpp)
target_link_libraries(Practice -lglut -lGL -lGLU)