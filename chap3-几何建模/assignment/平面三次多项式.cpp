/*************************************************************
@Author: 韩世杰
@Date:2019-07-04
@Description:例题3.2.1-1 平面三次多项式插值曲线
利用题目中给出的四个型值点确定其对应的平面三次多项式以及四个控制顶点
主要函数poly_cal()主要实现了解四元一次方程组并输出的功能
最后用<glut.h>画出最终解（四组控制顶点坐标）对应的曲线
在二维作图使用glBegin(GL_POLYGON)绘制任意位置的定点的两种选择:
1.在glVertex2f(x+a,y+b)表达式中加减相关坐标a,b
2.使用坐标存取栈glTranslatef(a,b,0)暂时改变原点的位置
**************************************************************/

#define GL_SILENCE_DEPRECATION // 预定义防止报错

// MAC Xcode环境
// #include <GLUT/GLUT.h>

// Windows10  VC2017环境
#include <glut.h>

// 通用声明
#include <cmath>
#include <cstdio>
#include <iostream>
using namespace std;
#define RGB 255.0
#define PI 3.1415926535
#define MAX 0x3f3f3f3f

// 定义存储数据的类
class poly_solve {
  public:
    float a[4], b[4], t[4];   // 输入的型值点组格式为(ai,bi,t)
    float matrix[4][5];       // 后续计算系数矩阵
    float c0x, c1x, c2x, c3x; // 四个控制顶点的x值
    float c0y, c1y, c2y, c3y; // 四个控制顶点的y值
    float store[5][5];        // 临时存储的备用矩阵
    poly_solve();             // 利用构造函数初始化系数矩阵
    void point_initial();     // 型值点数据输入
    void poly_cal();          // 求解方程组
    void res_output();        // 在console中打印得到的控制顶点
    friend void draw(void);   // 声明友元使得draw()可以调用最后的计算结果
} example;
void draw(void);

int main(int argc, char* argv[]) {
    example.point_initial();
    example.poly_cal();
    example.res_output();
    glutInit(&argc, argv);                // 完成必要初始化并开启事件处理循环
    glutInitDisplayMode(GLUT_RGBA);       // 指定颜色显示模式
    glutInitWindowPosition(150, 100);     // 指定窗口位置
    glutInitWindowSize(600, 600);         // 指定窗口大小
    glutCreateWindow("例题3.2.1-1 结果"); // 指定窗口名称
    glutDisplayFunc(&draw);               // 调用draw绘图函数
    glutMainLoop();                       // 无限调用事件循环
    // 主函数中glutMainLoop()后的代码将不会执行
    return 0;
}

// 画图函数
void draw(void) {
    // 准备工作
    GLfloat p_x, p_y, t;
    glLoadIdentity();
    glTranslatef(0, 0, 0);
    glClearColor(255 / RGB, 255 / RGB, 255 / RGB, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    gluOrtho2D(-5, 5, -5, 5);
    glBegin(GL_LINES);
    glColor3f(0 / RGB, 0 / RGB, 0 / RGB);
    glVertex2f(-10.0f, 0.0f);
    glVertex2f(10.0f, 0.0f);
    glVertex2f(0.0f, -10.0f);
    glVertex2f(0.0f, 10.0f);
    glEnd();
    int n = 1000;

    // 用方法2--位移存栈函数依次描出四个控制点，第一个(0,0)不需要位移
    glBegin(GL_POLYGON);
    for (int m = 0; m < n; m++) {
        glVertex2f(0.1 * cos(2 * PI * m / n), 0.1 * sin(2 * PI * m / n));
    }
    glEnd();
    n = 1000;
    glPushMatrix();
    glTranslatef(2, 2, 0); //(2,2)
    glBegin(GL_POLYGON);
    for (int m = 0; m < n; m++) {
        glVertex2f(0.1 * cos(2 * PI * m / n), 0.1 * sin(2 * PI * m / n));
    }
    glEnd();
    glPopMatrix();
    n = 1000;
    glPushMatrix();
    glTranslatef(0, 3, 0); //(0,3)
    glBegin(GL_POLYGON);
    for (int m = 0; m < n; m++) {
        glVertex2f(0.1 * cos(2 * PI * m / n), 0.1 * sin(2 * PI * m / n));
    }
    glEnd();
    glPopMatrix();
    n = 1000;
    glPushMatrix();
    glTranslatef(2, 4, 0); //(2,4)
    glBegin(GL_POLYGON);
    for (int m = 0; m < n; m++) {
        glVertex2f(0.1 * cos(2 * PI * m / n), 0.1 * sin(2 * PI * m / n));
    }
    glEnd();
    glPopMatrix();

    // 调用example中的计算结果,绘制标准图像
    glBegin(GL_POINTS);
    glColor3f(50 / RGB, 0 / RGB, 0 / RGB);
    for (t = 0; t < 1; t += 0.00001) {
        p_x = example.c0x + example.c1x * t + example.c2x * t * t +
              example.c3x * t * t * t;
        p_y = example.c0y + example.c1y * t + example.c2y * t * t +
              example.c3y * t * t * t;
        glVertex2d(p_x, p_y);
    }
    glEnd();
    glFlush();
}

// 型值点数据初始化
poly_solve::poly_solve() {
    a[0] = 0;
    b[0] = 0;
    t[0] = 0;
    a[1] = 2;
    b[1] = 2;
    t[1] = 0.25;
    a[2] = 0;
    b[2] = 3;
    t[2] = 0.75;
    a[3] = 2;
    b[3] = 4;
    t[3] = 1;
    // 仅针对本次例题数据，也可以在point_initial里面用cin写输入
}

// 初始化系数矩阵
void poly_solve::point_initial() {
    int n = 3;
    while (n >= 0) {
        for (int i = 0; i < 4; i++)
            matrix[n][i] = pow(t[n], i); // 初始化系数矩阵
        n--;
    }
}

// 采用直接消元法求解线性方程组
void poly_solve::poly_cal() {
    // 备份系数项矩阵
    for (int i = 0; i < 4; i++) {
        for (int m = 0; m < 4; m++) {
            store[i][m] = matrix[i][m];
        }
    }

    // 右侧实数项矩阵赋值
    for (int i = 0; i < 4; i++) {
        matrix[i][4] = a[i];
    }

    // x,y各求解一次四元一次方程组
    // 求解x
    for (int i = 1; i < 4; i++) {
        float weight = matrix[i][0] / matrix[0][0];
        for (int m = 0; m <= 4; m++)
            matrix[i][m] = matrix[i][m] - matrix[0][m] * weight;
    } // 第二行消元
    for (int i = 2; i < 4; i++) {
        float weight = matrix[i][1] / matrix[1][1];
        for (int m = 1; m <= 4; m++)
            matrix[i][m] = matrix[i][m] - matrix[1][m] * weight;
    } // 第三行消元
    for (int i = 3; i < 4; i++) {
        float weight = matrix[i][2] / matrix[2][2];
        for (int m = 2; m <= 4; m++)
            matrix[i][m] = matrix[i][m] - matrix[2][m] * weight;
    } // 第四行消元
    if (!(matrix[0][0] && matrix[1][1] && matrix[2][2] && matrix[3][3])) {
        cout << "" << endl;
        return;
    }
    c3x = matrix[3][4] / matrix[3][3];
    c2x = (matrix[2][4] - matrix[2][3] * c3x) / matrix[2][2];
    c1x =
        (matrix[1][4] - matrix[1][3] * c3x - matrix[1][2] * c2x) / matrix[1][1];
    c0x = (matrix[0][4] - matrix[0][3] * c3x - matrix[0][2] * c2x -
           matrix[0][1] * c1x) /
          matrix[0][0];

    // 用备份矩阵还原matrix
    for (int i = 0; i < 4; i++) {
        matrix[i][4] = b[i];
        for (int m = 0; m < 4; m++) {
            matrix[i][m] = store[i][m];
        }
    }
    // 求解y
    for (int i = 1; i < 4; i++) {
        float weight = matrix[i][0] / matrix[0][0];
        for (int m = 0; m <= 4; m++)
            matrix[i][m] = matrix[i][m] - matrix[0][m] * weight;
    } // 第二行消元
    for (int i = 2; i < 4; i++) {
        float weight = matrix[i][1] / matrix[1][1];
        for (int m = 1; m <= 4; m++)
            matrix[i][m] = matrix[i][m] - matrix[1][m] * weight;
    } // 第三行消元
    for (int i = 3; i < 4; i++) {
        float weight = matrix[i][2] / matrix[2][2];
        for (int m = 2; m <= 4; m++)
            matrix[i][m] = matrix[i][m] - matrix[2][m] * weight;
    } // 第四行消元
    if (!(matrix[0][0] && matrix[1][1] && matrix[2][2] && matrix[3][3])) {
        cout << "" << endl;
        return;
    }
    c3y = matrix[3][4] / matrix[3][3];
    c2y = (matrix[2][4] - matrix[2][3] * c3y) / matrix[2][2];
    c1y =
        (matrix[1][4] - matrix[1][3] * c3y - matrix[1][2] * c2y) / matrix[1][1];
    c0y = (matrix[0][4] - matrix[0][3] * c3y - matrix[0][2] * c2y -
           matrix[0][1] * c1y) /
          matrix[0][0];
}

// 输出四个控制点坐标
void poly_solve::res_output() {
    cout << "(" << c0x << "," << c0y << ")" << endl;
    cout << "(" << c1x << "," << c1y << ")" << endl;
    cout << "(" << c2x << "," << c2y << ")" << endl;
    cout << "(" << c3x << "," << c3y << ")" << endl;
}