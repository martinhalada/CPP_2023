#pragma once

#include <iostream>
#include <numeric>
#include <thread>

#include <opencv2/opencv.hpp>
// OpenGL Extension Wrangler
#include <GL/glew.h> 
#include <GL/wglew.h> //WGLEW = Windows GL Extension Wrangler (change for different platform) 
// GLFW toolkit
#include <GLFW/glfw3.h>
// OpenGL math
#include <glm/glm.hpp>

typedef struct s_globals {
	cv::VideoCapture capture;
	bool useFaceProtection = false;
	bool faceDetected = false;
} s_globals;

bool find_center_face(cv::Mat& frame);
void find_center(s_globals* globals);
