/*
例题 4.2.2-1 main.cpp
实现三角形网络的直接Lapacian平滑

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

#include <GL/freeglut.h>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <iostream>
#include <vector>


using namespace std;
typedef OpenMesh::TriMesh_ArrayKernelT<> MyMesh;

std::vector<MyMesh::Point> cogs;
std::vector<MyMesh::Point>::iterator cog_it;

MyMesh mesh;
MyMesh::VertexIter v_it, v_end(mesh.vertices_end());
MyMesh::VertexVertexIter vv_it;
MyMesh::Point cog;
MyMesh::Scalar valence;
float xRotate = 0.0f;
float yRotate = 0.0f;
float zRotate = 0.0f;
float scale = 1.2;
GLuint showFaceList, showWireList, showPointList;

bool showFace = true;
bool showWire = false;
bool showPoint = false;
float threshold = 0.01;
void initGL() {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    showFaceList = glGenLists(1);
    showWireList = glGenLists(1);
    showPointList = glGenLists(1);
    int temp = mesh.n_edges();
    // SHOW WIRE
    glNewList(showWireList, GL_COMPILE);
    glDisable(GL_LIGHTING);
    glLineWidth(1.0f);
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINES);
    for (MyMesh::HalfedgeIter he_it = mesh.halfedges_begin();
         he_it != mesh.halfedges_end(); ++he_it) {
        glVertex3fv(mesh.point(mesh.from_vertex_handle(*he_it)).data());
        glVertex3fv(mesh.point(mesh.to_vertex_handle(*he_it)).data());
    }
    glEnd();
    glEnable(GL_LIGHTING);
    glEndList();
    // SHOW POINT
    glNewList(showPointList, GL_COMPILE);
    glDisable(GL_LIGHTING);
    glLineWidth(1.0f);
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_POINTS);
    for (MyMesh::VertexIter v_it = mesh.vertices_begin();
         v_it != mesh.vertices_end(); ++v_it) {
        glVertex3fv(mesh.point(*v_it).data());
    }
    glEnd();
    glEnable(GL_LIGHTING);
    glEndList();
    // SHOW FACE
    glNewList(showFaceList, GL_COMPILE);
    for (MyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end();
         ++f_it) {
        glBegin(GL_TRIANGLES);
        for (MyMesh::FaceVertexIter fv_it = mesh.fv_iter(*f_it);
             fv_it.is_valid(); ++fv_it) {
            glNormal3fv(mesh.normal(*fv_it).data());
            glVertex3fv(mesh.point(*fv_it).data());
        }
        glEnd();
    }
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
//
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
//

float smooth() {
    float err = -1;
    // 清空上一轮计算的质心列表
    cogs.clear();
    v_end = mesh.vertices_end();
    // 对每个顶点：计算其邻接顶点的平均位置（质心），并保存到 cogs
    for (v_it = mesh.vertices_begin(); v_it != v_end; ++v_it) {
        cog = MyMesh::Point(0.0f, 0.0f, 0.0f);
        valence = 0.0f;
        // 累加邻接顶点的位置
        for (vv_it = mesh.vv_iter(*v_it); vv_it.is_valid(); ++vv_it) {
            cog += mesh.point(*vv_it);
            ++valence;
        }
        // 取平均（若无邻接则保持原位）
        if (valence > 0.0f)
            cog /= valence;
        else
            cog = mesh.point(*v_it);
        // 把计算得到的质心保存在列表中，后续统一更新位置
        cogs.push_back(cog);
    }
    for (v_it = mesh.vertices_begin(), cog_it = cogs.begin(); v_it != v_end;
         ++v_it, ++cog_it)
        // 对非边界顶点，用邻接质心替换位置，同时记录最大位移（用于收敛判断）
        if (!mesh.is_boundary(*v_it)) {
            MyMesh::Point delta = mesh.point(*v_it) - *cog_it;
            float d = delta.norm();
            if (d > err)
                err = d;
            // 更新顶点位置到邻接平均位置（Laplacian 平滑的直接实现）
            mesh.set_point(*v_it, *cog_it);
        }
    return err;
}
void smooth(float threshold) {
    float err;
    do {
        err = smooth();
        cout << "err:" << err << endl;
    } while (err >= threshold);
}
void myKeyboard(unsigned char key, int x, int y) {
    switch (key) {
    case '1':
        showFace = !showFace;
        break;
    case '2':
        showPoint = !showPoint;
        break;
    case '3':
        showWire = !showWire;
        break;
    case 's':
        cout << "smooth the mesh" << endl;
        // smooth();
        smooth(threshold);
        initGL();
        cout << "done smoothing" << endl;
        break;
    default:
        break;
    }
    glutPostRedisplay();
}
//
void myDisplay() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glRotatef(xRotate, 1.0f, 0.0f, 0.0f);
    glRotatef(yRotate, 0.0f, 1.0f, 0.0f);
    glRotatef(zRotate, 0.0f, 0.0f, 1.0f);
    glScalef(scale, scale, scale);

    if (showPoint)
        glCallList(showPointList);
    if (showWire)
        glCallList(showWireList);
    if (showFace)
        glCallList(showFaceList);
    glutSwapBuffers();
}

int main(int argc, char** argv) {
    mesh.request_vertex_normals();
    if (!mesh.has_vertex_normals()) {
        std::cerr
            << "ERROR: Standard vertex property 'Normals' not available!\n";
        return 1;
    }
    OpenMesh::IO::Options opt;
    const char* filename = "twelve.obj";
    if (!OpenMesh::IO::read_mesh(mesh, filename, opt)) {
        cerr << "Error: Cannot read mesh from " << filename << endl;
        return 1;
    }
    if (!opt.check(OpenMesh::IO::Options::VertexNormal)) {
        mesh.request_face_normals();
        mesh.update_normals();
        mesh.release_face_normals();
    }
    cogs.reserve(mesh.n_vertices());

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 500);
    glutCreateWindow("例题 4.2.2-1");
    initGL();
    glutKeyboardFunc(&myKeyboard);
    glutSpecialFunc(&mySpecial);
    glutReshapeFunc(&myReshape);
    glutDisplayFunc(&myDisplay);
    glutMainLoop();
    return 0;
}