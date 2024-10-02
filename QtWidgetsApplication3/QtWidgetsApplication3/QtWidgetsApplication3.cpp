#include "QtWidgetsApplication3.h"
#include <QFileDialog>
#include <QMessageBox>
//#include <QVideoWidget>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <iostream> // Include input/output library

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

    // Associate the player with the video widget
    player1->setVideoOutput(ui.videoWidget1);
    player2->setVideoOutput(ui.videoWidget2);
    // Connect signals and slots
    connect(ui.pushButtonStart, &QPushButton::clicked, this, &QtWidgetsApplication3::onStartClicked);
    connect(ui.pushButtonStart_2, &QPushButton::clicked, this, &QtWidgetsApplication3::onStartClicked2);
    connect(ui.pushButtonBrowse, &QLineEdit::textChanged, this, &QtWidgetsApplication3::onLineEditTextChanged);
}

QtWidgetsApplication3::~QtWidgetsApplication3()
{}

void QtWidgetsApplication3::onLineEditTextChanged() {
     lineText = ui.pushButtonBrowse->text(); // qstring
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
  //      QMessageBox::warning(this, tr("Error"), tr("Please select a video file first!"));
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
    // Get video properties
    int width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    int fps = static_cast<int>(cap.get(cv::CAP_PROP_FPS));

    // Define event visualization window
    cv::namedWindow("Simulated Events", cv::WINDOW_NORMAL);

    // Define video writer object
    cv::VideoWriter outputVideo;
    cv::Size frameSize(width, height);
    outputVideo.open("SimulatedEventsOutput.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, frameSize, false);

    // Open event data file
    std::ofstream eventFile("EventData.csv");

    // Write CSV file header
    eventFile << "Time,X,Y\n";

    // Event trigger threshold
    float threshold = 10.0f;

    cv::Mat frame, grayFrame, lastGrayFrame, diffFrame, eventImage;

    // Read the first frame
    if (!cap.read(frame)) {
        std::cout << "Unable to read video frame!" << std::endl;
    }

    // Convert to grayscale and convert to float type
    cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
    grayFrame.convertTo(lastGrayFrame, CV_32F);

    int frameIndex = 1; // First frame has been read
    try {
        while (cap.read(frame)) {
            // Convert to grayscale and convert to float type
            cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
            cv::Mat currentGrayFrame;
            grayFrame.convertTo(currentGrayFrame, CV_32F);

            // Calculate the difference between the current frame and the previous frame
            cv::absdiff(currentGrayFrame, lastGrayFrame, diffFrame);

            // Detect events (pixel intensity changes exceeding the threshold)
            eventImage = cv::Mat::zeros(height, width, CV_8UC1);
            cv::Mat eventMask = diffFrame >= threshold;
            eventImage.setTo(255, eventMask);

            // Use findNonZero to get event positions
            std::vector<cv::Point> eventPoints;
            cv::findNonZero(eventMask, eventPoints);

            // Calculate the time of the current frame (seconds)
            float frameTime = frameIndex / static_cast<float>(fps);

            // Write event data to file
            for (const auto& point : eventPoints) {
                eventFile << frameTime << "," << point.x << "," << point.y << "\n";
            }

            // Display event image
            cv::imshow("Simulated Events", eventImage);
            cv::waitKey(1);

            // Write to output video
            outputVideo.write(eventImage);

            // Update lastGrayFrame
            lastGrayFrame = currentGrayFrame.clone();

            // Update frame index
            ++frameIndex;
        }
    }
    catch (const std::exception& e) {
        std::cout << "Exception occurred: " << e.what() << std::endl;
    }
    catch (...) {
        std::cout << "Unknown exception occurred" << std::endl;
    }

    // Release resources
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
    // Set media source
    ui.pushButtonBrowse->setText("1");
    player1->setMedia(QUrl::fromLocalFile(fileName1));
    player2->setMedia(QUrl::fromLocalFile(fileName2));
   
    // Start playback
    player1->play();

    // Start playback
    player2->play();
}
