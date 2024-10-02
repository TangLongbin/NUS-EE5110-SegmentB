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
    ~EventCamera(); // Destructor
    void setEventFPS(double _event_fps); // Set the frame rate
    double getEventFPS() const; // Get the frame rate

    // Method to process the video and generate events
    void processVideo();
    void setSaveEvents(bool save);

private:
    std::string videoFilePath;       // Path to the input video file
    std::string outputVideoPath;     // Path to the output video file
    std::string eventDataPath;       // Path to the event data file
    cv::Size frameSize;              // Size of the video frames
    cv::VideoWriter outputVideo;     // Video writer for the output video
    std::ofstream eventFile;         // File stream for the event data file
    float threshold;                 // Threshold for detecting events
    double event_fps;                   // Frames per second of the video
    bool saveEvents = false;                 // Flag to save events to file
};

// Constructor implementation
EventCamera::EventCamera(const std::string& videoFilePath, const std::string& outputVideoPath, const std::string& eventDataPath, float threshold)
    : videoFilePath(videoFilePath), outputVideoPath(outputVideoPath), eventDataPath(eventDataPath), threshold(threshold) {
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

/**
 * @brief Sets the event frames per second (FPS) for the EventCamera.
 * 
 * This function allows you to specify the rate at which events are processed
 * by the EventCamera. The event FPS determines how frequently the camera
 * captures and processes events.
 * 
 * @param _event_fps The desired event frames per second (FPS) value.
 */
void EventCamera::setEventFPS(double _event_fps) {
    event_fps = _event_fps;
}

/**
 * @brief Retrieves the frames per second (FPS) for events.
 * 
 * This function returns the current event FPS, which indicates the 
 * rate at which events are processed or captured by the event camera.
 * 
 * @return double The current event FPS.
 */
double EventCamera::getEventFPS() const {
    return event_fps;
}

// Method to process the video and generate events
void EventCamera::processVideo() {
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

    // Define original visualization window
    cv::namedWindow("Original Frames", cv::WINDOW_NORMAL);

    // Define event visualization window
    cv::namedWindow("Simulated Events", cv::WINDOW_NORMAL);

    // Open the output video file
    if (outputVideoPath.substr(outputVideoPath.find_last_of(".") + 1) == "mp4") {
        outputVideo.open(outputVideoPath, cv::VideoWriter::fourcc('H', '2', '6', '4'), event_fps, frameSize); // mp4 format
    } else if (outputVideoPath.substr(outputVideoPath.find_last_of(".") + 1) == "avi") {
        outputVideo.open(outputVideoPath, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), event_fps, frameSize); // avi format
    } else {
        std::cout << "Unsupported video format!" << std::endl;
        return;
    }

    if (!outputVideo.isOpened()) {
        std::cout << "Unable to create output video file!" << std::endl;
        return;
    }

    cv::Mat frame, grayFrame, lastGrayFrame, diffFrame, eventImage;
    
    // Write CSV file header
    eventFile << "Time,X,Y,Event\n";
    
    // Read the first frame
    ReadFrameAndUndistort(frame);

    // Convert to grayscale and then to float type
    cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
    grayFrame.convertTo(lastGrayFrame, CV_16SC1);

    int frameIndex = 1; // First frame already read
    try {
        while (ReadFrameAndUndistort(frame)) {
            // Display original frame
            cv::imshow("Original Frames", frame);

            // Convert to grayscale and then to float type
            cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
            cv::Mat currentGrayFrame;
            grayFrame.convertTo(currentGrayFrame, CV_16SC1);

            // Calculate the difference between the current frame and the last frame
            diffFrame = currentGrayFrame - lastGrayFrame;

            // Detect events (pixel intensity changes exceeding the threshold)
            eventImage = cv::Mat::zeros(frameSize, CV_8UC3);
            cv::Mat eventMask = diffFrame >= threshold;
            eventImage.setTo(cv::Scalar(0, 0, 255), eventMask); // Red for positive changes
            eventMask = diffFrame <= -threshold;
            eventImage.setTo(cv::Scalar(255, 0, 0), eventMask); // Blue for negative changes

            // Use findNonZero to get event locations
            std::vector<cv::Point> eventPoints;
            cv::cvtColor(eventImage, eventMask, cv::COLOR_BGR2GRAY);
            cv::findNonZero(eventMask, eventPoints);

            // Calculate the time of the current frame (seconds)
            double frameTime = static_cast<double>(frameIndex) / event_fps;

            // Write event data to file
            if(saveEvents){
                for (const auto& point : eventPoints) {
                    int eventValue = eventImage.at<cv::Vec3b>(point)[2] == 255 ? 1 : -1;
                    eventFile << frameTime << "," << point.x << "," << point.y << eventValue << "\n";
                }
            }
            
            // Display event image
            cv::imshow("Simulated Events", eventImage);
            // int delayTime = static_cast<int>(1000 / event_fps);
            // cv::waitKey(delayTime);
            cv::waitKey(1); // minimal delay

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

void EventCamera::setSaveEvents(bool save) {
    saveEvents = save;
}

#endif // EVENT_CAMERA_HPP