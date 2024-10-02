#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

/**
 * @class Camera
 * @brief A class to represent a camera with intrinsic and extrinsic parameters.
 * 
 * This class provides functionalities to store and manipulate camera parameters,
 * including intrinsic parameters, distortion coefficients, rotation matrix, and
 * translation vector. It also provides a method to undistort images based on the
 * camera parameters.
 */
class Camera{

private:
    Mat intrinsicParameters;
    Mat distortionCoefficients;
    Mat rotationMatrix;
    Mat translationVector;
    VideoCapture videoCapture;
    Size resolution;
    double fps; // Add a member variable to store the frame rate

public:
    Camera(const Mat& intrinsic, const Mat& distortion, const Mat& rotation, const Mat& translation);
    ~Camera();
    void undistortImage(const Mat& inputImage, Mat& outputImage) const; // Undistorts an input image using the camera's intrinsic and distortion parameters.
    void SetIntrinsicParameters(const Mat& intrinsic); // Sets the intrinsic parameters of the camera.
    void SetDistortionCoefficients(const Mat& distortion); // Sets the distortion coefficients of the camera.
    void SetRotationMatrix(const Mat& rotation); // Sets the rotation matrix of the camera.
    void SetTranslationVector(const Mat& translation); // Sets the translation vector of the camera.
    bool openVideo(const string& filePath); // Opens a video file and updates the resolution and frame rate.
    Size getResolution() const; // Gets the resolution of the video.
    double getFPS() const; // Gets the frame rate of the video.
    bool ReadFrameAndUndistort(Mat& undistortedFrame); // Gets the next frame from the video, undistorts it, and returns the undistorted frame.
};

/**
 * @brief Constructs a Camera object with the given parameters.
 * 
 * @param intrinsic The intrinsic parameters of the camera.
 * @param distortion The distortion coefficients of the camera.
 * @param rotation The rotation matrix of the camera.
 * @param translation The translation vector of the camera.
 */
Camera::Camera(const Mat& intrinsic = Mat::eye(3, 3, CV_64F), 
               const Mat& distortion = Mat::zeros(5, 1, CV_64F), 
               const Mat& rotation = Mat::eye(3, 3, CV_64F), 
               const Mat& translation = Mat::zeros(3, 1, CV_64F)){
    intrinsicParameters = intrinsic;
    distortionCoefficients = distortion;
    rotationMatrix = rotation;
    translationVector = translation;
}

/**
 * @brief Destroys the Camera object.
 */
Camera::~Camera(){
    if (videoCapture.isOpened()) {
        videoCapture.release();
    }
}

/**
 * @brief Undistorts an input image using the camera's intrinsic and distortion parameters.
 * 
 * @param inputImage The distorted input image.
 * @param outputImage The undistorted output image.
 */
void Camera::undistortImage(const Mat& inputImage, Mat& outputImage) const{
    undistort(inputImage, outputImage, intrinsicParameters, distortionCoefficients);
}

/**
 * @brief Sets the intrinsic parameters of the camera.
 * 
 * @param intrinsic The new intrinsic parameters.
 */
void Camera::SetIntrinsicParameters(const Mat& intrinsic){
    intrinsicParameters = intrinsic;
}

/**
 * @brief Sets the distortion coefficients of the camera.
 * 
 * @param distortion The new distortion coefficients.
 */
void Camera::SetDistortionCoefficients(const Mat& distortion){
    distortionCoefficients = distortion;
}

/**
 * @brief Sets the rotation matrix of the camera.
 * 
 * @param rotation The new rotation matrix.
 */
void Camera::SetRotationMatrix(const Mat& rotation){
    rotationMatrix = rotation;
}

/**
 * @brief Sets the translation vector of the camera.
 * 
 * @param translation The new translation vector.
 */
void Camera::SetTranslationVector(const Mat& translation){
    translationVector = translation;
}

/**
 * @brief Opens a video file and updates the resolution and frame rate.
 * 
 * @param filePath The path to the video file.
 * @return True if the video file was successfully opened, false otherwise.
 */
bool Camera::openVideo(const string& filePath) {
    videoCapture.open(filePath);
    if (!videoCapture.isOpened()) {
        cerr << "Error: Could not open video file." << endl;
        return false;
    }
    resolution = Size(static_cast<int>(videoCapture.get(CAP_PROP_FRAME_WIDTH)),
                      static_cast<int>(videoCapture.get(CAP_PROP_FRAME_HEIGHT)));
    fps = videoCapture.get(CAP_PROP_FPS); // Get the frame rate
    return true;
}

/**
 * @brief Gets the resolution of the video.
 * 
 * @return The resolution of the video.
 */
Size Camera::getResolution() const {
    return resolution;
}

/**
 * @brief Gets the frame rate of the video.
 * 
 * @return The frame rate of the video.
 */
double Camera::getFPS() const {
    return fps;
}

/**
 * @brief Gets the next frame from the video, undistorts it, and returns the undistorted frame.
 * 
 * @param undistortedFrame The undistorted frame.
 * @return True if the frame was successfully read and undistorted, false otherwise.
 */
bool Camera::ReadFrameAndUndistort(Mat& undistortedFrame) {
    Mat frame;
    if (videoCapture.isOpened()) {
        videoCapture >> frame;
        if (frame.empty()) {
            cerr << "Error: Could not read frame from video." << endl;
            return false;
        }
        undistortImage(frame, undistortedFrame);
        return true;
    }
    return false;
}

#endif // CAMERA_HPP