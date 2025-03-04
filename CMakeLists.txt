cmake_minimum_required(VERSION 3.27)
project(face_recognition LANGUAGES CXX C CUDA)

find_package(OpenCV REQUIRED)

add_executable(face_recognition main.cpp
        main.cpp)