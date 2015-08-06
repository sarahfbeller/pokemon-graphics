#include <GLUT/glut.h>
#include <math.h>
#include <vector>
#include <iostream>

struct Bone {
    float x0, y0, angle, len, minAngle, maxAngle;
    struct Bone *children[5];
    int numChildren;
};
std::vector<struct Bone *> bones;

float rotDelta = 0.1;
float transDelta = 0.001;

// first bone is root
void makeBones() {
    bones = std::vector<struct Bone *>();
    struct Bone *a = new struct Bone;
    struct Bone *b = new struct Bone;
    struct Bone *c = new struct Bone;
    struct Bone *d = new struct Bone;
    struct Bone *e = new struct Bone;
    struct Bone *f = new struct Bone;
    struct Bone *g = new struct Bone;

    a->x0 = 0.3;
    a->y0 = 0.3;
    a->angle = 20.0;
    a->minAngle = -90.0;
    a->maxAngle = 90.0;
    a->len = 0.35;
    a->children[0] = b;
    a->numChildren++;

    b->x0 = 0.0;
    b->y0 = 0.0;
    b->angle = 40.0;
    b->minAngle = 0.0;
    b->maxAngle = 135.0;
    b->len = 0.3;
    b->children[0] = c;
    b->children[1] = d;
    b->children[2] = e;
    b->children[3] = f;
    b->children[4] = g;
    b->numChildren += 5;

    c->x0 = 0.0;
    c->y0 = 0.0;
    c->angle = 10.0;
    c->minAngle = 0.0;
    c->maxAngle = 20.0;
    c->len = 0.05;
    d->x0 = 0.0;
    d->y0 = 0.0;
    d->angle = 5.0;
    d->minAngle = -5.0;
    d->maxAngle = 15.0;
    d->len = 0.05;
    e->x0 = 0.0;
    e->y0 = 0.0;
    e->angle = 0.0;
    e->minAngle = -10.0;
    e->maxAngle = 10.0;
    e->len = 0.05;
    f->x0 = 0.0;
    f->y0 = 0.0;
    f->angle = -5.0;
    f->minAngle = -15.0;
    f->maxAngle = 5.0;
    f->len = 0.05;
    g->x0 = 0.0;
    g->y0 = 0.0;
    g->angle = -10.0;
    g->minAngle = -20.0;
    g->maxAngle = 0.0;
    g->len = 0.05;

    bones.push_back(a);
    bones.push_back(b);
    bones.push_back(c);
    bones.push_back(d);
    bones.push_back(e);
    bones.push_back(f);
    bones.push_back(g);
}

void drawBone(struct Bone *bone, bool isRoot) {
    // // rotate entire structure
    // float newAngle = bone->angle + rotDelta;
    // if (newAngle >= bone->minAngle && newAngle <= bone->maxAngle) {
    //     bone->angle = newAngle;
    // }
    // // translate entire structure
    // if(isRoot) {
    //     bone->x0 += transDelta;
    //     bone->y0 += transDelta;
    // }
    glPushMatrix();
    glTranslatef(bone->x0, bone->y0, 0.0);
    glRotatef(bone->angle, 0.0, 0.0, 1.0);

    glBegin(GL_LINES);
        glVertex2f(0.0, 0.0);
        glVertex2f(bone->len, 0.0);
    glEnd();
    glTranslatef(bone->len, 0.0, 0.0);

    for(int i = 0; i < bone->numChildren; i++) {
        drawBone(bone->children[i], false);
    }

    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.f,0.f,0.f);
    drawBone(bones.at(0), true);
    glFlush();
    glutPostRedisplay();
}

void keyFunc(unsigned char key, int x, int y) {
    if (key == 'd') { // rotate upper arm anticlockwise
        struct Bone * bone = bones[0];
        float newAngle = bone->angle + rotDelta;
        if (newAngle >= bone->minAngle && newAngle <= bone->maxAngle) {
            bone->angle = newAngle;
        }
        glutPostRedisplay();
    } else if (key == 'f') { // rotate upper arm clockwise
        struct Bone * bone = bones[0];
        float newAngle = bone->angle - rotDelta;
        if (newAngle >= bone->minAngle && newAngle <= bone->maxAngle) {
            bone->angle = newAngle;
        }
        glutPostRedisplay();
    } else if (key == 'j') { // rotate lower arm anticlockwise
        struct Bone * bone = bones[1];
        float newAngle = bone->angle + rotDelta;
        if (newAngle >= bone->minAngle && newAngle <= bone->maxAngle) {
            bone->angle = newAngle;
        }
        glutPostRedisplay();
    } else if (key == 'k') { // rotate lower arm clockwise
        struct Bone * bone = bones[1];
        float newAngle = bone->angle - rotDelta;
        if (newAngle >= bone->minAngle && newAngle <= bone->maxAngle) {
            bone->angle = newAngle;
        }
        glutPostRedisplay();      
    } else if (key == 'u') { // rotate hand anticlockwise
        for(int i = 2; i < 7; i++) {
            struct Bone * bone = bones[i];
            float newAngle = bone->angle + rotDelta;
            if (newAngle >= bone->minAngle && newAngle <= bone->maxAngle) {
                bone->angle = newAngle;
            }
        }
        glutPostRedisplay();       
    } else if (key == 'i') { // rotate hand clockwise
        for(int i = 2; i < 7; i++) {
            struct Bone * bone = bones[i];
            float newAngle = bone->angle - rotDelta;
            if (newAngle >= bone->minAngle && newAngle <= bone->maxAngle) {
                bone->angle = newAngle;
            }
        } 
        glutPostRedisplay();      
    }
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
    glutKeyboardFunc(keyFunc);
    glutDisplayFunc(display); 
    glutMainLoop();
    return 0;
}