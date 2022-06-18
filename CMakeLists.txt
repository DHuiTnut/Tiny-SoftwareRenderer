cmake_minimum_required(VERSION 3.21)
project(MyTinyRender)

set(CMAKE_CXX_STANDARD 14)

add_executable(MyTinyRender tgaimage.cpp rasterizater.hpp tgaimage.h MyMath.hpp model.hpp transform.hpp main.cpp triangle.hpp)
