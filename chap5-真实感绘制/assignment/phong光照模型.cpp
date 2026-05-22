/*
例题5.2.2-2 main.cpp
实现了一个使用Phong着色的球体

作者：周弈帆
最后编辑：2019.8.6

开发工具：
Microsoft Visual Studio 2017

OpenGL库版本：
GLFW-3.2.1
GLAD-0.1.29

开源库版本：
OpenGL Mathematics(glm)-0.9.9.5
(https://github.com/g-truc/glm/releases/tag/0.9.9.5)
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

const unsigned int SCR_WIDTH = 1366;
const unsigned int SCR_HEIGHT = 768;

void resizeGL(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// 输入球半径r,经纬度上需要的顶点数row,column
// 输出所有的顶点数 verticeCount
// 输出顶点数组
float* getBallVertice(int& verticeCount, float r = 1, GLuint row = 30,
                      GLuint column = 30);

glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;

glm::vec3 viewPos = glm::vec3(0, 0, 3);
glm::vec3 viewFront = glm::vec3(0, 0, -1);
glm::vec3 viewUp = glm::vec3(0.0f, 1.0f, 0.0f);

int main() {
    // 初始化OpenGL

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window =
        glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "例题5.2.2-2", NULL, NULL);
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
    // 着色器使用Phong光照模型对球体着色，实现细节和伪代码类似
    writeShader(vertexShader, fragmentShader);
    Shader shader(vertexShader.c_str(), fragmentShader.c_str());

    // 通过函数生成一个球的顶点信息并将其设为绘制对象

    float* ballVertice;
    int verticeCount;
    ballVertice = getBallVertice(verticeCount);

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verticeCount * 6, ballVertice,
                 GL_STATIC_DRAW);
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        // 处理输入信息并初始化缓冲、变换矩阵
        processInput(window);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        model = glm::mat4(1.0f);
        view = glm::lookAt(viewPos, viewPos + viewFront, viewUp);
        projection = glm::perspective(
            glm::radians(90.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

        // 设置着色器
        shader.use();

        // 设置观察位置与变换矩阵
        shader.setVec3("viewPos", viewPos);
        shader.setMatrix4("model", model);
        shader.setMatrix4("view", view);
        shader.setMatrix4("projection", projection);
        // 设置材质信息
        shader.setVec3("material.ambient", 1.0f, 0.0f, 0.0f);
        shader.setVec3("material.diffuse", 1.0f, 0.0f, 0.0f);
        shader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        shader.setFloat("material.shininess", 32.0f);
        // 设置光线信息
        shader.setVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);
        shader.setVec3("dirLight.diffuse", 0.6f, 0.6f, 0.6f);
        shader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
        shader.setVec3("dirLight.direction", -0.5f, -1.0f, -1.0f);

        // 绘制球
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, verticeCount);

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

float* getBallVertice(int& verticeCount, float r, GLuint row, GLuint column) {
    std::vector<std::vector<glm::vec3>> pointPos(row);
    for (int i = 0; i < row; i++) {
        pointPos[i] = std::vector<glm::vec3>(column);
        for (int j = 0; j < column; j++) {
            float phi = float(i + 1) / (row + 1) * glm::pi<float>();
            float theta = float(j) / (column) * 2 * glm::pi<float>();
            float x = r * sin(phi) * cos(theta);
            float z = r * sin(phi) * sin(theta);
            float y = r * cos(phi);
            pointPos[i][j] = {x, y, z};
        }
    }

    float* result = new float[column * 3 * 6 * 2 + (row - 1) * column * 6 * 6];

    GLuint index = 0;

    auto pushPoint = [&](const glm::vec3& p) {
        result[index++] = p.x;
        result[index++] = p.y;
        result[index++] = p.z;
        result[index++] = p.x;
        result[index++] = p.y;
        result[index++] = p.z;
    };

    glm::vec3 top(0, 0, r), bottom(0, 0, -r);

    for (int j = 0; j < column; j++) {
        pushPoint(top);
        pushPoint(pointPos[0][j]);
        pushPoint(pointPos[0][(j + 1) % column]);
    }
    for (int j = 0; j < column; j++) {
        pushPoint(bottom);
        pushPoint(pointPos[row - 1][(j + 1) % column]);
        pushPoint(pointPos[row - 1][j]);
    }
    for (int i = 0; i < row - 1; i++) {
        for (int j = 0; j < column; j++) {
            pushPoint(pointPos[i][j]);
            pushPoint(pointPos[i + 1][j]);
            pushPoint(pointPos[i][(j + 1) % column]);

            pushPoint(pointPos[i + 1][j]);
            pushPoint(pointPos[i][(j + 1) % column]);
            pushPoint(pointPos[i + 1][(j + 1) % column]);
        }
    }
    verticeCount = index / 6;
    return result;
}

void writeShader(std::string& vertexShader, std::string& fragmentShader) {
    vertexShader = std::string("\
#version 330 core\n\
layout (location = 0) in vec3 aPos;\n\
layout (location = 1) in vec3 aNormal;\n\
\n\
out vec3 vertexPos;\n\
out vec3 normal;\n\
\n\
uniform mat4 model;\n\
uniform mat4 view;\n\
uniform mat4 projection;\n\
\n\
void main()\n\
{\n\
	vertexPos = vec3(model * vec4(aPos.x, aPos.y, aPos.z, 1.0));\n\
    gl_Position = projection * view * vec4(vertexPos, 1.0);\n\
	normal = mat3(transpose(inverse(model))) * aNormal;\n\
}\n\
\n\
");

    fragmentShader = std::string("\
#version 330 core\n\
\n\
struct Material\n\
{\n\
	vec3 ambient;\n\
	vec3 diffuse;\n\
	vec3 specular;\n\
	float shininess;\n\
};\n\
\n\
struct DirLight\n\
{\n\
	vec3 direction;\n\
	vec3 ambient;\n\
	vec3 diffuse;\n\
	vec3 specular;\n\
};\n\
\n\
uniform Material material;\n\
uniform DirLight dirLight;\n\
\n\
vec3 calDirLight(DirLight light, vec3 norm, vec3 viewDir)\n\
{\n\
	vec3 ambient = light.ambient * material.ambient;\n\
	\n\
	vec3 lightDir = normalize(-light.direction);\n\
	float diff = max(dot(norm, lightDir), 0);\n\
	vec3 diffuse = diff * light.diffuse * material.diffuse;\n\
\n\
	vec3 reflectDir = reflect(-lightDir, norm);\n\
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);\n\
	vec3 specular = light.specular * spec * material.specular;\n\
\n\
	return (ambient + diffuse + specular);\n\
}\n\
\n\
\n\
\n\
in vec3 vertexPos;\n\
in vec3 normal;\n\
uniform vec3 viewPos;\n\
\n\
out vec4 FragColor;\n\
\n\
\n\
void main()\n\
{\n\
	vec3 viewDir = normalize(viewPos - vertexPos);\n\
	vec3 norm = normalize(normal);\n\
	FragColor = vec4(calDirLight(dirLight, norm, viewDir), 1.0);\n\
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