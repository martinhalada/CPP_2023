#include "camera_opencv.h"

cv::Mat framec;
static std::mutex m;
extern GLFWwindow* window;

cv::CascadeClassifier face_cascade = cv::CascadeClassifier("resources/haarcascade_frontalface_default.xml");

bool find_center_face(cv::Mat& frame) {
	cv::Point2f center;
	cv::Mat scene_gray;
	cv::cvtColor(frame, scene_gray, cv::COLOR_BGR2GRAY);

	std::vector<cv::Rect> faces;
	face_cascade.detectMultiScale(scene_gray, faces);
	if (faces.size() > 0)
	{
		return true;
	}
	return false;
}
//cont && !glfwWindowShouldClose(window)
void find_center(s_globals* globals) {
	cv::Mat frame;
	int detection_count = 0;
	int last_frame_face_detected = true;
	int detection_limit = 20;
	while (true) {
		// Kontrola každou sekundu, zda je ochrana zapnutá
		if (!globals->useFaceProtection) {
			globals->faceDetected = true;
			Sleep(1000);
			continue;
		}

		// Naètení snímku kamery
		globals->capture.read(frame);
		if (frame.empty()) {
			std::cerr << "Kamera odpojena v prùbìhu programu!" << std::endl;
			exit(EXIT_FAILURE);
		}

		bool face_detected = find_center_face(frame);

		if (face_detected && last_frame_face_detected) {
			detection_count++;
		}
		else if (!face_detected && !last_frame_face_detected) {
			detection_count++;
		}
		else if (face_detected && !last_frame_face_detected) {
			detection_count = 1;
			last_frame_face_detected = true;
		}
		else if (!face_detected && last_frame_face_detected) {
			detection_count = 1;
			last_frame_face_detected = false;
		}

		if (detection_count >= detection_limit) {
			detection_count = 0;

			if (last_frame_face_detected) {
				globals->faceDetected = true;
			}
			else {
				globals->faceDetected = false;
			}
		}

	}
}
