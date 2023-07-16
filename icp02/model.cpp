#include <vector>
#include <opencv2/opencv.hpp>
// OpenGL Extension Wrangler
#include <GL/glew.h> 
#include <GL/wglew.h> //WGLEW = Windows GL Extension Wrangler (change for different platform) 
// GLFW toolkit
#include <GLFW/glfw3.h>
// OpenGL math
#include <glm/glm.hpp>
#include "shaders.h"
#include "OBJloader.h"
#include "model.h"

Model::Model(const char* path, GLuint prog, std::string type)
{
    if (type == "fromFile") {
        GLuint ID;
        std::vector < glm::vec3 > out_vertices;
        std::vector < glm::vec2 > out_uvs;
        std::vector < glm::vec3 > out_normals;

        loadOBJ(path, out_vertices, out_uvs, out_normals);

        for (int i = 0; i < out_vertices.size(); i++) {
            my_vertex v = { out_vertices.at(i), {0.0f, 0.0f, 1.0f}, out_uvs.at(i), out_normals.at(i) };
            this->vertices.insert(this->vertices.begin(), v);
            this->indices.insert(this->indices.begin(), i);
        }
        this->prog = prog;
        this->VAO = generate_my_vertex_VAO(this->vertices, this->indices);
        std::cout << this->vertices.size() << std::endl;
    }
    else {
        cv::Mat hmap = cv::imread(path, cv::IMREAD_GRAYSCALE);
        this->hmap = cv::imread(path, cv::IMREAD_GRAYSCALE);

        if (hmap.empty())
        {
            std::cerr << "ERR: Height map empty? File:" << path << std::endl;
        }


        std::cout << hmap.size() << std::endl;
        unsigned int mesh_step_size = 1;

        unsigned int num = 0;
        //row == height
        //col == width
        for (unsigned int x_coord = 0; x_coord < (hmap.cols - mesh_step_size); x_coord += mesh_step_size)
        {
            for (unsigned int z_coord = 0; z_coord < (hmap.rows - mesh_step_size); z_coord += mesh_step_size)
            {
                // Get The (X, Y, Z) Value For The Bottom Left Vertex = 0
                glm::vec3 p0(x_coord, hmap.at<uchar>(cv::Point(x_coord, z_coord)), z_coord);
                // Get The (X, Y, Z) Value For The Bottom Right Vertex = 1
                glm::vec3 p1(x_coord + mesh_step_size, hmap.at<uchar>(cv::Point(x_coord + mesh_step_size, z_coord)), z_coord);
                // Get The (X, Y, Z) Value For The Top Right Vertex = 2
                glm::vec3 p2(x_coord + mesh_step_size, hmap.at<uchar>(cv::Point(x_coord + mesh_step_size, z_coord + mesh_step_size)), z_coord + mesh_step_size);
                // Get The (X, Y, Z) Value For The Top Left Vertex = 3
                glm::vec3 p3(x_coord, hmap.at<uchar>(cv::Point(x_coord, z_coord + mesh_step_size)), z_coord + mesh_step_size);

                // Get max normalized height for tile, set texture accordingly
                // Grayscale image returns 0..256, normalize to 0.0f..1.0f by dividing by 256
                float max_h = std::max(hmap.at<uchar>(cv::Point(x_coord, z_coord)) / 256.0f,
                    std::max(hmap.at<uchar>(cv::Point(x_coord, z_coord + mesh_step_size)) / 256.0f,
                        std::max(hmap.at<uchar>(cv::Point(x_coord + mesh_step_size, z_coord + mesh_step_size)) / 256.0f,
                            hmap.at<uchar>(cv::Point(x_coord + mesh_step_size, z_coord)) / 256.0f
                        )));

                // Get texture coords in vertices, bottom left of geometry == bottom left of texture
                glm::vec2 tc0 = get_subtex_by_height(max_h);
                glm::vec2 tc1 = tc0 + glm::vec2(1.0f / 16, 0.0f);			//add offset for bottom right corner
                glm::vec2 tc2 = tc0 + glm::vec2(1.0f / 16, 1.0f / 16);  //add offset for top right corner
                glm::vec2 tc3 = tc0 + glm::vec2(0.0f, 1.0f / 16);       //add offset for bottom leftcorner

                //place vertices and ST to mesh
                my_vertex v;
                v.position = p0;
                v.texture = tc0;

                this->vertices.emplace_back(v);
                v; v.position = p1; v.texture = tc1;
                this->vertices.emplace_back(v);
                v; v.position = p2; v.texture = tc2;
                this->vertices.emplace_back(v);
                v; v.position = p3; v.texture = tc3;
                this->vertices.emplace_back(v);


                // place indices
                this->indices.emplace_back(num++);
                this->indices.emplace_back(num++);
                this->indices.emplace_back(num++);
                this->indices.emplace_back(num - 3);
                this->indices.emplace_back(num - 1);
                this->indices.emplace_back(num++);

            }
        }

        this->prog = prog;
        this->VAO = generate_my_vertex_VAO(this->vertices, this->indices);
        std::cout << "Terrain: " << this->vertices.size() << std::endl;

    }
    this->createBoundingBox();
}

float Model::getHeightAt(float x, float z, float ratio) {

    float height = 0;

    // Interpolace
    if (x < 0 || x > hmap.cols || z < 0 || z > hmap.rows) {
        return 0;
    }
    if (x > 0 && z > 0 && x < hmap.cols && z < hmap.rows) {
        float x1 = floor(x);
        float x2 = ceil(x);

        float z1 = floor(z);
        float z2 = ceil(z);

        int p1 = this->hmap.at<uchar>(cv::Point(x2, z2));
        int p2 = this->hmap.at<uchar>(cv::Point(x1, z2));
        int p3 = this->hmap.at<uchar>(cv::Point(x2, z1));
        int p4 = this->hmap.at<uchar>(cv::Point(x1, z1));

        float top = (float)p1 * (x - x1) + p2 * (1 - (x - x1));
        float bottom = (float)p3 * (x - x1) + p4 * (1 - (x - x1));

        float interpolation = top * (z - z1) + bottom * (1 - (z - z1));

        height = interpolation;

    }
    else {
        height = (float)this->hmap.at<uchar>(cv::Point(x, z));
    }

    height -= 5;
    return height;
}

//return bottom left ST of subtexture
glm::vec2 get_subtex_st(const int x, const int y)
{
    return glm::vec2(x * 1.0f / 16, y * 1.0f / 16);
}

glm::vec2 get_subtex_by_height(float height)
{
    if (height > 0.9)
        return get_subtex_st(2, 4); //snow
    else if (height > 0.8)
        return get_subtex_st(3, 4); //ice
    else if (height > 0.5)
        return get_subtex_st(1, 0); //rock
    else if (height > 0.3)
        return get_subtex_st(7, 0); //soil
    else
        return get_subtex_st(0, 0); //grass
}


void Model::setTexture(std::string path) {
    this->texture = path;
    this->loadTexture(this->texture);
}

void Model::loadTexture(std::string path) {
    this->texture = path;
    cv::Mat image = cv::imread(path, cv::IMREAD_UNCHANGED); // Read with (potential) Alpha
    //if (image.channels() != 4) exit(-1);  // Check the image, we want Alpha in this example    

    // Generates an OpenGL texture object
    glGenTextures(1, &this->ID);

    // Assigns the texture to a Texture Unit
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->ID);

    // Texture data alignment for transfer (single byte = basic, slow, but safe option; usually not necessary) 
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Assigns the image to the OpenGL Texture object
    if (image.channels() == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, image.data);
    }
    else {
        this->transparent = true;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.cols, image.rows, 0, GL_BGRA, GL_UNSIGNED_BYTE, image.data);
    }

    // Configures the type of algorithm that is used to make the image smaller or bigger
    // nearest neighbor - ugly & fast 
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // bilinear - nicer & slower
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // MIPMAP filtering + automatic MIPMAP generation - nicest, needs more memory. Notice: MIPMAP is only for image minifying.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // bilinear magnifying
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // trilinear minifying
    glGenerateMipmap(GL_TEXTURE_2D);  //Generate mipmaps now.

    // Configures the way the texture repeats
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Unbinds the OpenGL Texture object so that it can't accidentally be modified
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Model::createBoundingBox() {

    glm::vec3 minVertex = this->vertices[0].position;
    glm::vec3 maxVertex = this->vertices[0].position;

    // Iterate through all vertices to find the bounding box
    for (int i = 1; i < vertices.size(); i++) {
        if (this->vertices[i].position.x < minVertex.x) {
            minVertex.x = this->vertices[i].position.x;
        }
        if (this->vertices[i].position.y < minVertex.y) {
            minVertex.y = this->vertices[i].position.y;
        }
        if (this->vertices[i].position.z < minVertex.z) {
            minVertex.z = this->vertices[i].position.z;
        }
        if (this->vertices[i].position.x > maxVertex.x) {
            maxVertex.x = this->vertices[i].position.x;
        }
        if (this->vertices[i].position.y > maxVertex.y) {
            maxVertex.y = this->vertices[i].position.y;
        }
        if (this->vertices[i].position.z > maxVertex.z) {
            maxVertex.z = this->vertices[i].position.z;
        }
    }

    // Define the bounding box vertices

    this->boundingBoxVertices[0] = glm::vec3(minVertex.x, minVertex.y, minVertex.z); // back bottom left
    this->boundingBoxVertices[1] = glm::vec3(maxVertex.x, minVertex.y, minVertex.z); // back bottom right
    this->boundingBoxVertices[2] = glm::vec3(maxVertex.x, maxVertex.y, minVertex.z); // back top right
    this->boundingBoxVertices[3] = glm::vec3(minVertex.x, maxVertex.y, minVertex.z); // back top left
    this->boundingBoxVertices[4] = glm::vec3(minVertex.x, minVertex.y, maxVertex.z); // front bottom left
    this->boundingBoxVertices[5] = glm::vec3(maxVertex.x, minVertex.y, maxVertex.z); // front bottom right
    this->boundingBoxVertices[6] = glm::vec3(maxVertex.x, maxVertex.y, maxVertex.z); // front top right
    this->boundingBoxVertices[7] = glm::vec3(minVertex.x, maxVertex.y, maxVertex.z); // front top left
}

bool Model::detectCollision(glm::vec3 point, glm::mat4 m_m) {

    glm::vec3 boundingBoxVertices[8];
    memcpy(boundingBoxVertices, this->boundingBoxVertices, sizeof(this->boundingBoxVertices));

    for (int i = 0; i < 8; i++) {
        boundingBoxVertices[i] = m_m * glm::vec4(boundingBoxVertices[i], 1.0f);
    }

    glm::vec3 minVertex = boundingBoxVertices[0];
    glm::vec3 maxVertex = boundingBoxVertices[0];

    for (int i = 1; i < 8; i++) {
        if (boundingBoxVertices[i].x < minVertex.x) {
            minVertex.x = boundingBoxVertices[i].x;
        }
        if (boundingBoxVertices[i].y < minVertex.y) {
            minVertex.y = boundingBoxVertices[i].y;
        }
        if (boundingBoxVertices[i].z < minVertex.z) {
            minVertex.z = boundingBoxVertices[i].z;
        }
        if (boundingBoxVertices[i].x > maxVertex.x) {
            maxVertex.x = boundingBoxVertices[i].x;
        }
        if (boundingBoxVertices[i].y > maxVertex.y) {
            maxVertex.y = boundingBoxVertices[i].y;
        }
        if (boundingBoxVertices[i].z > maxVertex.z) {
            maxVertex.z = boundingBoxVertices[i].z;
        }
    }

    // Check if the point is inside the bounding box
    if (point.x >= minVertex.x && point.x <= maxVertex.x &&
        point.y >= minVertex.y && point.y <= maxVertex.y &&
        point.z >= minVertex.z && point.z <= maxVertex.z) {
        return true;
    }
    else {
        return false;
    }

}

void Model::draw(const glm::mat4& M, const glm::mat4& V, const glm::mat4& P) {
    glUseProgram(this->prog);
    glUniformMatrix4fv(glGetUniformLocation(this->prog, "uP_m"), 1, GL_FALSE, glm::value_ptr(P));
    glUniformMatrix4fv(glGetUniformLocation(this->prog, "uV_m"), 1, GL_FALSE, glm::value_ptr(V));
    glUniformMatrix4fv(glGetUniformLocation(this->prog, "uM_m"), 1, GL_FALSE, glm::value_ptr(M));


    if (this->texture != "") {

        glActiveTexture(GL_TEXTURE0);

        //send texture unit number to FS
        glUniform1i(glGetUniformLocation(this->prog, "tex0"), 0);

        glBindTexture(GL_TEXTURE_2D, this->ID);
    }

    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);

}