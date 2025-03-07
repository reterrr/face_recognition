cmake_minimum_required(VERSION 3.27)
project(face_recognition LANGUAGES CXX C CUDA)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(OpenCV REQUIRED)
find_package(Qt6 COMPONENTS Core Widgets REQUIRED)

add_executable(face_recognition
        main.cpp
        MainWindow.cpp
        MainWindow.h)

target_link_libraries(face_recognition PRIVATE ${OpenCV_LIBS} Qt6::Core Qt6::Widgets)