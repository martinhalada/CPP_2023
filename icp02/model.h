#include <vector>
// OpenGL Extension Wrangler
#include <GL/glew.h> 
#include <GL/wglew.h> //WGLEW = Windows GL Extension Wrangler (change for different platform) 
// GLFW toolkit
#include <GLFW/glfw3.h>
// OpenGL math
#include <glm/glm.hpp>
#include "shaders.h"
#include "OBJloader.h"
#include <vector>
#include <string>

struct Vertex {
    glm::vec3 position;
    glm::vec2 texCoords;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Model
{
public:
    std::vector<my_vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;
    std::vector<glm::vec4> colors;
    GLuint VAO;
    GLuint prog;
    GLuint ID;
    std::string texture = "";
    cv::Mat hmap;

    bool transparent = false;
    glm::vec3 boundingBoxVertices[8];
    Model(const char* path, GLuint prog, std::string type);
    void draw(const glm::mat4& M, const glm::mat4& V, const glm::mat4& P);
    void loadTexture(std::string path);
    void setTexture(std::string path);
    float getHeightAt(float x, float y, float ratio);
    void createBoundingBox();
    bool detectCollision(glm::vec3 point, glm::mat4 m_m);
};

glm::vec2 get_subtex_st(const int x, const int y);
glm::vec2 get_subtex_by_height(float height);