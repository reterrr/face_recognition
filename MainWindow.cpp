#include "MainWindow.h"
#include <QVBoxLayout>

#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    imageLabel = new QLabel(centralWidget);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setMinimumSize(400, 300);

    loadButton = new QPushButton("Load Image", centralWidget);
    unloadButton = new QPushButton("Unload Image", centralWidget);

    connect(loadButton, &QPushButton::clicked, this, &MainWindow::loadImage);
    connect(unloadButton, &QPushButton::clicked, this, &MainWindow::unloadImage);

    buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(loadButton);
    buttonLayout->addWidget(unloadButton);

    mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addWidget(imageLabel);
    mainLayout->addLayout(buttonLayout);

    setWindowTitle("Image Processor");
    resize(600, 500);
}

MainWindow::~MainWindow() {
    delete imageLabel;
    delete unloadButton;
    delete buttonLayout;
    delete mainLayout;
}

void MainWindow::loadImage()
{
    const QString fileName = QFileDialog::getOpenFileName(this, "Open Image", QString(),
                                                       "Images (*.png *.jpg *.jpeg *.bmp)");
    if (fileName.isEmpty())
        return;

    const cv::Mat image = cv::imread(fileName.toStdString());
    if (image.empty()) {
        QMessageBox::warning(this, "Error", "Failed to load image.");

        return;
    }

    processor.process(image, processed_image);

    refresh_display_image();
}

void MainWindow::unloadImage()
{
    imageLabel->clear();
    processed_image.release();
}

void MainWindow::refresh_display_image() const {
    cv::Mat rgb;
    cvtColor(processed_image, rgb, cv::COLOR_BGR2RGB);

    const QImage qimg(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888);

    imageLabel->setPixmap(QPixmap::fromImage(qimg)
                          .scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
