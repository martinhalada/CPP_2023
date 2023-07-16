#include "opengl.h"
#include "callbacks.h"

extern GLFWwindow* window;

void init_glew(void)
{
	//
	// Initialize all valid GL extensions with GLEW.
	// Usable AFTER creating GL context!
	//

	GLenum glew_ret;
	glew_ret = glewInit();
	if (glew_ret != GLEW_OK)
	{
		std::cerr << "WGLEW failed with error: " << glewGetErrorString(glew_ret) << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		std::cout << "GLEW successfully initialized to version: " << glewGetString(GLEW_VERSION) << std::endl;
	}
	// Platform specific. (Change to GLXEW or ELGEW if necessary.)
	glew_ret = wglewInit();
	if (glew_ret != GLEW_OK)
	{
		std::cerr << "WGLEW failed with error: " << glewGetErrorString(glew_ret) << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		std::cout << "WGLEW successfully initialized platform specific functions." << std::endl;
	}
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(MessageCallback, 0);
}

void init_glfw(void)
{
	// Nastavení error callbacku
	glfwSetErrorCallback(error_callback);

	//Inicializace GLFW knihovny
	int glfw_ret = glfwInit();

	if (!glfw_ret) {
		std::cerr << "GLFW init failed." << std::endl;
		exit(EXIT_FAILURE);
	}

	// Nastavení verze OpenGL 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // only new functions

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing


	window = glfwCreateWindow(800, 600, "OpenGL context", NULL, NULL); // Vytvoøení okna

	if (!window) {
		std::cerr << "GLFW window creation error." << std::endl;
		exit(EXIT_FAILURE);
	}

	// Nastavení kontextu pro aktuální vytvoøené okno
	glfwMakeContextCurrent(window);

	// Nastavení výmìny bufferù pøi obnovení obrazovky (FPS = Hz)
	glfwSwapInterval(1);

	glfwSetKeyCallback(window, key_callback); // Nastavení callbacku pøi zmáèknutí klávesy
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // Nastavení callbacku pøi zmìnì velikosti okna
	glfwSetMouseButtonCallback(window, mouse_button_callback); // Nastavení callbacku pøi stistku tlaèítka na myši
	glfwSetScrollCallback(window, scroll_callback); // Nastavení callbacku pøi zmáèknutí klávesy
	glfwSetCursorPosCallback(window, cursor_pos_callback); // Nastavení callbacku pøi zmáèknutí klávesy
}
