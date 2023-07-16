#pragma once
// OpenGL Extension Wrangler
#include <GL/glew.h> 
#include <GL/wglew.h> //WGLEW = Windows GL Extension Wrangler (change for different platform) 
// GLFW toolkit
#include <GLFW/glfw3.h>
// OpenGL math
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


class Camera {

public:

    enum class Direction {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Right;
    glm::vec3 Up; // camera local UP vector

    GLfloat Yaw = -70.0f;
    GLfloat Pitch = 0.0f;;
    GLfloat Roll = 0.0f;

    // Camera options
    GLfloat MovementSpeed = 1.0f;
    GLfloat MouseSensitivity = 0.25f;

    Camera(glm::vec3 position) :Position(position)
    {
        //this->Position = glm::vec3(0.0f, 1.0f, 0.0f);
        // initialization of the camera reference system
        this->updateCameraVectors();
    }

    glm::mat4 GetViewMatrix();
    glm::vec3 ProcessInput(Camera::Direction direction, GLfloat deltaTime);
    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constraintPitch);

private:

    void updateCameraVectors();

};
