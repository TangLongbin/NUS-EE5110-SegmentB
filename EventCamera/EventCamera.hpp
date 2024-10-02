#ifndef EVENT_CAMERA_HPP
#define EVENT_CAMERA_HPP

#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <vector>

class EventCamera {
public:
    EventCamera(const std::string& videoFilePath, const std::string& outputVideoPath, const std::string& eventDataPath, float threshold = 10.0f);
    void processVideo();

private:
    std::string videoFilePath;
    std::string outputVideoPath;
    std::string eventDataPath;
    float threshold;
};

EventCamera::EventCamera(const std::string& videoFilePath, const std::string& outputVideoPath, const std::string& eventDataPath, float threshold)
    : videoFilePath(videoFilePath), outputVideoPath(outputVideoPath), eventDataPath(eventDataPath), threshold(threshold) {}

void EventCamera::processVideo() {
    // Open video file
    cv::VideoCapture cap(videoFilePath);
    if (!cap.isOpened()) {
        std::cout << "Unable to open video file!" << std::endl;
        return;
    }

    // Get video properties
    int width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    int fps = static_cast<int>(cap.get(cv::CAP_PROP_FPS));

    // Define event visualization window
    cv::namedWindow("Simulated Events", cv::WINDOW_NORMAL);

    // Define video writer object
    cv::VideoWriter outputVideo;
    cv::Size frameSize(width, height);
    outputVideo.open(outputVideoPath, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, frameSize, false);

    if (!outputVideo.isOpened()) {
        std::cout << "Unable to create output video file!" << std::endl;
        return;
    }

    // Open event data file
    std::ofstream eventFile(eventDataPath);
    if (!eventFile.is_open()) {
        std::cout << "Unable to create event data file!" << std::endl;
        return;
    }
    // Write CSV file header
    eventFile << "Time,X,Y\n";

    cv::Mat frame, grayFrame, lastGrayFrame, diffFrame, eventImage;

    // Read the first frame
    if (!cap.read(frame)) {
        std::cout << "Unable to read video frame!" << std::endl;
        return;
    }

    // Convert to grayscale and then to float type
    cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
    grayFrame.convertTo(lastGrayFrame, CV_32F);

    int frameIndex = 1; // First frame already read
    try {
        while (cap.read(frame)) {
            // Convert to grayscale and then to float type
            cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
            cv::Mat currentGrayFrame;
            grayFrame.convertTo(currentGrayFrame, CV_32F);

            // Calculate the difference between the current frame and the last frame
            cv::absdiff(currentGrayFrame, lastGrayFrame, diffFrame);

            // Detect events (pixel intensity changes exceeding the threshold)
            eventImage = cv::Mat::zeros(height, width, CV_8UC1);
            cv::Mat eventMask = diffFrame >= threshold;
            eventImage.setTo(255, eventMask);

            // Use findNonZero to get event locations
            std::vector<cv::Point> eventPoints;
            cv::findNonZero(eventMask, eventPoints);

            // Calculate the time of the current frame (seconds)
            float frameTime = frameIndex / static_cast<float>(fps);

            // Write event data to file
            eventFile << std::fixed << std::setprecision(2);
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
    } catch (const std::exception& e) {
        std::cout << "Exception occurred: " << e.what() << std::endl;
    } catch (...) {
        std::cout << "Unknown exception occurred" << std::endl;
    }

    // Release resources
    cap.release();
    outputVideo.release();
    cv::destroyAllWindows();
    eventFile.close();

    std::cout << "Event simulation completed, results saved as " << outputVideoPath << " and " << eventDataPath << std::endl;
}

#endif // EVENT_CAMERA_HPP