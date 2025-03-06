cmake_minimum_required(VERSION 3.27)
project(face_recognition LANGUAGES CXX C CUDA)

find_package(OpenCV REQUIRED)

set(OpenCV_DIR /opt/opencv/)

add_executable(face_recognition
        main.cpp)

target_link_libraries(face_recognition PRIVATE ${OpenCV_LIBS})