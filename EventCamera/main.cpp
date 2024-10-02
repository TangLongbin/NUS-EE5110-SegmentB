#include <iostream>
using namespace std;

#include "EventCamera.hpp"

int main(int argc, char** argv) {
    // Check input arguments
    if (argc != 2) {
        std::cout << "Usage: ./main [video file path]" << std::endl;
        return -1;
    }

    // Create EventCamera object with specified video path, output path, and threshold, then process the video
    std::string videoPath = argv[1];
    // std::string videoPath = "./data/123.mp4";
    std::string outputPath = "./data/SimulatedEventsOutput.avi";
    std::string eventDataPath = "./data/EventData.csv";
    float threshold = 10.0; // Example threshold value
    float event_fps = 300.0; // Example fps value

    // float fx = 1.0, fy = 1.0;
    // float cx = 0.0, cy = 0.0;
    // cv::Mat intrinsic = (cv::Mat_<double>(3, 3) << fx, 0, cx, 0, fy, cy, 0, 0, 1);
    // float k1 = 0.01, k2 = 0.01, p1 = 0.0, p2 = 0.0, k3 = 0.0;
    // cv::Mat distortion = (cv::Mat_<double>(5, 1) << k1, k2, p1, p2, k3);

    EventCamera eventCamera(videoPath, outputPath, eventDataPath, threshold);
    eventCamera.setUseCameraDevice(false);


    // eventCamera.SetIntrinsicParameters(intrinsic);
    // eventCamera.SetDistortionCoefficients(distortion);
    eventCamera.SetRotationMatrix(cv::Mat::eye(3, 3, CV_64F));
    eventCamera.SetTranslationVector(cv::Mat::zeros(3, 1, CV_64F));

    eventCamera.setEventFPS(event_fps);
    eventCamera.setSaveEvents(false);


    eventCamera.processVideo();

    return 0;
}