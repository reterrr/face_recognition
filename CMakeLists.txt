cmake_minimum_required(VERSION 3.27)
project(face_recognition LANGUAGES CXX CUDA)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)
enable_language(CUDA)

set(TENSORRT_ROOT "/home/yhwach/Downloads/TensorRT-10.9.0.34.Linux.x86_64-gnu.cuda-12.8/TensorRT-10.9.0.34/targets/x86_64-linux-gnu/lib")
set(TENSORRT_INCLUDE "/home/yhwach/Downloads/TensorRT-10.9.0.34.Linux.x86_64-gnu.cuda-12.8/TensorRT-10.9.0.34/targets/x86_64-linux-gnu/include")

include_directories(${TENSORRT_INCLUDE})

find_package(OpenCV REQUIRED)
find_package(Qt6 COMPONENTS Core Widgets REQUIRED)
find_package(CUDAToolkit REQUIRED)

add_library(nvinfer SHARED IMPORTED)
set_target_properties(nvinfer PROPERTIES IMPORTED_LOCATION "${TENSORRT_ROOT}/libnvinfer.so.10.9.0")

add_library(nvinfer_plugin SHARED IMPORTED)
set_target_properties(nvinfer_plugin PROPERTIES IMPORTED_LOCATION "${TENSORRT_ROOT}/libnvinfer_plugin.so.10.9.0")

add_library(nvinfer_vc_plugin SHARED IMPORTED)
set_target_properties(nvinfer_vc_plugin PROPERTIES IMPORTED_LOCATION "${TENSORRT_ROOT}/libnvinfer_vc_plugin.so.10.9.0")

add_library(nvonnxparser SHARED IMPORTED)
set_target_properties(nvonnxparser PROPERTIES IMPORTED_LOCATION "${TENSORRT_ROOT}/libnvonnxparser.so.10.9.0")

add_executable(face_recognition
        main.cpp
        MainWindow.cpp
        MainWindow.h
        ImageProcessor.cpp
        ImageProcessor.h)

target_link_libraries(face_recognition PRIVATE
        ${OpenCV_LIBS}
        Qt6::Core
        Qt6::Widgets
        CUDA::cudart
        CUDA::toolkit
        nvinfer
        nvinfer_plugin
        nvinfer_vc_plugin
        nvonnxparser)
