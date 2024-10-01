#include "QtWidgetsApplication3.h"
#include <QFileDialog>
#include <QMessageBox>
//#include <QVideoWidget>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <iostream> // 引入输入输出库

struct Event {
    int x;
    int y;
    float time;
};

QtWidgetsApplication3::QtWidgetsApplication3(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    
   // player2->setVideoOutput(ui.videoWidget2);
    player1 = new QMediaPlayer(this);
    player2 = new QMediaPlayer(this);

    // 将播放器与视频部件关联
    player1->setVideoOutput(ui.videoWidget1);
    player2->setVideoOutput(ui.videoWidget2);
    // 连接信号和槽
    connect(ui.pushButtonStart, &QPushButton::clicked, this, &QtWidgetsApplication3::onStartClicked);
    connect(ui.pushButtonStart_2, &QPushButton::clicked, this, &QtWidgetsApplication3::onStartClicked2);
    connect(ui.pushButtonBrowse, &QLineEdit::textChanged, this, &QtWidgetsApplication3::onLineEditTextChanged);
}

QtWidgetsApplication3::~QtWidgetsApplication3()
{}

void QtWidgetsApplication3::onLineEditTextChanged() {
     lineText = ui.pushButtonBrowse->text();//qstring
  //  lineText.append("gg");
    
    
}

void QtWidgetsApplication3::onStartClicked2() {

    fileName_output = QFileDialog::getOpenFileName(this, tr("choose"), "", tr("choose ()"));
}

void QtWidgetsApplication3::onStartClicked()
{
    // ui.pushButtonBrowse->setText(lineText);
    QString fileName = QFileDialog::getOpenFileName(this, tr("choose"), "", tr("choose (*.mp4 *.avi *.mkv)"));


    //fileName = ui.pushButtonBrowse->text();
    //  QString fileName = "D:/test.mp4";
  //  if (fileName.isEmpty())
  //  {
  //      QMessageBox::warning(this, tr("错误"), tr("请先选择视频文件！"));
   //     return;
 //   }
    /*
    QFileInfo checkFile(fileName);
    if (checkFile.exists() && checkFile.isFile()) {
        ui.pushButtonBrowse->setText("1");
    }
    else {
        ui.pushButtonBrowse->setText("0");
    }*/
    std::string str = fileName.toUtf8().constData();

    cv::VideoCapture cap(str);

    ui.pushButtonBrowse->setText("0");
    // 获取视频属性
    int width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    int fps = static_cast<int>(cap.get(cv::CAP_PROP_FPS));

    // 定义事件可视化窗口
    cv::namedWindow("Simulated Events", cv::WINDOW_NORMAL);

    // 定义视频写入对象
    cv::VideoWriter outputVideo;
    cv::Size frameSize(width, height);
    outputVideo.open("SimulatedEventsOutput.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, frameSize, false);



    // 打开事件数据文件
    std::ofstream eventFile("EventData.csv");

    // 写入 CSV 文件头
    eventFile << "Time,X,Y\n";

    // 事件触发阈值
    float threshold = 10.0f;

    cv::Mat frame, grayFrame, lastGrayFrame, diffFrame, eventImage;

    // 读取第一帧
    if (!cap.read(frame)) {
        std::cout << "无法读取视频帧！" << std::endl;
        
    }

    // 转换为灰度并转换为浮点型
    cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
    grayFrame.convertTo(lastGrayFrame, CV_32F);

    int frameIndex = 1; // 第一帧已读取
    try {
        while (cap.read(frame)) {
            // 转换为灰度并转换为浮点型
            cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
            cv::Mat currentGrayFrame;
            grayFrame.convertTo(currentGrayFrame, CV_32F);

            // 计算当前帧与上一帧的差异
            cv::absdiff(currentGrayFrame, lastGrayFrame, diffFrame);

            // 检测事件（像素强度变化超过阈值）
            eventImage = cv::Mat::zeros(height, width, CV_8UC1);
            cv::Mat eventMask = diffFrame >= threshold;
            eventImage.setTo(255, eventMask);

            // 使用 findNonZero 获取事件位置
            std::vector<cv::Point> eventPoints;
            cv::findNonZero(eventMask, eventPoints);

            // 计算当前帧的时间（秒）
            float frameTime = frameIndex / static_cast<float>(fps);

            // // 将事件数据写入文件
             for (const auto& point : eventPoints) {
                 eventFile << frameTime << "," << point.x << "," << point.y << "\n";
             }

            // 显示事件图像
          cv::imshow("Simulated Events", eventImage);
           cv::waitKey(1);

            // 写入输出视频
            outputVideo.write(eventImage);

            // 更新 lastGrayFrame
            lastGrayFrame = currentGrayFrame.clone();

            // 更新帧索引
            ++frameIndex;
        }
    }
    catch (const std::exception& e) {
        std::cout << "发生异常：" << e.what() << std::endl;
    }
    catch (...) {
        std::cout << "发生未知异常" << std::endl;
    }

    // 释放资源
    cap.release();
    outputVideo.release();
    cv::destroyAllWindows();
    eventFile.close();


    QString fileName1 = fileName;
    QString fileName2 = "SimulatedEventsOutput.avi";
    QFileInfo checkFile(fileName_output);
    if (checkFile.exists())
    {
        fileName_output.append("SimulatedEventsOutput.avi");
        fileName2 = fileName_output;
    }
    // 设置媒体源
    ui.pushButtonBrowse->setText("1");
    player1->setMedia(QUrl::fromLocalFile(fileName1));
    player2->setMedia(QUrl::fromLocalFile(fileName2));
   
    // 开始播放
    player1->play();



    // 开始播放
    player2->play();
}