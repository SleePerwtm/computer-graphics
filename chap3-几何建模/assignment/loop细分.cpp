/*************************************************************
@Author: 韩世杰
@Date:2019-08-20
@Description:例题3.3.3-1 Loop细分曲面
建立了三维点、面的数据结构，确定了相关数据的组织形式
在建立好数据结构的基础上产生了便于运算的半边结构存储
做好准备工作后按课本公式更新了封闭立方体情况下内部点和内部边产生的新点
将新点存入新的数组中，刷新初始数组，根据递推时的关系建立新边和新面（思维难点）
用OpenGL 3D函数展示本次运算结果
----------------------------------------------------------------
操作说明：运行程序，会看到输出图形框中已经画好的正四面体
按“1”“2”“3”可以分别以面、线、点的形式观察 图像
按“d”即可看到进行一次细分之后的结果，若无反应，检查一下输入法是否为英文
感谢CSDN的 "fengluoD" 贡献的立方体细分程序样例在思路上和方法上的帮助:
https://blog.csdn.net/fengluod/article/details/83821348
**************************************************************/

#define GL_SILENCE_DEPRECATION
#define _CRT_SECURE_NO_WARNINGS

// MAC Xcode环境
// #include <GLUT/GLUT.h>

// Windows10  VC2017环境
#include <GL/glut.h>

// 共同使用的库
#include <assert.h>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

using namespace std;
#define RGB 255
#define PI 3.1415926536
const int maxn = 200;
// 存储数据数组的大小，这里取100*100，所以只能支持3次细分的数据量

// 顶点结构
typedef struct Vertex {
    float x, y, z;
};

// 面结构
typedef struct Face {
    int num;
    int order[3];
};

// 半边结构
typedef struct HalfEdge {
    int origin;
    struct HalfEdge* next;
    struct HalfEdge* opposite;
    int IncFace;
};

vector<Vertex> vertex;
vector<Face> face;
vector<HalfEdge*> edge;
int edge_num = 0;
int n_node = 0;
int n_face = 0;
int n_edge = 0;

void init_data() {
    // 正四面体细分输入的数据
    // 变更样例时，注意数据的输入要符合自己定义出半边的方向，如123和132是不一样的
    n_node = 4;
    n_edge = 0;
    n_face = 4;
    Vertex v;
    v.x = 0;
    v.y = 0;
    v.z = 0;
    vertex.push_back(v);
    v.x = 1;
    v.y = 0;
    v.z = 0;
    vertex.push_back(v);
    v.x = 0;
    v.y = 1;
    v.z = 0;
    vertex.push_back(v);
    v.x = 0;
    v.y = 0;
    v.z = 1;
    vertex.push_back(v);
    Face f;
    f.num = 3;
    f.order[0] = 0;
    f.order[1] = 1;
    f.order[2] = 2;
    face.push_back(f);
    f.num = 3;
    f.order[0] = 0;
    f.order[1] = 3;
    f.order[2] = 1;
    face.push_back(f);
    f.num = 3;
    f.order[0] = 1;
    f.order[1] = 3;
    f.order[2] = 2;
    face.push_back(f);
    f.num = 3;
    f.order[0] = 2;
    f.order[1] = 3;
    f.order[2] = 0;
    face.push_back(f);

    // 将正方体细分的数据
    /*n_node = 8; n_edge = 0; n_face = 12;
    Vertex v;
    v.x = -0.5; v.y = -0.5; v.z = -0.5;
    vertex.push_back(v);
    v.x = 0.5; v.y = -0.5; v.z = -0.5;
    vertex.push_back(v);
    v.x = -0.5; v.y = 0.5; v.z = -0.5;
    vertex.push_back(v);
    v.x = 0.5; v.y = 0.5; v.z = -0.5;
    vertex.push_back(v);
    v.x = -0.5; v.y = -0.5; v.z = 0.5;
    vertex.push_back(v);
    v.x = 0.5; v.y = -0.5; v.z = 0.5;
    vertex.push_back(v);
    v.x = -0.5; v.y = 0.5; v.z = 0.5;
    vertex.push_back(v);
    v.x = 0.5; v.y = 0.5; v.z = 0.5;
    vertex.push_back(v);

    Face f;
    f.num = 3; f.order[0] = 4; f.order[1] = 5; f.order[2] = 6;
    face.push_back(f);
    f.num = 3; f.order[0] = 6; f.order[1] = 5; f.order[2] = 7;
    face.push_back(f);
    f.num = 3; f.order[0] = 5; f.order[1] = 1; f.order[2] = 7;
    face.push_back(f);
    f.num = 3; f.order[0] = 7; f.order[1] = 1; f.order[2] = 3;
    face.push_back(f);
    f.num = 3; f.order[0] = 0; f.order[1] = 3; f.order[2] = 1;
    face.push_back(f);
    f.num = 3; f.order[0] = 0; f.order[1] = 2; f.order[2] = 3;
    face.push_back(f);
    f.num = 3; f.order[0] = 4; f.order[1] = 6; f.order[2] = 2;
    face.push_back(f);
    f.num = 3; f.order[0] = 4; f.order[1] = 2; f.order[2] = 0;
    face.push_back(f);
    f.num = 3; f.order[0] = 6; f.order[1] = 7; f.order[2] = 2;
    face.push_back(f);
    f.num = 3; f.order[0] = 3; f.order[1] = 2; f.order[2] = 7;
    face.push_back(f);
    f.num = 3; f.order[0] = 5; f.order[1] = 4; f.order[2] = 0;
    face.push_back(f);
    f.num = 3; f.order[0] = 5; f.order[1] = 0; f.order[2] = 1;
    face.push_back(f);*/
}

void initEdge() {
    int map[maxn][maxn] = {0};
    for (int i = 0; i < n_node; i++) {
        for (int j = 0; j < n_node; j++) {
            map[i][j] = -1;
        }
    }
    edge_num = 0;
    for (int i = 0; i < n_face; i++) {
        HalfEdge* edge1 = new HalfEdge();
        HalfEdge* new_edge = new HalfEdge();
        HalfEdge* edge3 = new HalfEdge();

        edge1->origin = face[i].order[0];
        new_edge->origin = face[i].order[1];
        edge3->origin = face[i].order[2];

        edge1->next = new_edge;
        new_edge->next = edge3;
        edge3->next = edge1;

        HalfEdge* tmpe = new HalfEdge();
        if (map[face[i].order[1]][face[i].order[0]] != -1) {
            tmpe = edge[map[face[i].order[1]][face[i].order[0]]];
            edge1->opposite = tmpe;
            tmpe->opposite = edge1;
        } else {
            edge1->opposite = NULL;
            map[face[i].order[0]][face[i].order[1]] = edge_num;
        }
        edge_num++;
        if (map[face[i].order[2]][face[i].order[1]] != -1) {
            tmpe = edge[map[face[i].order[2]][face[i].order[1]]];
            new_edge->opposite = tmpe;
            tmpe->opposite = new_edge;
        } else {
            new_edge->opposite = NULL;
            map[face[i].order[1]][face[i].order[2]] = edge_num;
        }
        edge_num++;
        if (map[face[i].order[0]][face[i].order[2]] != -1) {
            tmpe = edge[map[face[i].order[0]][face[i].order[2]]];
            edge3->opposite = tmpe;
            tmpe->opposite = edge3;
        } else {
            edge3->opposite = NULL;
            map[face[i].order[2]][face[i].order[0]] = edge_num;
        }
        edge_num++;

        edge1->IncFace = i;
        new_edge->IncFace = i;
        edge3->IncFace = i;

        edge.push_back(edge1);
        edge.push_back(new_edge);
        edge.push_back(edge3);
    }
    n_edge = edge.size();
}

// 找到从该定点出发的一条半边
HalfEdge* locate(int v) {
    for (int k = 0; k < n_edge; k++) {
        if (edge[k]->origin == v)
            return edge[k];
    }
    return NULL;
}

void subdivide() {
    vector<Vertex> new_vertex;
    vector<Face> new_face;
    vector<HalfEdge*> new_edge;
    HalfEdge* start_edge = new HalfEdge();
    int n = 0;
    float sumx, sumy, sumz;
    float px, py, pz, cal;
    for (int i = 0; i < n_node; i++) // 旧点更新
    {
        start_edge = locate(i);
        if (start_edge != NULL) {
            n = 0;
            sumx = 0;
            sumy = 0;
            sumz = 0;
            HalfEdge* e = new HalfEdge();
            e = start_edge->next;
            int p0 = e->origin;

            while (e->next->origin != p0) {
                n++;
                sumx += vertex[e->next->origin].x;
                sumy += vertex[e->next->origin].y;
                sumz += vertex[e->next->origin].z;
                HalfEdge* te = new HalfEdge();
                te = e->next->opposite;
                e = te->next;
            }
            n++;
            sumx += vertex[p0].x;
            sumy += vertex[p0].y;
            sumz += vertex[p0].z;
            cal = 1 / (double)n *
                  (0.625 - pow(0.375 + 0.25 * cos(2 * PI / n), 2));
            px = (1 - n * cal) * vertex[i].x + cal * sumx;
            py = (1 - n * cal) * vertex[i].y + cal * sumy;
            pz = (1 - n * cal) * vertex[i].z + cal * sumz;

            Vertex v;
            v.x = px;
            v.y = py;
            v.z = pz;
            new_vertex.push_back(v);
        }
    }

    int map1[maxn][maxn] = {0};
    float qx, qy, qz;

    // 新点生成
    for (int i = 0; i < n_edge; i++) {
        if (!map1[edge[i]->origin][edge[i]->next->origin]) {
            int p1 = edge[i]->origin;
            int p2 = edge[i]->next->origin;
            int p3 = edge[i]->next->next->origin;
            int p4 = edge[i]->opposite->next->next->origin;
            qx = 3 * (vertex[p1].x + vertex[p2].x) / 8 +
                 (vertex[p3].x + vertex[p4].x) / 8;
            qy = 3 * (vertex[p1].y + vertex[p2].y) / 8 +
                 (vertex[p3].y + vertex[p4].y) / 8;
            qz = 3 * (vertex[p1].z + vertex[p2].z) / 8 +
                 (vertex[p3].z + vertex[p4].z) / 8;

            // 补充完整代码
            Vertex v;
            v.x = qx;
            v.y = qy;
            v.z = qz;
            new_vertex.push_back(v);
            int idx = (int)new_vertex.size() - 1; // 新点的索引
            map1[p1][p2] = idx;
            map1[p2][p1] = idx;
        }
    }

    for (int i = 0; i < n_face; i++) // 生成新面
    {
        int a, b, c, d, e, f;
        a = face[i].order[0];
        b = face[i].order[1];
        c = face[i].order[2];
        d = map1[a][b];
        e = map1[b][c];
        f = map1[a][c];

        // 补充完整代码
        int d_idx = d;
        int e_idx = e;
        int f_idx = f;
        Face f1; // (a, d, f)
        f1.num = 3;
        f1.order[0] = a;
        f1.order[1] = d_idx;
        f1.order[2] = f_idx;
        new_face.push_back(f1);

        Face f2; // (d, b, e)
        f2.num = 3;
        f2.order[0] = d_idx;
        f2.order[1] = b;
        f2.order[2] = e_idx;
        new_face.push_back(f2);

        Face f3; // (f, e, c)
        f3.num = 3;
        f3.order[0] = f_idx;
        f3.order[1] = e_idx;
        f3.order[2] = c;
        new_face.push_back(f3);

        Face f4; // (d, e, f)
        f4.num = 3;
        f4.order[0] = d_idx;
        f4.order[1] = e_idx;
        f4.order[2] = f_idx;
        new_face.push_back(f4);
    }

    n_face = new_face.size();
    n_node = new_vertex.size();
    int map2[maxn][maxn] = {0};
    for (int i = 0; i < n_node; i++) {
        for (int j = 0; j < n_node; j++) {
            map2[i][j] = -1;
        }
    }
    edge_num = 0;
    for (int i = 0; i < n_face; i++) // 新边
    {
        HalfEdge* edge4 = new HalfEdge();
        HalfEdge* edge5 = new HalfEdge();
        HalfEdge* edge6 = new HalfEdge();

        edge4->origin = new_face[i].order[0];
        edge5->origin = new_face[i].order[1];
        edge6->origin = new_face[i].order[2];

        edge4->next = edge5;
        edge5->next = edge6;
        edge6->next = edge4;

        HalfEdge* tmpe = new HalfEdge();
        if (map2[new_face[i].order[1]][new_face[i].order[0]] != -1) {
            tmpe = new_edge[map2[new_face[i].order[1]][new_face[i].order[0]]];
            edge4->opposite = tmpe;
            tmpe->opposite = edge4;
        } else {
            edge4->opposite = NULL;
            map2[new_face[i].order[0]][new_face[i].order[1]] = edge_num;
        }
        edge_num++;
        if (map2[new_face[i].order[2]][new_face[i].order[1]] != -1) {
            tmpe = new_edge[map2[new_face[i].order[2]][new_face[i].order[1]]];
            edge5->opposite = tmpe;
            tmpe->opposite = edge5;
        } else {
            edge5->opposite = NULL;
            map2[new_face[i].order[1]][new_face[i].order[2]] = edge_num;
        }
        edge_num++;
        if (map2[new_face[i].order[0]][new_face[i].order[2]] != -1) {
            tmpe = new_edge[map2[new_face[i].order[0]][new_face[i].order[2]]];
            edge6->opposite = tmpe;
            tmpe->opposite = edge6;
        } else {
            edge6->opposite = NULL;
            map2[new_face[i].order[2]][new_face[i].order[0]] = edge_num;
        }
        edge_num++;

        edge4->IncFace = i;
        edge5->IncFace = i;
        edge6->IncFace = i;

        new_edge.push_back(edge4);
        new_edge.push_back(edge5);
        new_edge.push_back(edge6);
    }
    n_edge = new_edge.size();
    vertex.assign(new_vertex.begin(), new_vertex.end());
    face.assign(new_face.begin(), new_face.end());
    edge.assign(new_edge.begin(), new_edge.end());
    cout << n_node << " " << n_edge << " " << n_face << endl;
}

void display() {
    glClearColor(255 / RGB, 255 / RGB, 255 / RGB, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -5.0f);
    gluLookAt(0.01, 0.01, 0.013, 0.0, 0.0, 0.0, 0.0, 1.0,
              0.0); // 设置观察的位置和方向
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINES);
    glVertex3f(100, 0, 0);
    glVertex3f(-100, 0, 0);
    glVertex3f(0, 0, 100);
    glVertex3f(0, 0, -100);
    glVertex3f(0, 100, 0);
    glVertex3f(0, -100, 0);
    glEnd();

    glBegin(GL_TRIANGLES);
    for (int i = 0; i < n_face; i++) {
        glVertex3f(vertex[face[i].order[0]].x, vertex[face[i].order[0]].y,
                   vertex[face[i].order[0]].z);
        glVertex3f(vertex[face[i].order[1]].x, vertex[face[i].order[1]].y,
                   vertex[face[i].order[1]].z);
        glVertex3f(vertex[face[i].order[2]].x, vertex[face[i].order[2]].y,
                   vertex[face[i].order[2]].z);
    }
    glEnd();
    glPopMatrix();
    glutSwapBuffers();
}

// 交互页面函数
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case '1':
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    case '2':
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case '3':
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        break;
    case 'd':
        subdivide();
        break;
    }
    glutPostRedisplay();
}

void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    init_data();
    initEdge();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("例题3.3.3-1结果");
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}
