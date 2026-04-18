/*
例题 4.3.2-1 main.cpp
实现三角形网络的渐进式网络简化

作者：王加贺
最后编辑：2019.10.15

开放工具：
Microoft Visual Studio 2019

OpenGL库版本：
GLFW-3.2.1
freeglut-3.0.0
OpenMesh-6.3

*/

#define _USE_MATH_DEFINES

#include <GL\freeglut.h>
#include <OpenMesh\Core\IO\MeshIO.hh>
#include <OpenMesh\Core\Mesh\TriMesh_ArrayKernelT.hh>
#include <iostream>
#include <set>
#include <stack>
#include <stdio.h>  /* printf, scanf, puts, NULL */
#include <stdlib.h> /* srand, rand */
#include <time.h>
#include <vector>

using namespace std;
typedef OpenMesh::TriMesh_ArrayKernelT<> OMesh;
OMesh mesh_src;

float xRotate = 0.0f;
float yRotate = 50.0f;
float zRotate = 0.0f;
float scale = 2;
GLuint showWireList;
void plane();
int read_mesh();
struct Point {
    float x, y, z;
    Point(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {};
};
struct Edge {
    int v1, v2;
    Edge(int _v1, int _v2) : v1(_v1), v2(_v2) {};
    bool operator<(Edge other) const {
        if (v1 != other.v1)
            return v1 < other.v1;
        else
            return v2 < other.v2;
    }
};
struct MyMesh {
    int n_edges;
    MyMesh() { n_edges = 0; };
    vector<Point> Points;
    set<Edge> Edges;
    void AddPoints(float x, float y, float z) {
        Points.push_back(Point(x, y, z));
    }
    bool AddEdge(int v1, int v2) {
        bool b1 = Edges.find(Edge(v1, v2)) != Edges.end();
        bool b2 = Edges.find(Edge(v2, v1)) != Edges.end();
        if (b1 || b2)
            return false;
        Edges.insert(Edge(v1, v2));
        n_edges++;
        cout << "add edge(" << v1 << ',' << v2 << ')' << endl;

        return true;
    };
    bool DeleteEdge(int v1, int v2) {
        set<Edge>::iterator it, it1, it2;
        it1 = Edges.find(Edge(v1, v2));
        it2 = Edges.find(Edge(v2, v1));
        bool b1 = it1 != Edges.end();
        bool b2 = it2 != Edges.end();
        if (b1 || b2) {
            it = b1 ? it1 : it2;
            Edges.erase(it);
            n_edges--;
            return true;
        }
        return false;
    }
    bool DeleteEdge(int v1, int v2, set<Edge>::iterator& _it) {
        set<Edge>::iterator it, it1, it2;
        it1 = Edges.find(Edge(v1, v2));
        it2 = Edges.find(Edge(v2, v1));
        bool b1 = it1 != Edges.end();
        bool b2 = it2 != Edges.end();
        if (b1 || b2) {
            it = b1 ? it1 : it2;
            _it = Edges.erase(it);
            n_edges--;
            cout << "delete edge(" << v1 << ',' << v2 << ')' << endl;
            return true;
        }
        return false;
    }
    Edge randomEdge() {
        int rd = rand() % n_edges, i = 0;
        for (set<Edge>::iterator it = Edges.begin(); it != Edges.end();
             it++, i++) {
            if (rd != i)
                continue;
            return *it;
        }
    }
};
MyMesh mesh;
int n_edges;
void initGL() {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    // ------------------- Lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // ------------------- Display List
    showWireList = glGenLists(1);
    // SHOW WIRE
    glNewList(showWireList, GL_COMPILE);
    glDisable(GL_LIGHTING);
    glLineWidth(1.0f);
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINES);
    for (set<Edge>::iterator it = mesh.Edges.begin(); it != mesh.Edges.end();
         it++) {
        int v1, v2;
        v1 = it->v1;
        v2 = it->v2;
        Point &p1 = mesh.Points[v1], &p2 = mesh.Points[v2];
        glVertex3f(p1.x, p1.y, p1.z);
        glVertex3f(p2.x, p2.y, p2.z);
    }
    glEnd();
    glEnable(GL_LIGHTING);
    glEndList();
}

void myReshape(GLint w, GLint h) {
    glViewport(0, 0, static_cast<GLsizei>(w), static_cast<GLsizei>(h));
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w > h)
        glOrtho(-static_cast<GLdouble>(w) / h, static_cast<GLdouble>(w) / h,
                -1.0, 1.0, -1.0, 1.0);
    else
        glOrtho(-1.0, 1.0, -static_cast<GLdouble>(h) / w,
                static_cast<GLdouble>(h) / w, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
void myIdle() {
    xRotate += 0.5f;
    yRotate += 1.0f;
    zRotate += 1.5f;
    if (xRotate >= 360.0f)
        xRotate -= 360.0f;
    if (yRotate >= 360.0f)
        yRotate -= 360.0f;
    if (zRotate >= 360.0f)
        zRotate -= 360.0f;
    glutPostRedisplay();
}
void mySpecial(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP:
        xRotate += 5.0f;
        break;
    case GLUT_KEY_DOWN:
        xRotate -= 5.0f;
        break;
    case GLUT_KEY_LEFT:
        yRotate += 5.0f;
        break;
    case GLUT_KEY_RIGHT:
        yRotate -= 5.0f;
        break;
    case GLUT_KEY_PAGE_UP:
        scale = scale + 0.02;
        break;
    case GLUT_KEY_PAGE_DOWN:
        scale = scale - 0.02;
        break;
    default:
        break;
    }
    glutPostRedisplay();
}
void simplify() {
    Edge rde = mesh.randomEdge();
    int n = mesh.n_edges;
    int v1 = rde.v1, v2 = rde.v2;
    cout << "Collapse " << v1 << " to " << v2 << endl;
    // 补充代码
}
void restart() {
    mesh.Points.clear();
    mesh.Edges.clear();
    mesh.n_edges = 0;
    // plane();
    read_mesh();
}
void myKeyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 's':
        cout << "simplify the mesh" << endl;
        simplify();
        initGL();
        cout << "done simplifying" << endl << endl;
        break;
    case 'r':
        restart();
        initGL();
        break;
    default:
        break;
    }
    glutPostRedisplay();
}
void myDisplay() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glRotatef(xRotate, 1.0f, 0.0f, 0.0f);
    glRotatef(yRotate, 0.0f, 1.0f, 0.0f);
    glRotatef(zRotate, 0.0f, 0.0f, 1.0f);
    glScalef(scale, scale, scale);
    glCallList(showWireList);
    glutSwapBuffers();
}
void plane() {
    mesh.AddPoints(-1, -1, 0);
    mesh.AddPoints(-1, 1, 0);
    mesh.AddPoints(0, -2, 0);
    mesh.AddPoints(0, 0, 0);
    mesh.AddPoints(0, 2, 0);
    mesh.AddPoints(1, -1, 0);
    mesh.AddPoints(1, 1, 0);
    int edges[12][2] = {
        {0, 1}, {0, 2}, {0, 3}, {1, 3}, {1, 4}, {1, 6},
        {2, 3}, {2, 5}, {3, 5}, {3, 6}, {4, 6}, {5, 6},
    };
    for (int i = 0; i < 12; i++)
        mesh.AddEdge(edges[i][0], edges[i][1]);
}

int read_mesh() {
    const char* filename = "horse.off";
    OpenMesh::IO::Options opt;
    if (!OpenMesh::IO::read_mesh(mesh_src, filename, opt)) {
        cerr << "Error: Cannot read mesh from " << filename << endl;
        return 1;
    }
    for (OMesh::VertexIter v_it = mesh_src.vertices_begin();
         v_it != mesh_src.vertices_end(); ++v_it) {
        OMesh::Point p = mesh_src.point(*v_it);
        mesh.AddPoints(p[0], p[1], p[2]);
    }
    cout << mesh_src.n_edges();
    OMesh::EdgeIter eit;
    for (eit = mesh_src.edges_begin(); eit != mesh_src.edges_end(); eit++) {
        auto vh1 = mesh_src.to_vertex_handle(mesh_src.halfedge_handle(*eit, 0));
        auto vh2 =
            mesh_src.from_vertex_handle(mesh_src.halfedge_handle(*eit, 0));
        mesh.AddEdge(vh1.idx(), vh2.idx());
    }
}

int main(int argc, char** argv) {
    // plane();
    read_mesh();
    srand(time(NULL));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 500);
    glutCreateWindow("例题 4.3.2-1");
    initGL();
    glutKeyboardFunc(&myKeyboard);
    glutSpecialFunc(&mySpecial);
    glutReshapeFunc(&myReshape);
    glutDisplayFunc(&myDisplay);
    glutMainLoop();
    return 0;
}
