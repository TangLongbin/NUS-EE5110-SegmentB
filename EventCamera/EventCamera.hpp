#ifndef EVENT_CAMERA_HPP
#define EVENT_CAMERA_HPP

#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <vector>

#include "Camera.hpp"

// EventCamera class inherits from Camera
class EventCamera : public Camera {
public:
    // Constructor to initialize the EventCamera with file paths and threshold
    EventCamera(const std::string& videoFilePath, const std::string& outputVideoPath, const std::string& eventDataPath, float threshold = 10.0f);
    void setFPS(double fps); // Set the frame rate

    // Method to process the video and generate events
    void processVideo();

private:
    std::string videoFilePath;       // Path to the input video file
    std::string outputVideoPath;     // Path to the output video file
    std::string eventDataPath;       // Path to the event data file
    cv::Size frameSize;              // Size of the video frames
    cv::VideoWriter outputVideo;     // Video writer for the output video
    std::ofstream eventFile;         // File stream for the event data file
    float threshold;                 // Threshold for detecting events
    double event_fps;                      // Frames per second of the video
};

// Constructor implementation
EventCamera::EventCamera(const std::string& videoFilePath, const std::string& outputVideoPath, const std::string& eventDataPath, float threshold = 10.0f)
    : videoFilePath(videoFilePath), outputVideoPath(outputVideoPath), eventDataPath(eventDataPath), threshold(threshold) {
    if (!openVideo(videoFilePath)) {
        throw std::runtime_error("Unable to open video file: " + videoFilePath);
    }
    // Open event data file
    eventFile.open(eventDataPath);
    if (!eventFile.is_open()) {
        std::cout << "Unable to create event data file!" << std::endl;
        return;
    }
    frameSize = getResolution();
    event_fps = getFPS(); // Default frame rate
}

// Destructor implementation
EventCamera::~EventCamera() {
    // Release resources
    outputVideo.release();
    cv::destroyAllWindows();
    eventFile.close();
    std::cout << "Event simulation completed, results saved as " << outputVideoPath << " and " << eventDataPath << std::endl;
}

void EventCamera::setFPS(double fps) {
    event_fps = fps;
}

// Method to process the video and generate events
void EventCamera::processVideo() {
    // Define event visualization window
    cv::namedWindow("Simulated Events", cv::WINDOW_NORMAL);

    // Open the output video file
    outputVideo.open(outputVideoPath, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), event_fps, frameSize, false);

    if (!outputVideo.isOpened()) {
        std::cout << "Unable to create output video file!" << std::endl;
        return;
    }

    cv::Mat frame, grayFrame, lastGrayFrame, diffFrame, eventImage;
    
    // Write CSV file header
    eventFile << "Time,X,Y\n";
    
    // Read the first frame
    ReadFrameAndUndistort(frame);

    // Convert to grayscale and then to float type
    cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
    grayFrame.convertTo(lastGrayFrame, CV_32F);

    int frameIndex = 1; // First frame already read
    try {
        while (ReadFrameAndUndistort(frame)) {
            // Convert to grayscale and then to float type
            cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
            cv::Mat currentGrayFrame;
            grayFrame.convertTo(currentGrayFrame, CV_32F);

            // Calculate the difference between the current frame and the last frame
            cv::absdiff(currentGrayFrame, lastGrayFrame, diffFrame);

            // Detect events (pixel intensity changes exceeding the threshold)
            eventImage = cv::Mat::zeros(frameSize, CV_8UC1);
            cv::Mat eventMask = diffFrame >= threshold;
            eventImage.setTo(255, eventMask);
            cv::Mat eventMask = diffFrame <= -threshold;
            eventImage.setTo(128, eventMask);

            // Use findNonZero to get event locations
            std::vector<cv::Point> eventPoints;
            cv::findNonZero(eventMask, eventPoints);

            // Calculate the time of the current frame (seconds)
            double frameTime = static_cast<double>(frameIndex) / event_fps;

            // Write event data to file
            eventFile << std::fixed << std::setprecision(2);
            for (const auto& point : eventPoints) {
                eventFile << frameTime << "," << point.x << "," << point.y << "\n";
            }

            // Display event image
            cv::imshow("Simulated Events", eventImage);
            int delayTime = static_cast<int>(1000 / event_fps);
            cv::waitKey(delayTime);

            // Write to output video
            outputVideo.write(eventImage);

            // Update lastGrayFrame
            lastGrayFrame = currentGrayFrame.clone();

            // Update frame index
            ++frameIndex;
        }
    } catch (const std::exception& e) {
        std::cout << "Exception occurred: " << e.what() << std::endl;
    } catch (...) {
        std::cout << "Unknown exception occurred" << std::endl;
    }
    
}

#endif // EVENT_CAMERA_HPP