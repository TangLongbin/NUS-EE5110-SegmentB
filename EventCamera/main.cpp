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
    std::string outputPath = "./data/SimulatedEventsOutput.mp4";
    std::string eventDataPath = "./data/EventData.csv";
    float threshold = 10.0; // Example threshold value
    float event_fps = 60.0; // Example fps value

    EventCamera eventCamera(videoPath, outputPath, eventDataPath, threshold);
    eventCamera.setEventFPS(event_fps);
    eventCamera.processVideo();

    return 0;
}