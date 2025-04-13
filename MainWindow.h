//
// Created by yhwach on 3/7/25.
//

#pragma once

#include <QHBoxLayout>
#include <QMainWindow>
#include <QLabel>
#include <QPushButton>

#include <opencv2/core/mat.hpp>

#include "ImageProcessor.h"

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

private slots:
    void loadImage();

    void unloadImage();

private:
    void refresh_display_image() const;

    QHBoxLayout *buttonLayout;
    QVBoxLayout *mainLayout;

    QWidget *centralWidget;
    QLabel *imageLabel;
    QPushButton *loadButton;
    QPushButton *unloadButton;
    cv::Mat processed_image;

    ImageProcessor processor;
};
