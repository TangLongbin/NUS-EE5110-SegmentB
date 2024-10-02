#include "EventCamera.hpp"
#include "Camera.hpp"

int main(int argc, char** argv) {
    // Check input arguments
    if (argc != 2) {
        std::cout << "Usage: ./test [video file path]" << std::endl;
        return -1;
    }

    // Create EventCamera object and process the video
    EventCamera eventCamera(argv[1], "SimulatedEventsOutput.avi", "EventData.csv");
    eventCamera.processVideo();

    return 0;
}