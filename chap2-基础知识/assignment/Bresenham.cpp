/*
例题 2.3.1-2 main.cpp
实现了一个用Bresenham算法在网格上画的线

作者：王加贺
最后编辑：2019.8.8

开发工具：
Microoft Visual Studio 2019

OpenGL库版本：
GLFW-3.2.1

*/

#include <iostream>
#include <vector>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
using namespace std;

void DrawPixel(int x, int y, vector<int>& indexVec) {
    indexVec.push_back(x);
    indexVec.push_back(y);
}

class DrawLineBase {
  public:
    DrawLineBase();
    ~DrawLineBase();
    virtual vector<int>& DrawLine(int x0, int y0, int x1, int y1) = 0;
};

DrawLineBase::DrawLineBase() {}

DrawLineBase::~DrawLineBase() {}

class BresenhamDrawLine : public DrawLineBase {
  public:
    void swap_int(int* a, int* b);

    vector<int>& DrawLine(int x0, int y0, int x1, int y1);
};

void BresenhamDrawLine::swap_int(int* a, int* b) {
    if (a != b) {
        *a ^= *b;
        *b ^= *a;
        *a ^= *b;
    }
}

vector<int>& BresenhamDrawLine::DrawLine(int x1, int y1, int x2, int y2) {
    vector<int>* pIndexVec = new vector<int>();

    ////请补充代码
}

class BuilderBase {
  public:
    BuilderBase();
    virtual ~BuilderBase();
    vector<GLfloat>& GetAllFloats();
    const vector<GLfloat>& GetThisFloats() const;
    virtual void Draw() = 0;
    void DrawAll();

  protected:
    vector<BuilderBase*>* pBuilderBaseVec;
    vector<GLfloat>* pFloatVec;
};

BuilderBase::BuilderBase() {
    pFloatVec = new vector<GLfloat>();
    pBuilderBaseVec = new vector<BuilderBase*>();
}

BuilderBase::~BuilderBase() {
    if (pFloatVec != nullptr) {
        delete pFloatVec;
        pFloatVec = nullptr;
    }
    if (pBuilderBaseVec != nullptr) {
        delete pBuilderBaseVec;
        pBuilderBaseVec = nullptr;
    }
}

vector<GLfloat>& BuilderBase::GetAllFloats() {
    vector<GLfloat>* pAllFloats = new vector<GLfloat>();
    if (pBuilderBaseVec->size() != 0) {
        for (auto it = pBuilderBaseVec->begin(); it != pBuilderBaseVec->end();
             ++it) {
            vector<GLfloat>& thisFloats = (*it)->GetAllFloats();
            pAllFloats->insert(pAllFloats->end(), thisFloats.begin(),
                               thisFloats.end());
            delete &thisFloats;
        }
    } else {
        pAllFloats->insert(pAllFloats->end(), pFloatVec->begin(),
                           pFloatVec->end());
    }
    return *pAllFloats;
}

const vector<GLfloat>& BuilderBase::GetThisFloats() const { return *pFloatVec; }

void BuilderBase::DrawAll() {
    for (auto it = pBuilderBaseVec->begin(); it != pBuilderBaseVec->end();
         ++it) {
        (*it)->DrawAll();
    }
    Draw();
}

class GridBuilder : public BuilderBase {
  public:
    GridBuilder(int width, int height);
    virtual ~GridBuilder();
    virtual void Draw();

  private:
    inline void GetLine(bool isRow, int index);

  private:
    int width;
    int height;
};

GridBuilder::GridBuilder(int width, int height)
    : width(width), height(height) {}

GridBuilder::~GridBuilder() {}

void GridBuilder::GetLine(bool isRow, int index) {
    static GLfloat exp = 0.005f;
    GLfloat dw = 2.0f / width;
    GLfloat dh = 2.0f / height;
    if (isRow) {
        GLfloat wIndex = dh * index - 1.0f;
        pFloatVec->push_back(-1.0f);
        pFloatVec->push_back(wIndex);
        pFloatVec->push_back(0.0f);

        pFloatVec->push_back(1.0f);
        pFloatVec->push_back(wIndex + exp);
        pFloatVec->push_back(0.0f);

        pFloatVec->push_back(-1.0f);
        pFloatVec->push_back(wIndex + exp);
        pFloatVec->push_back(0.0f);

        pFloatVec->push_back(-1.0f);
        pFloatVec->push_back(wIndex);
        pFloatVec->push_back(0.0f);

        pFloatVec->push_back(1.0f);
        pFloatVec->push_back(wIndex + exp);
        pFloatVec->push_back(0.0f);

        pFloatVec->push_back(1.0f);
        pFloatVec->push_back(wIndex);
        pFloatVec->push_back(0.0f);
    } else {
        GLfloat hIndex = dh * index - 1.0f;
        pFloatVec->push_back(hIndex);
        pFloatVec->push_back(-1.0f);
        pFloatVec->push_back(0.0f);

        pFloatVec->push_back(hIndex + exp);
        pFloatVec->push_back(1.0f);
        pFloatVec->push_back(0.0f);

        pFloatVec->push_back(hIndex);
        pFloatVec->push_back(1.0f);
        pFloatVec->push_back(0.0f);

        pFloatVec->push_back(hIndex);
        pFloatVec->push_back(-1.0f);
        pFloatVec->push_back(0.0f);

        pFloatVec->push_back(hIndex + exp);
        pFloatVec->push_back(1.0f);
        pFloatVec->push_back(0.0f);

        pFloatVec->push_back(hIndex + exp);
        pFloatVec->push_back(-1.0f);
        pFloatVec->push_back(0.0f);
    }
}

void GridBuilder::Draw() {
    for (int i = 0; i <= width; i++) {
        GetLine(true, i);
    }
    for (int i = 0; i <= height; i++) {
        GetLine(false, i);
    }
}

class LineBuilder : public BuilderBase {
  public:
    LineBuilder(DrawLineBase* pDrawLine, int startX, int startY, int endX,
                int endY, int width, int height);
    virtual ~LineBuilder();
    virtual void Draw();

  private:
    void GetPoint(int x, int y);

  private:
    DrawLineBase* pDrawLine;
    int startX, startY, endX, endY;
    int width, height;
};

LineBuilder::LineBuilder(DrawLineBase* pDrawLine, int startX, int startY,
                         int endX, int endY, int width, int height)
    : pDrawLine(pDrawLine), startX(startX), startY(startY), endX(endX),
      endY(endY), width(width), height(height) {}

LineBuilder::~LineBuilder() {
    if (pDrawLine != nullptr) {
        delete pDrawLine;
        pDrawLine = nullptr;
    }
}

void LineBuilder::GetPoint(int xIndex, int yIndex) {
    static GLfloat dw = 2.0f / width, dh = 2.0f / height;
    GLfloat x = dw * xIndex, y = dh * yIndex;
    pFloatVec->push_back(x);
    pFloatVec->push_back(y);
    pFloatVec->push_back(0.0f);
    pFloatVec->push_back(x + dw);
    pFloatVec->push_back(y + dh);
    pFloatVec->push_back(0.0f);
    pFloatVec->push_back(x);
    pFloatVec->push_back(y + dh);
    pFloatVec->push_back(0.0f);
    pFloatVec->push_back(x);
    pFloatVec->push_back(y);
    pFloatVec->push_back(0.0f);
    pFloatVec->push_back(x + dw);
    pFloatVec->push_back(y + dh);
    pFloatVec->push_back(0.0f);
    pFloatVec->push_back(x + dw);
    pFloatVec->push_back(y);
    pFloatVec->push_back(0.0f);
}

void LineBuilder::Draw() {
    vector<int>& indexVec = pDrawLine->DrawLine(startX, startY, endX, endY);
    for (int i = 0; i < indexVec.size(); i += 2) {
        GetPoint(indexVec[i], indexVec[i + 1]);
    }
    delete &indexVec;
}

class PictureBuilder : public BuilderBase {
  public:
    PictureBuilder(DrawLineBase* pDrawLine, int width, int height, int startX,
                   int startY, int endX, int endY);
    virtual ~PictureBuilder();
    virtual void Draw();
};

PictureBuilder::PictureBuilder(DrawLineBase* pDrawLine, int width, int height,
                               int startX, int startY, int endX, int endY) {
    pBuilderBaseVec->push_back(new GridBuilder(width, height));
    pBuilderBaseVec->push_back(
        new LineBuilder(pDrawLine, startX, startY, endX, endY, width, height));
}

PictureBuilder::~PictureBuilder() {}

void PictureBuilder::Draw() {}

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mode);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// Shaders
const GLchar* vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 position;\n"
    "void main()\n"
    "{\n"
    "gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
    "}\0";
const GLchar* fragmentShaderSource = "#version 330 core\n"
                                     "out vec4 color;\n"
                                     "void main()\n"
                                     "{\n"
                                     "color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                     "}\n\0";

// The MAIN function, from here we start the application and run the game loop
int main() {
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window =
        glfwCreateWindow(WIDTH, HEIGHT, u8"例题 2.3.1-2", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);

    // Set this to true so GLEW knows to use a modern approach to retrieving
    // function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    glewInit();

    // Define the viewport dimensions
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Build and compile our shader program
    // Vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // Check for compile time errors
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // Fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // Check for compile time errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // Link shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Set up vertex data (and buffer(s)) and attribute pointers
    // Decide the number of vertices
    const int VERTICES_NUM = 5000;

    // Default vars
    int GRID_WIDTH = 100;
    int GRID_HEIGHT = GRID_WIDTH;
    int startX = 0, startY = -40, endX = -30, endY = 45;
    DrawLineBase* pDrawLine = new BresenhamDrawLine();

    // Input vars
    cout << "请输入网格宽度：" << endl;
    cin >> GRID_WIDTH;
    GRID_HEIGHT = GRID_WIDTH;
    cout << "请输入起点x坐标(范围：-" << GRID_WIDTH / 2 << ", "
         << GRID_WIDTH / 2 << ")：" << endl;
    cin >> startX;
    startX--;
    cout << "请输入起点y坐标(范围：-" << GRID_HEIGHT / 2 << ", "
         << GRID_HEIGHT / 2 << ")：" << endl;
    cin >> startY;
    startY--;
    cout << "请输入终点x坐标(范围：-" << GRID_WIDTH / 2 << ", "
         << GRID_WIDTH / 2 << ")：" << endl;
    cin >> endX;
    endX--;
    cout << "请输入终点y坐标(范围：-" << GRID_HEIGHT / 2 << ", "
         << GRID_HEIGHT / 2 << ")：" << endl;
    cin >> endY;
    endY--;
    // int which;
    // cout << "采用DDA还是Bresenham？(0/1)" << endl;
    // cin >> which;
    // if (which == 1)
    pDrawLine = new BresenhamDrawLine();
    // else
    //	pDrawLine = new DDADrawLine();

    // New objects to draw a picture
    PictureBuilder* pictureBuilder = new PictureBuilder(
        pDrawLine, GRID_WIDTH, GRID_HEIGHT, startX, startY, endX, endY);
    pictureBuilder->DrawAll();
    vector<GLfloat>& allFloats = pictureBuilder->GetAllFloats();

    // Generate veretices array
    float vertices[3 * VERTICES_NUM];
    int vertice_index_count =
        static_cast<int>(allFloats.size()) < 3 * VERTICES_NUM
            ? static_cast<int>(allFloats.size())
            : 3 * VERTICES_NUM;

    for (int i = 0; i < vertice_index_count; i++) {
        vertices[i] = allFloats[i];
    }

    // Release memories
    delete &allFloats;
    if (pictureBuilder != nullptr) {
        delete pictureBuilder;
        pictureBuilder = nullptr;
    }

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s)
    // and attribute pointer(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                          (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(
        GL_ARRAY_BUFFER,
        0); // Note that this is allowed, the call to glVertexAttribPointer
            // registered VBO as the currently bound vertex buffer object so
            // afterwards we can safely unbind

    glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any
                          // buffer/array to prevent strange bugs)

    // Game loop
    while (!glfwWindowShouldClose(window)) {
        // Check if any events have been activiated (key pressed, mouse moved
        // etc.) and call corresponding response functions
        glfwPollEvents();

        // Render
        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertice_index_count);
        glBindVertexArray(0);

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}
