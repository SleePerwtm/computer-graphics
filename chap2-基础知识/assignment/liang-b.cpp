// clang-format off
#include <glad/glad.h> // 必须放到最前面，不然会出现报错
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

#include "shader.h"

// clang-format on

using namespace std;

struct Point {
    float x;
    float y;
};

// 裁剪窗口
float xleft = -60.0f, xright = 70.0f, ytop = 60.0f, ybottom = -40.0f;

// 裁剪后的线段
Point P0, P1;

// 坐标系范围
int range = 100;
const unsigned int SCR_WIDTH = 2 * range;
const unsigned int SCR_HEIGHT = 2 * range;

bool clip(float p, float q, float& t0, float& t1) {
    if (fabs(p) < 1e-6) {
        if (q < 0.0f)
            return false;
        else
            return true;
    }
    float u = q / p;
    if (p < 0.0f) {
        if (u > t1)
            return false;
        if (u > t0)
            t0 = u;
    } else { // p > 0
        if (u < t0)
            return false;
        if (u < t1)
            t1 = u;
    }
    return true;
}

bool LiangBarskyClipLine(Point p1, Point p2) {
    struct PQ {
        float p, q;
    } pq_arr[4];
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    // clang-format off
    pq_arr[0].p = -dx, pq_arr[0].q = p1.x - xleft;   // left
    pq_arr[1].p =  dx, pq_arr[1].q = xright - p1.x;   // right
    pq_arr[2].p = -dy, pq_arr[2].q = p1.y - ybottom; // bottom
    pq_arr[3].p =  dy, pq_arr[3].q = ytop - p1.y;     // top
    // clang-format on
    float t0 = 0.0f, t1 = 1.0f;
    for (int i = 0; i < 4; ++i) {
        if (!clip(pq_arr[i].p, pq_arr[i].q, t0, t1))
            return false;
    }

    P0.x = p1.x + t0 * dx;
    P0.y = p1.y + t0 * dy;
    P1.x = p1.x + t1 * dx;
    P1.y = p1.y + t1 * dy;
    return true;
}

void processInput(GLFWwindow* window);
int main() {
    cout << "请输入裁剪窗口的顶部边界 (范围: [" << -range << ", " << range
         << "]):\n";
    cin >> ytop;
    cout << "请输入裁剪窗口的底部边界 (范围: [" << -range << ", " << range
         << "]):\n";
    cin >> ybottom;
    cout << "请输入裁剪窗口的左侧边界 (范围: [" << -range << ", " << range
         << "]):\n";
    cin >> xleft;
    cout << "请输入裁剪窗口的右侧边界 (范围: [" << -range << ", " << range
         << "]):\n";
    cin >> xright;

    // 输入的线段
    Point p1, p2;
    /*p1.x = -80;
    p1.y = 85;
    p2.x = 80;
    p2.y = -5;*/
    cout << "请输入线段起点的 x 坐标 (范围: [-" << SCR_WIDTH / 2 << ", "
         << SCR_WIDTH / 2 << "]):\n";
    cin >> p1.x;
    cout << "请输入线段起点的 y 坐标 (范围: [-" << SCR_HEIGHT / 2 << ", "
         << SCR_HEIGHT / 2 << "]):\n";
    cin >> p1.y;
    cout << "请输入线段终点的 x 坐标 (范围: [-" << SCR_WIDTH / 2 << ", "
         << SCR_WIDTH / 2 << "]):\n";
    cin >> p2.x;
    cout << "请输入线段终点的 y 坐标 (范围: [-" << SCR_HEIGHT / 2 << ", "
         << SCR_HEIGHT / 2 << "]):\n";
    cin >> p2.y;

    if (LiangBarskyClipLine(p1, p2)) {
        cout << "\n裁剪后的线段为(" << P0.x << ", " << P0.y << "), (" << P1.x
             << ", " << P1.y << ")\n";
    } else {
        cout << "\n拒绝裁剪\n";
        return 0;
    }

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    int scale = 3;
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH * scale, SCR_HEIGHT * scale,
                                          "2-5", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    vector<glm::vec3> clip_window(4, glm::vec3(0.0f));
    clip_window[0] = glm::vec3(xleft, ytop, 0.0f);
    clip_window[1] = glm::vec3(xleft, ybottom, 0.0f);
    clip_window[2] = glm::vec3(xright, ybottom, 0.0f);
    clip_window[3] = glm::vec3(xright, ytop, 0.0f);

    unsigned int VAO_window, VBO_window;
    glGenVertexArrays(1, &VAO_window);
    glGenBuffers(1, &VBO_window);
    glBindVertexArray(VAO_window);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_window);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec3), &clip_window[0],
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                          (void*)0);
    glEnableVertexAttribArray(0);

    vector<glm::vec3> line(2, glm::vec3(0.0f));
    line[0] = glm::vec3(p1.x, p1.y, -0.01f);
    line[1] = glm::vec3(p2.x, p2.y, -0.01f);

    unsigned int VAO_line, VBO_line;
    glGenVertexArrays(1, &VAO_line);
    glGenBuffers(1, &VBO_line);
    glBindVertexArray(VAO_line);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
    glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec3), &line[0],
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                          (void*)0);
    glEnableVertexAttribArray(0);

    vector<glm::vec3> clip(2, glm::vec3(0.0f));
    clip[0] = glm::vec3(P0.x, P0.y, -0.02f);
    clip[1] = glm::vec3(P1.x, P1.y, -0.02f);

    unsigned int VAO_clip, VBO_clip;
    glGenVertexArrays(1, &VAO_clip);
    glGenBuffers(1, &VBO_clip);
    glBindVertexArray(VAO_clip);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_clip);
    glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec3), &clip[0],
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                          (void*)0);
    glEnableVertexAttribArray(0);

    Shader shader_window("liang-b-others/v.glsl", "liang-b-others/f_window.glsl");
    Shader shader_line("liang-b-others/v.glsl", "liang-b-others/f_line.glsl");
    Shader shader_clip("liang-b-others/v.glsl", "liang-b-others/f_clip.glsl");

    glLineWidth(3.0f);
    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader_window.use();
        shader_window.setFloat("range", range);
        glBindVertexArray(VAO_window);
        glDrawArrays(GL_LINE_LOOP, 0, 4);

        shader_line.use();
        shader_line.setFloat("range", range);
        glBindVertexArray(VAO_line);
        glDrawArrays(GL_LINES, 0, 2);

        shader_clip.use();
        shader_clip.setFloat("range", range);
        glBindVertexArray(VAO_clip);
        glDrawArrays(GL_LINES, 0, 2);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}