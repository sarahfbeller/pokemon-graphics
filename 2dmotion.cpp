#include <GLUT/glut.h>
#include "SimpleImage.h"
#include <math.h>
#include <vector>
#include <iostream>

struct Bone {
    float x0, y0, length, width; // starting point, length, and width of bone
    float angle, minAngle, maxAngle; // initial angle and limits of motion
    std::vector<struct Bone *> children;
    RGBColor color;
};
std::vector<struct Bone *> bones; // bones[0] = root

// amount to add to rotation/translation with each frame / keystroke
float rotDelta = 0.1; // in degrees
float transDelta = 0.001;

void makeBone(struct Bone *bone, float x0, float y0, float length, float width,
            float angle, float minAngle, float maxAngle,
            std::vector<struct Bone *> children, RGBColor color) {
    bone->x0 = x0;
    bone->y0 = y0;
    bone->angle = angle;
    bone->minAngle = minAngle;
    bone->maxAngle = maxAngle;
    bone->length = length;
    bone->width = width;
    bone->children = children;
    bone->color = color;
}

/* Currently makes upper arm, lower arm, and 5 hand bones;
 * joints at elbow and wrist
 * First bone added must be root */
void makeBones() {
    bones = std::vector<struct Bone *>();
    struct Bone *a = new struct Bone;
    struct Bone *b = new struct Bone;
    struct Bone *c = new struct Bone;
    struct Bone *d = new struct Bone;
    struct Bone *e = new struct Bone;
    struct Bone *f = new struct Bone;
    struct Bone *g = new struct Bone;

    std::vector<struct Bone *> children;
    children.push_back(b);
    // upper arm
    makeBone(a, 0.3, 0.3, 0.35, 0.05, 20.0, -90.0, 
            90.0, children, RGBColor(0.5, 0.3, 0.6));
    // lower arm
    children[0] = c;
    children.push_back(d);
    children.push_back(e);
    children.push_back(f);
    children.push_back(g);
    makeBone(b, 0.0, 0.0, 0.3, 0.05, 40.0, 0.0, 
            135.0, children, RGBColor(0.2, 0.8, 0.2));
    // fingers
    makeBone(c, 0.0, 0.0, 0.05, 0.005, 10.0, 0.0, 
            20.0, std::vector<struct Bone *>(), RGBColor(0.8, 0.2, 0.2));    
    makeBone(d, 0.0, 0.0, 0.05, 0.005, 5.0, -5.0, 
            15.0, std::vector<struct Bone *>(), RGBColor(0.8, 0.2, 0.2)); 
    makeBone(e, 0.0, 0.0, 0.05, 0.005, 0.0, -10.0, 
            10.0, std::vector<struct Bone *>(), RGBColor(0.8, 0.2, 0.2)); 
    makeBone(f, 0.0, 0.0, 0.05, 0.005, -5.0, -15.0, 
            5.0, std::vector<struct Bone *>(), RGBColor(0.8, 0.2, 0.2)); 
    makeBone(g, 0.0, 0.0, 0.05, 0.005, -10.0, -20.0, 
            0.0, std::vector<struct Bone *>(), RGBColor(0.8, 0.2, 0.2)); 

    bones.push_back(a);
    bones.push_back(b);
    bones.push_back(c);
    bones.push_back(d);
    bones.push_back(e);
    bones.push_back(f);
    bones.push_back(g);
}

/* recursively draws each bone, then its children */
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

    // // makes bones as lines
    // glBegin(GL_LINES);
    //     glVertex2f(0.0, 0.0);
    //     glVertex2f(bone->length, 0.0);
    // glEnd();

    // makes bones as rectangles
    glColor3f(bone->color.r, bone->color.g, bone->color.b);
    glBegin(GL_QUADS);
        glVertex2f(0.0, -0.5*bone->width);
        glVertex2f(0.0, 0.5*bone->width);
        glVertex2f(bone->length, 0.5*bone->width);
        glVertex2f(bone->length, -0.5*bone->width);
    glEnd();
    glTranslatef(bone->length, 0.0, 0.0);

    for(int i = 0; i < bone->children.size(); i++) {
        drawBone(bone->children[i], false);
    }
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.f,0.f,0.f);
    drawBone(bones[0], true);
    glFlush();
    glutPostRedisplay();
}

/* keystrokes to rotate each of the 3 bone segments */
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