//
// Created by yhwach on 3/7/25.
//

#pragma once

#include <opencv2/opencv.hpp>

class ImageProcessor {
public:
    void process(const cv::Mat& image, cv::Mat& result);
};