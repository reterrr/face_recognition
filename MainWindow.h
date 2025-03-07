//
// Created by yhwach on 3/7/25.
//

#pragma once

#include <QHBoxLayout>
#include <QMainWindow>
#include <QLabel>
#include <QPushButton>

#include <opencv2/core/mat.hpp>

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

private slots:
    void loadImage();

    void unloadImage();

private:
    void displayImage(const cv::Mat &img) const;

    QHBoxLayout *buttonLayout;
    QVBoxLayout *mainLayout;

    QWidget *centralWidget;
    QLabel *imageLabel;
    QPushButton *loadButton;
    QPushButton *unloadButton;
    cv::Mat processedImage;
};
