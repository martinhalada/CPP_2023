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

#include "callbacks.h"
#include "camera_opencv.h"
#include "opengl.h"
#include <fstream>
#include <sstream>
#include "shaders.h"


std::string getShaderInfoLog(const GLuint obj) {
	int infologLength = 0;
	std::string s;
	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
	if (infologLength > 0) {
		std::vector<char> v(infologLength);
		glGetShaderInfoLog(obj, infologLength, NULL,
			v.data());
		s.assign(begin(v), end(v));
	}
	return s;
}
std::string getProgramInfoLog(const GLuint obj) {
	int infologLength = 0;
	std::string s;
	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
	if (infologLength > 0) {
		std::vector<char> v(infologLength);
		glGetProgramInfoLog(obj, infologLength, NULL,
			v.data());
		s.assign(begin(v), end(v));
	}
	return s;
}

std::string textFileRead(const std::string fn) {
	std::ifstream file;
	std::stringstream ss;
	file.open(fn);

	if (file.is_open()) {
		std::string content;
		ss << file.rdbuf();
	}
	else {
		std::cerr << "Error opening file: " << fn <<
			std::endl;
		exit(EXIT_FAILURE);
	}

	return std::move(ss.str());
}

GLuint generate_my_vertex_VAO(std::vector<my_vertex> vertices, std::vector<GLuint> indices) {
	GLuint VAO;

	{
		GLuint VBO, EBO;

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		// Bind VAO (set as the current)
		glBindVertexArray(VAO);
		// Bind the VBO, set type as GL_ARRAY_BUFFER
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// Fill-in data into the VBO
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(my_vertex),
			vertices.data(), GL_STATIC_DRAW);
		// Bind EBO, set type GL_ELEMENT_ARRAY_BUFFER
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		// Fill-in data into the EBO
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
			indices.data(), GL_STATIC_DRAW);
		// Set Vertex Attribute to explain OpenGL how to interpret the VBO
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(my_vertex),
			(void*)(0 + offsetof(my_vertex, position)));
		// Enable the Vertex Attribute 0 = position
		glEnableVertexAttribArray(0);
		// Set end enable Vertex Attribute 1 = Color
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(my_vertex),
			(void*)(0 + offsetof(my_vertex, color)));
		glEnableVertexAttribArray(1);
		// Set end enable Vertex Attribute 2 = Texture Coordinates
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(my_vertex),
			(void*)(0 + offsetof(my_vertex, texture)));
		glEnableVertexAttribArray(2);
		// Set end enable Vertex Attribute 2 = Normal
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(my_vertex),
			(void*)(0 + offsetof(my_vertex, normal)));
		glEnableVertexAttribArray(3);
		// Bind VBO and VAO to 0 to prevent unintended modification
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	return VAO;
}

GLuint loadShaders(std::string path) {
	GLuint VS_h = glCreateShader(GL_VERTEX_SHADER);
	GLuint FS_h = glCreateShader(GL_FRAGMENT_SHADER);
	std::string VSsrc = textFileRead(path + ".vert");
	const char* VS_string = VSsrc.c_str();
	std::string FSsrc = textFileRead(path + ".frag");
	const char* FS_string = FSsrc.c_str();
	glShaderSource(VS_h, 1, &VS_string, NULL);
	glShaderSource(FS_h, 1, &FS_string, NULL);
	glCompileShader(VS_h);
	getShaderInfoLog(VS_h);
	glCompileShader(FS_h);
	getShaderInfoLog(FS_h);
	GLuint prog_h;
	prog_h = glCreateProgram();
	glAttachShader(prog_h, VS_h);
	glAttachShader(prog_h, FS_h);
	glLinkProgram(prog_h);
	getProgramInfoLog(prog_h);
	return prog_h;
}