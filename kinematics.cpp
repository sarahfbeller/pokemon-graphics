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
    a->x0 = 0.3;
    a->y0 = 0.3;
    a->angle = 20.0;
    a->minAngle = -90.0;
    a->maxAngle = 90.0;
    a->len = 0.2;
    a->children[0] = b;
    a->numChildren++;

    b->x0 = 0.0;
    b->y0 = 0.0;
    b->angle = 40.0;
    b->minAngle = 0.0;
    b->maxAngle = 135.0;
    b->len = 0.2;
    b->children[0] = c;
    b->numChildren++;

    c->x0 = 0.0;
    c->y0 = 0.0;
    c->angle = 10.0;
    c->minAngle = -10.0;
    c->maxAngle = 10.0;
    c->len = 0.05;
    bones.push_back(a);
    bones.push_back(b);
    bones.push_back(c);
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
        struct Bone * bone = bones[2];
        float newAngle = bone->angle + rotDelta;
        if (newAngle >= bone->minAngle && newAngle <= bone->maxAngle) {
            bone->angle = newAngle;
        } 
        glutPostRedisplay();       
    } else if (key == 'i') { // rotate hand clockwise
        struct Bone * bone = bones[2];
        float newAngle = bone->angle - rotDelta;
        if (newAngle >= bone->minAngle && newAngle <= bone->maxAngle) {
            bone->angle = newAngle;
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