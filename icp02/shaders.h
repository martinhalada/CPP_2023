#pragma once
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

struct my_vertex {
	glm::vec3 position; // Vertex
	glm::vec3 color;
	glm::vec2 texture;
	glm::vec3 normal;
};


std::string textFileRead(const std::string fn);
std::string getShaderInfoLog(const GLuint obj);
std::string getProgramInfoLog(const GLuint obj);
GLuint generate_my_vertex_VAO(std::vector<my_vertex> vertices, std::vector<GLuint> indices);
GLuint loadShaders(std::string path);
