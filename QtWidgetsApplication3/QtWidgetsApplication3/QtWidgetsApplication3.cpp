#include "QtWidgetsApplication3.h"
#include <QFileDialog>
#include <QMessageBox>
//#include <QVideoWidget>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <iostream> // �������������

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

    // ������������Ƶ��������
    player1->setVideoOutput(ui.videoWidget1);
    player2->setVideoOutput(ui.videoWidget2);
    // �����źźͲ�
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
  //      QMessageBox::warning(this, tr("����"), tr("����ѡ����Ƶ�ļ���"));
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
    // ��ȡ��Ƶ����
    int width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    int fps = static_cast<int>(cap.get(cv::CAP_PROP_FPS));

    // �����¼����ӻ�����
    cv::namedWindow("Simulated Events", cv::WINDOW_NORMAL);

    // ������Ƶд�����
    cv::VideoWriter outputVideo;
    cv::Size frameSize(width, height);
    outputVideo.open("SimulatedEventsOutput.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, frameSize, false);



    // ���¼������ļ�
    std::ofstream eventFile("EventData.csv");

    // д�� CSV �ļ�ͷ
    eventFile << "Time,X,Y\n";

    // �¼�������ֵ
    float threshold = 10.0f;

    cv::Mat frame, grayFrame, lastGrayFrame, diffFrame, eventImage;

    // ��ȡ��һ֡
    if (!cap.read(frame)) {
        std::cout << "�޷���ȡ��Ƶ֡��" << std::endl;
        
    }

    // ת��Ϊ�ҶȲ�ת��Ϊ������
    cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
    grayFrame.convertTo(lastGrayFrame, CV_32F);

    int frameIndex = 1; // ��һ֡�Ѷ�ȡ
    try {
        while (cap.read(frame)) {
            // ת��Ϊ�ҶȲ�ת��Ϊ������
            cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
            cv::Mat currentGrayFrame;
            grayFrame.convertTo(currentGrayFrame, CV_32F);

            // ���㵱ǰ֡����һ֡�Ĳ���
            cv::absdiff(currentGrayFrame, lastGrayFrame, diffFrame);

            // ����¼�������ǿ�ȱ仯������ֵ��
            eventImage = cv::Mat::zeros(height, width, CV_8UC1);
            cv::Mat eventMask = diffFrame >= threshold;
            eventImage.setTo(255, eventMask);

            // ʹ�� findNonZero ��ȡ�¼�λ��
            std::vector<cv::Point> eventPoints;
            cv::findNonZero(eventMask, eventPoints);

            // ���㵱ǰ֡��ʱ�䣨�룩
            float frameTime = frameIndex / static_cast<float>(fps);

            // // ���¼�����д���ļ�
             for (const auto& point : eventPoints) {
                 eventFile << frameTime << "," << point.x << "," << point.y << "\n";
             }

            // ��ʾ�¼�ͼ��
          cv::imshow("Simulated Events", eventImage);
           cv::waitKey(1);

            // д�������Ƶ
            outputVideo.write(eventImage);

            // ���� lastGrayFrame
            lastGrayFrame = currentGrayFrame.clone();

            // ����֡����
            ++frameIndex;
        }
    }
    catch (const std::exception& e) {
        std::cout << "�����쳣��" << e.what() << std::endl;
    }
    catch (...) {
        std::cout << "����δ֪�쳣" << std::endl;
    }

    // �ͷ���Դ
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
    // ����ý��Դ
    ui.pushButtonBrowse->setText("1");
    player1->setMedia(QUrl::fromLocalFile(fileName1));
    player2->setMedia(QUrl::fromLocalFile(fileName2));
   
    // ��ʼ����
    player1->play();



    // ��ʼ����
    player2->play();
}