#include <GLUT/glut.h>
#include <math.h>
#include <vector>
#include <iostream>

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
    bones = std::vector<struct Bone *>();
    struct Bone *a = new struct Bone;
    a->x0 = 0.5;
    a->y0 = 0.5;
    a->x1 = 0.7;
    a->y1 = 0.7;
    a->parent = NULL;
    bones.push_back(a);
    struct Bone *b = new struct Bone;
    b->x0 = 0.0;
    b->y0 = 0.0;
    b->x1 = 0.3;
    b->y1 = 0.3;
    b->parent = a;
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
        for(int i = 0; i < bones.size(); i++) {
            struct Bone *bone = bones.at(i);
            if (!bone->parent) {
                glVertex2f(bone->x0, bone->y0);
                glVertex2f(bone->x1, bone->y1);
            } else {
                glVertex2f(bone->parent->x0, bone->parent->y0);
                glVertex2f(bone->x1, bone->y1);
            }
        }
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
    makeBones();
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500); 
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Kinematics");
    init();
    glutDisplayFunc(display); 
    glutMainLoop();
    return 0;
}