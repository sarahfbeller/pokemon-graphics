#include <GLUT/glut.h>
#include <math.h>
#include <vector>
#include <iostream>

struct Bone {
    float x0, y0, x1, y1;
    struct Bone *parent;
};
std::vector<struct Bone *> bones;

float rotDelta = 0.1;
float rot = 0.0;
float transDelta = 0.001;
float offset_x = 0.0;
float offset_y = 0.0;

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

void drawBone(struct Bone *bone) {
    glPushMatrix();
    glTranslatef(offset_x, offset_y, 0.0);
    if (bone->parent) {
        glTranslatef(bone->parent->x0, bone->parent->y0, 0.0);
        glRotatef(rot, 0.0, 0.0, 1.0);
        glTranslatef(-1*bone->parent->x0, -1*bone->parent->y0, 0.0);
    }

    glBegin(GL_LINES);
    if (!bone->parent) {
        glVertex2f(bone->x0, bone->y0);
        glVertex2f(bone->x1, bone->y1);
    } else {
        glColor3f(0.6f,0.2f,0.6f);
        glVertex2f(bone->parent->x0, bone->parent->y0);
        glVertex2f(bone->x1, bone->y1);
    }
    glEnd();
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.f,0.f,0.f);
    rot += rotDelta;
    offset_x += transDelta;
    offset_y += transDelta;

    for(int i = 0; i < bones.size(); i++) {
        struct Bone *bone = bones.at(i);
        drawBone(bone);
    }
    glFlush();
    glutPostRedisplay();
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