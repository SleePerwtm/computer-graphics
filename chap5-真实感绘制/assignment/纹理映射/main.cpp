/*
例题5.6.1-1 main.cpp
实现了一个使用纹理映射的立方体

作者：周弈帆
最后编辑：2019.8.6

开发工具：
Microsoft Visual Studio 2017

OpenGL库版本：
GLFW-3.2.1
GLAD-0.1.29

开源库版本：
OpenGL Mathematics(glm)-0.9.9.5
(https://github.com/g-truc/glm/releases/tag/0.9.9.5) stb_image - v2.22
(https://github.com/nothings/stb)
*/
#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// 着色器部分

// 用于读写着色器的类
class Shader {
  public:
    unsigned int getID() { return _ID; }
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
    void use();
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setMatrix4(const std::string& name, const glm::mat4& value) const;

  private:
    unsigned int _ID;
};
// 着色器源代码
std::string vertexShader, fragmentShader;
// 写入着色器源代码
void writeShader(std::string& vertexShader, std::string& fragmentShader);

// 初始化OpenGL所需要的函数及变量
const unsigned int SCR_WIDTH = 1366;
const unsigned int SCR_HEIGHT = 768;
void resizeGL(GLFWwindow* window, int width, int height); // 初始化OpenGL窗口
void processInput(GLFWwindow* window);                    // 处理输入
unsigned int loadTexture(const char* path); // 将纹理读入OpenGL并返回纹理Id

// 变换矩阵
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::mat4(1.0f);

// 观察点位置及观察方向
glm::vec3 viewPos(0.0f, 0.0f, 2.0f);
glm::vec3 viewUp(0.0f, 1.0f, 0.0f);
glm::vec3 viewFront(0.0f, 0.0f, -1.0f);

// 立方体的顶点坐标及纹理坐标信息
// 每个面由6个顶点描述，每个顶点有5个float参数，前3个表示顶点坐标，后2个表示映射到纹理上的坐标
float cube[] = {
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 0.0f,
    0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
    -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

    -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,

    -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  1.0f, 0.0f,

    0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
    0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f,
    0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 1.0f,
    0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

    -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f};

int main() {
    // 初始化OpenGL
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window =
        glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "例题5.6.1-1", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, resizeGL);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 读取着色器
    // 着色器接收顶点的三维坐标、纹理映射二维坐标，将对应的纹理像素输出
    writeShader(vertexShader, fragmentShader);
    Shader shader(vertexShader.c_str(), fragmentShader.c_str());

    // 将立方体的信息加载进GPU，将立方体作为绘制对象
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 读取纹理
    unsigned one, two, three;
    one = loadTexture("yorushika-logo.png");
    two = loadTexture("two.png");
    three = loadTexture("three.png");

    // 设置立方体每一个面的纹理
    unsigned cubeTexture[6] = {one, one, two, two, three, three};

    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        // 处理输入
        processInput(window);

        // 初始化颜色缓存和深度缓存
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 初始化变换矩阵
        view = glm::lookAt(viewPos, viewPos + viewFront, viewUp);
        projection = glm::perspective(
            glm::radians(90.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

        // 调整物体的摆放角度
        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-40.0f),
                            glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(20.0f),
                            glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(-20.0f),
                            glm::vec3(0.0f, 0.0f, 1.0f));

        // 设置变换矩阵
        shader.use();
        shader.setMatrix4("model", model);
        shader.setMatrix4("view", view);
        shader.setMatrix4("projection", projection);

        // 将立方体设置为绘制对象
        glBindVertexArray(VAO);
        shader.setInt("cubeTexture", 0);
        for (int i = 0; i < 6; i++) {
            // 对6个面分别进行绘制，每个面用不同的纹理
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, cubeTexture[i]);
            glDrawArrays(GL_TRIANGLES, 6 * i, 6);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

void resizeGL(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

unsigned int loadTexture(const char* path) {
    unsigned textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void writeShader(std::string& vertexShader, std::string& fragmentShader) {
    vertexShader = std::string("\
#version 330 core\n\
layout (location = 0) in vec3 aPos;\n\
layout (location = 1) in vec2 aTexCoord;\n\
\n\
out vec2 TexCoord;\n\
\n\
uniform mat4 model;\n\
uniform mat4 view;\n\
uniform mat4 projection;\n\
\n\
void main()\n\
{\n\
	TexCoord = aTexCoord;\n\
    gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n\
}\n\
\n\
");

    fragmentShader = std::string("\
#version 330 core\n\
\n\
in vec2 TexCoord;\n\
out vec4 FragColor;\n\
\n\
uniform sampler2D cubeTexture;\n\
\n\
void main()\n\
{\n\
	FragColor = texture(cubeTexture, TexCoord);\n\
}\n\
");
}

//***********************************************************************************************
//*********************************** Shader类代码
//**********************************************
//***********************************************************************************************

Shader::Shader(const GLchar* vShaderCode, const GLchar* fShaderCode) {
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    };

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    };
    _ID = glCreateProgram();
    glAttachShader(_ID, vertex);
    glAttachShader(_ID, fragment);
    glLinkProgram(_ID);
    glGetProgramiv(_ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(_ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() { glUseProgram(_ID); }

void Shader::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(_ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(_ID, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(_ID, name.c_str()), value);
}
void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(_ID, name.c_str()), 1,
                 glm::value_ptr(value));
}
void Shader::setVec3(const std::string& name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(_ID, name.c_str()), x, y, z);
}
void Shader::setMatrix4(const std::string& name, const glm::mat4& value) const {
    glUniformMatrix4fv(glGetUniformLocation(_ID, name.c_str()), 1, GL_FALSE,
                       glm::value_ptr(value));
}

//***********************************************************************************************
//***********************************************************************************************
//***********************************************************************************************