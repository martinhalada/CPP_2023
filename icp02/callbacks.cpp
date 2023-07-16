#include "callbacks.h"
#include <unordered_set>
#include "camera.h"
#include "camera_opencv.h"
#include <irrklang/irrKlang.h>

extern glm::vec4 color;
extern Camera* camera;
extern s_globals globals;

std::unordered_set<int> keys;

int xpos = 100;
int ypos = 100;
int width = 0;
int height = 0;
bool isFullscreen = false;

irrklang::ISoundEngine* SoundEngine = irrklang::createIrrKlangDevice();
bool playing = false;

void error_callback(int error, const char* description)
{
	std::cerr << "Error: " << description << std::endl;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Vypnutí aplikace
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		return;
	}

	// Fullscreen pøepnutí
	if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		// Get resolution of monitor
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		if (!isFullscreen) {
			glfwGetWindowSize(window, &width, &height);
			glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
			isFullscreen = true;
			glViewport(0, 0, mode->width, mode->height);
			glfwSwapInterval(1); // Nastavení FPS na maximální obnovovací frekvence monitoru

		}
		else {
			glfwSetWindowMonitor(window, nullptr, xpos, ypos, width, height, 0);
			isFullscreen = false;
			glViewport(0, 0, width, height);
			glfwSwapInterval(1); // Nastavení FPS na maximální obnovovací frekvence monitoru
		}
		return;
	}

	if (action == GLFW_PRESS) {
		keys.insert(key);
	}
	else if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_W || key == GLFW_KEY_A || key == GLFW_KEY_S || key == GLFW_KEY_D) {
			SoundEngine->stopAllSounds();
			playing = false;
		}
		keys.erase(key);
	}
}

void process_input(Camera* camera, double delta_t) {

	if (keys.count(GLFW_KEY_A) >= 1) {
		if (!playing) {
			SoundEngine->play2D("resources/walk_sound.wav", true);
			playing = true;
		}
		camera->ProcessInput(Camera::Direction::LEFT, delta_t);
	}
	if (keys.count(GLFW_KEY_D) >= 1) {
		if (!playing) {
			SoundEngine->play2D("resources/walk_sound.wav", true);
			playing = true;
		}
		camera->ProcessInput(Camera::Direction::RIGHT, delta_t);
	}
	if (keys.count(GLFW_KEY_W) >= 1) {
		if (!playing) {
			SoundEngine->play2D("resources/walk_sound.wav", true);
			playing = true;
		}
		camera->ProcessInput(Camera::Direction::UP, delta_t);
	}
	if (keys.count(GLFW_KEY_S) >= 1) {
		if (!playing) {
			SoundEngine->play2D("resources/walk_sound.wav", true);
			playing = true;
		}
		camera->ProcessInput(Camera::Direction::DOWN, delta_t);
	}
	if (keys.count(GLFW_KEY_P) >= 1) {
		globals.useFaceProtection = !globals.useFaceProtection;
	}
}

double xposs = width / 2;
double yposs = height / 2;


void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	GLfloat xoff = xpos - xposs;
	GLfloat yoff = ypos - yposs;
	xposs = xpos;
	yposs = ypos;
	camera->ProcessMouseMovement(xoff, yoff, GL_TRUE);
	//glfwGetWindowSize(window, &width, &height);
	//glfwSetCursorPos(window, width / 2, height / 2);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		std::cout << "Tlaèítko zmáèknuto" << std::endl;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	std::cout << "Scroll: " << xoffset << " " << yoffset << std::endl;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	std::cout << "Framebuffer: " << width << " " << height << std::endl;
}

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void*
	userParam)
{
	auto const src_str = [source]() {
		switch (source)
		{
		case GL_DEBUG_SOURCE_API: return "API";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
		case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
		case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
		case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
		case GL_DEBUG_SOURCE_OTHER: return "OTHER";
		default: return "Unknown";
		}
	}();
	auto const type_str = [type]() {
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR: return "ERROR";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
		case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
		case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
		case GL_DEBUG_TYPE_MARKER: return "MARKER";
		case GL_DEBUG_TYPE_OTHER: return "OTHER";
		default: return "Unknown";
		}
	}();
	auto const severity_str = [severity]() {
		switch (severity) {
		case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
		case GL_DEBUG_SEVERITY_LOW: return "LOW";
		case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
		case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
		default: return "Unknown";
		}
	}();
	std::cout << "[GL CALLBACK]: " <<
		"source = " << src_str <<
		", type = " << type_str <<
		", severity = " << severity_str <<
		", ID = '" << id << '\'' <<
		", message = '" << message << '\'' << std::endl;
}
