#include <GLUT/glut.h>
#include <math.h>
#include <vector>

struct Bone {
    float x0, y0, x1, y1;
    struct Bone *parent;
};
std::vector<struct Bone *> bones;


float radius = 0.1;
float delta = 0.001;
float initPos1 = 0.6;
float initPos2 = 0.3;

void makeBones() {
    Bone *a;
    a->x0 = 0.0;
    a->y0 = 0.0;
    a->x1 = 0.3;
    a->y1 = 0.3;
    a->parent = NULL;
    bones.push_back(a);
    Bone *b;
    b->x0 = 0.5;
    b->y0 = 0.5;
    b->x1 = 0.7;
    b->y1 = 0.7;
    a->parent = a;
    bones.push_back(b);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.f,0.f,0.f);

    initPos1 += delta;
    initPos2 += delta;

    // glBegin(GL_LINE_LOOP);
    //     for(int i = 0; i < 100; i++) {
    //         float angle = 2*M_PI*i/100;
    //         glVertex2f(radius*cos(angle)+initPos1, radius*sin(angle)+initPos1);
    //     }
    // glEnd();
    // glBegin(GL_LINE_LOOP);
    //     for(int i = 0; i < 100; i++) {
    //         float angle = 2*M_PI*i/100;
    //         glVertex2f(radius*cos(angle)+initPos2, radius*sin(angle)+initPos2);
    //     }
    // glEnd();
    // glBegin(GL_LINES);
    //     glVertex2f(initPos1, initPos1);
    //     glVertex2f(initPos2, initPos2);
    // glEnd();
    glBegin(GL_LINES);
        glVertex2f();
    glEnd();


    glFlush();
    // glutPostRedisplay();
}

void init() {
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500); 
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Kinematics");
    init();
    // makeBones();
    glutDisplayFunc(display); 
    glutMainLoop();
    return 0;
}