#include <array>
#include <cstring>
#include <NvInferRuntime.h>
#include <cuda_runtime_api.h>
#include <fstream>
#include <iostream>
#include <memory>

#include <vector>
#include <opencv2/opencv.hpp>

// Logger class for TensorRT
class Logger final : public nvinfer1::ILogger {
public:
    void log(nvinfer1::ILogger::Severity severity, const char* msg) noexcept override {
        if (severity <= nvinfer1::ILogger::Severity::kWARNING) {
            std::cout << msg << std::endl;
        }
    }
};

// Utility function to load the engine file into memory
std::vector<char> loadEngineFile(const std::string& engineFilePath) {
    std::ifstream file(engineFilePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open engine file: " + engineFilePath);
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        throw std::runtime_error("Unable to read engine file: " + engineFilePath);
    }

    file.close();
    return buffer;
}

constexpr auto image_size = 640;

constexpr std::size_t input_size = 1 * 3 * image_size * image_size;
std::array<float, input_size> input_data{}; // Zero-initialized input

int main() {
    Logger logger;
    std::string engineFilePath =
        "/home/yhwach/Downloads/TensorRT-10.9.0.34.Linux.x86_64-gnu.cuda-12.8/TensorRT-10.9.0.34/targets/x86_64-linux-gnu/bin/yolov5.trt";

    cv::Mat image = cv::imread("/home/yhwach/Downloads/photo_2025-03-18_14-11-51.jpg");
    cv::resize(image, image, cv::Size(image_size, image_size));
    cv::cvtColor(image, image, cv::COLOR_RGBA2RGB);

    // Convert image to float and normalize
    image.convertTo(image, CV_32FC3, 1.0 / 255.0);

    // Fill input_data in CHW format
    for (int c = 0; c < 3; ++c) {  // Channels: RGB
        for (int h = 0; h < image_size; ++h) {  // Height
            for (int w = 0; w < image_size; ++w) {  // Width
                input_data[c * image_size * image_size + h * image_size + w] = image.at<cv::Vec3f>(h, w)[c];
            }
        }
    }

    try {
        // Load the engine file
        std::vector<char> engineData = loadEngineFile(engineFilePath);

        // Create TensorRT runtime and deserialize engine
        std::unique_ptr<nvinfer1::IRuntime> runtime(nvinfer1::createInferRuntime(logger));
        if (!runtime) throw std::runtime_error("Failed to create TensorRT runtime");

        std::unique_ptr<nvinfer1::ICudaEngine> engine(runtime->deserializeCudaEngine(
            engineData.data(), engineData.size()));
        if (!engine) throw std::runtime_error("Failed to deserialize CUDA engine");

        std::unique_ptr<nvinfer1::IExecutionContext> context(engine->createExecutionContext());
        if (!context) throw std::runtime_error("Failed to create execution context");

        // Get number of I/O tensors
        int numIOTensors = engine->getNbIOTensors();
        std::cout << "Number of I/O tensors: " << numIOTensors << std::endl;

        // Find input and output tensor indices by name
        int inputIndex = -1;
        int outputIndex = -1;
        for (int i = 0; i < numIOTensors; ++i) {
            const char* tensorName = engine->getIOTensorName(i);
            nvinfer1::Dims dims = engine->getTensorShape(tensorName);
            std::cout << "Tensor " << i << ": " << tensorName << ", Shape: [";
            for (int d = 0; d < dims.nbDims; ++d) {
                std::cout << dims.d[d];
                if (d < dims.nbDims - 1) std::cout << ", ";
            }
            std::cout << "]" << std::endl;

            if (strcmp(tensorName, "onnx::Cast_0") == 0) inputIndex = i;
            if (strcmp(tensorName, "629") == 0) outputIndex = i;
        }
        if (inputIndex == -1 || outputIndex == -1) {
            throw std::runtime_error("Could not find 'onnx::Cast_0' or '629' tensor");
        }

        // Verify input dimensions
        nvinfer1::Dims inputDims = engine->getTensorShape("onnx::Cast_0");
        if (inputDims.nbDims != 4 || inputDims.d[0] != 1 || inputDims.d[1] != 3 ||
            inputDims.d[2] != 640 || inputDims.d[3] != 640) {
            throw std::runtime_error("Unexpected input dimensions");
        }

        // Get output dimensions
        nvinfer1::Dims outputDims = engine->getTensorShape("629");
        int outputSize = 1;
        for (int i = 0; i < outputDims.nbDims; ++i) {
            outputSize *= outputDims.d[i];
        }
        std::cout << "Output size: " << outputSize << " elements" << std::endl;

        // Allocate GPU buffers
        void* buffers[2]; // Input and output buffers
        cudaMalloc(&buffers[inputIndex], input_size * sizeof(float));
        cudaMalloc(&buffers[outputIndex], outputSize * sizeof(float));

        // Copy input data to GPU
        cudaMemcpy(buffers[inputIndex], input_data.data(), input_size * sizeof(float),
                   cudaMemcpyHostToDevice);

        // Perform inference
        context->executeV2(buffers);

        // Copy output data back to host
        std::vector<float> outputData(outputSize);
        cudaMemcpy(outputData.data(), buffers[outputIndex], outputSize * sizeof(float),
                   cudaMemcpyDeviceToHost);

        // Print output (YOLOv5 format: [x_center, y_center, width, height, confidence, class_prob])
        int numDetections = outputDims.d[1]; // 25200 detections
        int numElementsPerDetection = outputDims.d[2]; // 85 elements per detection
        std::cout << "Detected " << numDetections << " objects:" << std::endl;

        int detectionCount = 0;
        for (int i = 0; i < numDetections; ++i) {
            float* detection = outputData.data() + i * numElementsPerDetection;
            float confidence = detection[4]; // Confidence score
            if (confidence > 0.5) { // Threshold for display
                float x_center = detection[0];
                float y_center = detection[1];
                float width = detection[2];
                float height = detection[3];
                std::cout << "Detection " << detectionCount++ << ": "
                          << "x_center=" << x_center << ", y_center=" << y_center
                          << ", width=" << width << ", height=" << height
                          << ", confidence=" << confidence << std::endl;

                int x1 = static_cast<int>(x_center - width / 2);
                int y1 = static_cast<int>(y_center - height / 2);
                int x2 = static_cast<int>(x_center + width / 2);
                int y2 = static_cast<int>(y_center + height / 2);

                // Draw rectangle on the image
                cv::rectangle(image, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 255, 0), 2);

            }
        }

        if (detectionCount == 0) {
            std::cout << "No detections with confidence > 0.5" << std::endl;
        }

        // Clean up GPU memory
        cudaFree(buffers[inputIndex]);
        cudaFree(buffers[outputIndex]);

        std::cout << "Inference completed successfully!" << std::endl;

        cv::imshow("Detections", image);
        cv::waitKey(0);  // Wait for a key press

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}