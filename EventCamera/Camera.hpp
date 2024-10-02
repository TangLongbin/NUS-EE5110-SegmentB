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

public:
    Camera(const Mat& intrinsic, const Mat& distortion, const Mat& rotation, const Mat& translation);
    ~Camera();
    void undistortImage(const Mat& inputImage, Mat& outputImage) const;
    void SetIntrinsicParameters(const Mat& intrinsic);
    void SetDistortionCoefficients(const Mat& distortion);
    void SetRotationMatrix(const Mat& rotation);
    void SetTranslationVector(const Mat& translation);
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
Camera::~Camera(){}

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

#endif // CAMERA_HPP