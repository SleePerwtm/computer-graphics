/*************************************************************
@Author: 韩世杰
@Date:2019-07-10
@Description:例题3.2.2-1 平面三次Bezier曲线
根据题目所给的四个控制顶点绘制对应的Bezier曲线
方法一：递归实现了de Casteljau算法，对每一个t都能计算出最后的p(t)
方法二：手工计算出了最后的表达式（见同一目录下的递归实现Word）
使用<glut.h>绘制出两种方法对应的图像，完全重合，验证了递归算法的正确性
**************************************************************/

#define GL_SILENCE_DEPRECATION // 预定义防止报错

// MAC Xcode环境
// #include <GLUT/GLUT.h>

// Windows10  VC2017环境
#include <GL/glut.h>

// 通用声明
#include <cmath>
#include <cstdio>
#include <iostream>
using namespace std;
#define MAX 0x3f3f3f3f
#define RGB 255
#define PI 3.1415926535897

struct point {
    float x, y;
}; // 定义一个点的数据结构体
int num(int a, int b); // 计算排列组合数的函数，对应c_(a,b)

// 定义存储数据的类
class Bezier {
  private:
    point control[4]; // 题目给出的四个控制顶点
    int size = 3;     // 控制顶点数为4
  public:
    void control_point_input();
    void print_equation();
    friend void draw();
    point p_cal(int r, int i, float t);
    // de Casteljau算法递归计算p_r,i(t)
} problem;
void draw();

int main(int argc, char* argv[]) {
    problem.control_point_input();
    problem.print_equation();
    glutInit(&argc, argv);                // 完成必要初始化并开启事件处理循环
    glutInitDisplayMode(GLUT_RGBA);       // 指定颜色显示模式
    glutInitWindowPosition(150, 100);     // 指定窗口位置
    glutInitWindowSize(600, 600);         // 指定窗口大小
    glutCreateWindow("例题3.2.2-1 结果"); // 指定窗口名称
    glutDisplayFunc(&draw);               // 调用draw绘图函数
    glutMainLoop();                       // 无限调用事件循环
    // 主函数中glutMainLoop()后的代码将不会执行
    return 0;
}

// 补充 de Casteljau的递归函数计算实现
point Bezier::p_cal(int r, int i, float t) {
    if (r == 0) {
        return control[i];
    } else {
        point temp;
        temp.x = (1 - t) * problem.p_cal(r - 1, i, t).x +
                t * problem.p_cal(r - 1, i + 1, t).x;
        temp.y = (1 - t) * problem.p_cal(r - 1, i, t).y +
                t * problem.p_cal(r - 1, i + 1, t).y;
        return temp;
    }
}

// 绘图函数
void draw() {
    // 准备工作
    GLfloat t;
    glClearColor(255 / RGB, 255 / RGB, 255 / RGB, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    gluOrtho2D(-3, 3, -3, 3);
    glBegin(GL_LINES);
    glColor3f(0 / RGB, 0 / RGB, 0 / RGB);
    glVertex2f(-100.0f, 0.0f);
    glVertex2f(100.0f, 0.0f);
    glVertex2f(0.0f, -100.0f);
    glVertex2f(0.0f, 100.0f);
    glEnd();

    // 将四个控制顶点依次相连
    glBegin(GL_LINES);
    glVertex2f(-2, 0);
    glVertex2f(-1, 1);
    glVertex2f(-1, 1);
    glVertex2f(1, 1.5);
    glVertex2f(1, 1.5);
    glVertex2f(2, 0);
    glEnd();

    // 四个控制顶点描点
    int n = 1000;
    glBegin(GL_POLYGON);
    for (int m = 0; m < n; m++) {
        glVertex2f(0.03 * cos(2 * PI * m / n) - 2, 0.03 * sin(2 * PI * m / n));
    }
    glEnd();
    glBegin(GL_POLYGON);
    for (int m = 0; m < n; m++) {
        glVertex2f(0.03 * cos(2 * PI * m / n) - 1,
                   0.03 * sin(2 * PI * m / n) + 1);
    }
    glEnd();
    glBegin(GL_POLYGON);
    for (int m = 0; m < n; m++) {
        glVertex2f(0.03 * cos(2 * PI * m / n) + 1,
                   0.03 * sin(2 * PI * m / n) + 1.5);
    }
    glEnd();
    glBegin(GL_POLYGON);
    for (int m = 0; m < n; m++) {
        glVertex2f(0.03 * cos(2 * PI * m / n) + 2, 0.03 * sin(2 * PI * m / n));
    }
    glEnd();

    // 作出两种方法得到的曲线
    point temp;
    glBegin(GL_POINTS);
    for (t = 0; t <= 1; t += 0.0001) {
        temp = problem.p_cal(3, 3, t);
        glVertex2f(temp.x, temp.y);
        // 递归得到的点数据绘制的图线1
        glVertex2f((-2 * pow(1 - t, 3) - 3 * pow(1 - t, 2) * t +
                    3 * pow(t, 2) * (1 - t) + 2 * pow(t, 3)),
                   (3 * pow(1 - t, 2) * t + 4.5 * (1 - t) * pow(t, 2)));
        // 手算出的精准表达式绘制的图线2
    }
    glEnd();
    glFlush();
}

// 计算排列组合数c_(a,b)
int num(int a, int b) {
    if (b == 0 || a - b == 0) {
        return 1;
    }
    int top = 1, bottom_m = 1, bottom_n = 1; // 分子和分母
    for (int i = a; i > 0; i--)
        top *= i;
    for (int i = b; i > 0; i--)
        bottom_m *= i;
    for (int i = a - b; i > 0; i--)
        bottom_n *= i;
    return top / (bottom_m * bottom_n);
}

// 控制顶点数据初始化
void Bezier::control_point_input() {
    control[0].x = -2;
    control[0].y = 0;
    control[1].x = -1;
    control[1].y = 1;
    control[2].x = 1;
    control[2].y = 1.5;
    control[3].x = 2;
    control[3].y = 0;
}

// 打印最终得到的曲线表达式
void Bezier::print_equation() {
    cout << "控制顶点定义的三次Bezier曲线为:\n";
    cout << "p(t)=" << "(";
    for (int i = 0; i <= size; i++) {
        float temp = control[i].x * num(size, i); // temp是系数
        if (temp == 0) {
        } else if (!i)
            cout << temp;
        else if (i && !control[i - 1].x) {
            cout << temp;
        } else
            cout << "  +  " << temp;
        if (temp && size - i)
            cout << "(1-t)";
        if (temp && size - i - 1 > 0)
            cout << "^" << size - i;
        if (temp && i)
            cout << "*t";
        if (temp && i - 1 > 0)
            cout << "^" << i;
    } // x
    cout << "  ,  ";
    for (int i = 0; i <= size; i++) {
        float temp = control[i].y * num(size, i); // temp是系数
        if (temp == 0) {
        } else if (!i)
            cout << temp;
        else if (i && !control[i - 1].y) {
            cout << temp;
        } else
            cout << "  +  " << temp;
        if (temp && size - i)
            cout << "(1-t)";
        if (temp && size - i - 1 > 0)
            cout << '^' << size - i;
        if (temp && i)
            cout << "*t";
        if (temp && i - 1 > 0)
            cout << "^" << i;
    } // y
    cout << ")" << endl;
}