#include <iostream>
#include <numeric>
#include <opencv2/opencv.hpp>
#include <thread>
#include <chrono>

// OpenGL Extension Wrangler
#include <GL/glew.h> 
#include <GL/wglew.h> //WGLEW = Windows GL Extension Wrangler (change for different platform) 
// GLFW toolkit
#include <GLFW/glfw3.h>
// OpenGL math
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "callbacks.h"
#include "camera_opencv.h"
#include "opengl.h"
#include "shaders.h"
#include "camera.h"
#include "OBJloader.h"
#include "model.h"
#include <string>

s_globals globals;
GLFWwindow* window;
Camera* camera = new Camera(glm::vec3(0.0f, 0.0f, -2.0f));

int main()
{


	// Incializace GLFW a GLEW (OpenGL Extension Wrangler)
	init_glfw();
	init_glew();

	// Zapnutí Z-bufferu
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_LINE_SMOOTH);

	// Nastavení blend funkce
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Inicializace kamery
	globals.capture = cv::VideoCapture(cv::CAP_DSHOW);

	// Kontrola zda je kamera připojená
	if (!globals.capture.isOpened())
	{
		std::cerr << "no camera" << std::endl;
	}
	else {
		//Detekce obličeje v další vlákně
		std::thread image_procesing_thread(find_center, &globals);
		image_procesing_thread.detach();
	}

	// Kontrola, zda je GLFW inicializování
	if (!glfwInit() || !window) {
		glfwTerminate();
		return -1;
	}

	// Načtení shaderů
	GLuint prog = loadShaders("resources/triangle");

	// Načtení modelů
	Model height_map = Model("resources/heightmap.png", prog, "heightMap");
	height_map.loadTexture("resources/textures/tex_256.png");

	Model m = Model("resources/obj/Tree.obj", prog, "fromFile");
	m.loadTexture("resources/textures/Fabric.jpg");

	Model m2 = Model("resources/obj/teapot_tri_vnt.obj", prog, "fromFile");
	m2.loadTexture("resources/textures/stoneFloor.jpg");

	Model m3 = Model("resources/obj/Bottle.obj", prog, "fromFile");
	m3.loadTexture("resources/textures/matne_sklo.png");

	try {

		bool collision = false;
		int width, height;
		glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);

		float ratio = static_cast<float>(width) / height;

		glm::mat4 projectionMatrix = glm::perspective(
			glm::radians(60.0f), // The vertical Field of View, in radians: the amount of "zoom". Think "camera lens". Usually between 90� (extra wide) and 30� (quite zoomed in)
			ratio,			     // Aspect Ratio. Depends on the size of your window.
			0.1f,                // Near clipping plane. Keep as big as possible, or you'll get precision issues.
			20000.0f              // Far clipping plane. Keep as little as possible.
		);

		//set uniform for shaders - projection matrix


		// set visible area
		glViewport(0, 0, width, height);

		using clock = std::chrono::high_resolution_clock;
		auto start = clock::now();
		float delta_t = 0.005f;

		// Umístění kamery do scény
		camera->Position.z = 0;
		camera->Position.x = 0;

		//Nastavení shaderů a světel
		glUseProgram(prog);

		// directional light
		glUniform3f(glGetUniformLocation(prog, "dirLight.direction"), 100.0f, 100.0f, 100.0f);
		glUniform3f(glGetUniformLocation(prog, "dirLight.ambient"), 0.4f, 0.4f, 0.4f);
		glUniform3f(glGetUniformLocation(prog, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
		glUniform3f(glGetUniformLocation(prog, "dirLight.specular"), 0.1f, 0.1f, 0.1f);
		// point light 1
		glUniform3f(glGetUniformLocation(prog, "pointLights[0].position"), 73.0f, 25.0f, 100.0f);
		glUniform3f(glGetUniformLocation(prog, "pointLights[0].ambient"), 5.5f, 5.5f, 5.5f);
		glUniform3f(glGetUniformLocation(prog, "pointLights[0].diffuse"), 5.5f, 5.5f, 5.5f);
		glUniform3f(glGetUniformLocation(prog, "pointLights[0].specular"), 5.5f, 5.5f, 5.5f);
		glUniform1f(glGetUniformLocation(prog, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(prog, "pointLights[0].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(prog, "pointLights[0].quadratic"), 0.032f);
		// point light 2
		glUniform3f(glGetUniformLocation(prog, "pointLights[1].position"), 180.3f, 30.0f, 66.0f);
		glUniform3f(glGetUniformLocation(prog, "pointLights[1].ambient"), 5.5f, 5.5f, 5.5f);
		glUniform3f(glGetUniformLocation(prog, "pointLights[1].diffuse"), 5.5f, 5.5f, 5.5f);
		glUniform3f(glGetUniformLocation(prog, "pointLights[1].specular"), 5.5f, 5.5f, 5.5f);
		glUniform1f(glGetUniformLocation(prog, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(prog, "pointLights[1].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(prog, "pointLights[1].quadratic"), 0.032f);
		// spotLight
		glUniform3f(glGetUniformLocation(prog, "spotLight.position"), 10.0f, 20.0f, 76.0f);
		glUniform3f(glGetUniformLocation(prog, "spotLight.direction"), 0.0f, 0.0f, 90.0f);
		glUniform3f(glGetUniformLocation(prog, "spotLight.ambient"), 5.0f, 5.0f, 5.0f);
		glUniform3f(glGetUniformLocation(prog, "spotLight.diffuse"), 5.0f, 5.0f, 5.0f);
		glUniform3f(glGetUniformLocation(prog, "spotLight.specular"), 0.2f, 0.2f, 0.2f);
		glUniform1f(glGetUniformLocation(prog, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(prog, "spotLight.linear"), 0.09f);
		glUniform1f(glGetUniformLocation(prog, "spotLight.quadratic"), 0.032f);
		glUniform1f(glGetUniformLocation(prog, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
		glUniform1f(glGetUniformLocation(prog, "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));

		// Hlavní vykreslovací smyčka
		while (!glfwWindowShouldClose(window)) {
			if (true) {

				start = clock::now();
				glm::vec3 camera_backup = glm::vec3(camera->Position.x, camera->Position.y, camera->Position.z);
				process_input(camera, delta_t);

				// Výpočet aktuální výšky kamery podle pozice na mapě
				float x = camera->Position.x;
				float z = camera->Position.z;
				camera->Position.y = height_map.getHeightAt(x, z, 1);

				/* Render here */
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


				{
					glUniformMatrix4fv(glGetUniformLocation(prog, "uProj_m"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
					glUniform3fv(glGetUniformLocation(prog, "camPos"), 1, &camera->Position[0]);

					// Nastavení model matrix pro každý model

					// Vykreslení stromu
					glm::mat4 m_m_tree = glm::identity<glm::mat4>();
					m_m_tree = glm::scale(m_m_tree, glm::vec3(4.4f));
					m_m_tree = glm::translate(m_m_tree, glm::vec3(15.0f, -2.0f, 5.0f));
					m_m_tree = glm::rotate(m_m_tree, glm::radians(100.0f * (float)glfwGetTime()), glm::vec3(0.0f, 0.1f, 0.0f));

					// Vykreslení konvičky
					glm::mat4 m_m_teapot = glm::identity<glm::mat4>();
					m_m_teapot = glm::scale(m_m_teapot, glm::vec3(1.0f));
					m_m_teapot = glm::translate(m_m_teapot, glm::vec3(50.0f, -9.0f, 20.0f));
					m_m_teapot = glm::rotate(m_m_teapot, glm::radians(100.0f * (float)glfwGetTime()), glm::vec3(0.0f, 0.1f, 0.0f));


					// Vykreslení mapy
					glm::mat4 m_m_map = glm::identity<glm::mat4>();
					m_m_map = glm::scale(m_m_map, glm::vec3(1.0f));
					m_m_map = glm::translate(m_m_map, glm::vec3(0.0f, -10.0f, 0.0f));


					// Poloprůhledná láhev
					glm::mat4 m_m_bottle = glm::identity<glm::mat4>();
					m_m_bottle = glm::scale(m_m_bottle, glm::vec3(75.0f));
					m_m_bottle = glm::translate(m_m_bottle, glm::vec3(1.0f, -0.1f, 0.7f));

					// Detekce kolizí

					glm::vec3 point = camera->Position;
					if (m3.detectCollision(camera->Position, m_m_bottle) || m.detectCollision(camera->Position, m_m_tree) || m2.detectCollision(camera->Position, m_m_teapot)) {
						camera->Position = camera_backup;
						glUniform3fv(glGetUniformLocation(prog, "camPos"), 1, &camera->Position[0]);

					}

					// Vykreslení

					glm::mat4 v_m = camera->GetViewMatrix();

					m.draw(m_m_tree, v_m, projectionMatrix);
					m2.draw(m_m_teapot, v_m, projectionMatrix);
					height_map.draw(m_m_map, v_m, projectionMatrix);

					// Vykreslení průhledných objektů
					glEnable(GL_BLEND); // Zapnutí blendingu

					m3.draw(m_m_bottle, v_m, projectionMatrix);

					glDisable(GL_BLEND);
					// Konec vykreslování průhledných objektů
				}

				/* Swap front and back buffers */
				glfwSwapBuffers(window);

			}

			// Výpočet Delta_T času a kontrola FaceDetection
			auto end = clock::now();
			if (!globals.useFaceProtection || (globals.useFaceProtection && globals.faceDetected) ) {

				auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
				float fps = 1000000.0f / elapsed.count();
				delta_t = 0.005f * fps;
			}
			else if (globals.useFaceProtection && !globals.faceDetected) {
				delta_t = 0;
			}


			glfwPollEvents();
		}
		glfwTerminate();
		exit(EXIT_SUCCESS);
	}
	catch (cv::Exception& e)
	{
		std::cerr << e.msg << std::endl; // output exception message
	}

	std::cerr << "KONEC" << std::endl;
}