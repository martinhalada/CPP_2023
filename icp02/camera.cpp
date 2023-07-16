#include "camera.h"
#include <iostream>


glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
}

glm::vec3 Camera::ProcessInput(Camera::Direction direction, GLfloat deltaTime)
{
    GLfloat velocity = this->MovementSpeed * deltaTime;

    switch (direction)
    {
    case Direction::UP:
        return this->Position += this->Front * velocity;
    case Direction::DOWN:
        return this->Position -= this->Front * velocity;
    case Direction::LEFT:
        return this->Position -= glm::normalize(glm::cross(this->Front, this->Up)) * velocity;
    case Direction::RIGHT:
        return this->Position += glm::normalize(glm::cross(this->Front, this->Up)) * velocity;
    }

    return glm::vec3(0.0f);
}

void Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constraintPitch = GL_TRUE)
{
    xoffset *= this->MouseSensitivity;
    yoffset *= this->MouseSensitivity;

    this->Yaw += xoffset;
    this->Pitch -= yoffset;  //+= ... inverted pitch axe

    if (constraintPitch)
    {
        if (this->Pitch > 89.0f)
            this->Pitch = 89.0f;
        if (this->Pitch < -89.0f)
            this->Pitch = -89.0f;
    }

    this->updateCameraVectors();
}


void Camera::updateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
    front.y = sin(glm::radians(this->Pitch));
    front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));

    this->Front = glm::normalize(front);
    this->Right = glm::normalize(glm::cross(this->Front, glm::vec3(0.0f, 1.0f, 0.0f)));
    this->Up = glm::normalize(glm::cross(this->Right, this->Front));
}

