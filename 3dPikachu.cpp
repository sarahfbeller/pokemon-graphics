#include <GLUT/glut.h>
#include "SimpleImage.h"
#include <math.h>
#include <cmath>
#include <vector>
#include <iostream>

struct Bone {
    float x0, y0, z0; // starting point
    float xdim, ydim, zdim; // x,y,z dimensions of bone
    float angle_x, minAngle_x, maxAngle_x; // initial angle and limits of motion
    float angle_y, minAngle_y, maxAngle_y;
    float angle_z, minAngle_z, maxAngle_z;
    std::vector<struct Bone *> children;
    RGBColor color;
};
std::vector<struct Bone *> bones; // bones[0] = root

// indices into bones array for skeleton bones
#define TORSO 0
#define LEFTARM 1
#define RIGHTARM 2
#define LEFTLEG 3
#define RIGHTLEG 4
#define HEAD 5
#define LEFTEAR 6
#define RIGHTEAR 7

// amount to add to rotation/translation with each frame / keystroke
float rotDelta = 0.05; // in degrees
float transDelta = 0.001;
// globals for rotation with mouse click/drag
int X = 0;
int Y = 0;
bool isClicked = false;
float rot_z = 0.0;
// globals for keyframe animation
float dt = 0.01;
float currTime = 0.0;
bool isWalking = false;
bool goingForward = false;
bool headShaking = false;
bool walkingAllFours = false;
float currBodyRotation = 0.0;
float currHeadRotation = 0.0;

void makeBone(struct Bone *bone, float x0, float y0, float z0,
            float xdim, float ydim, float zdim,
            float angle_x, float minAngle_x, float maxAngle_x,
            float angle_y, float minAngle_y, float maxAngle_y,
            float angle_z, float minAngle_z, float maxAngle_z,
            std::vector<struct Bone *> children, RGBColor color) {
    bone->x0 = x0;
    bone->y0 = y0;
    bone->z0 = z0;
    bone->angle_x = angle_x;
    bone->minAngle_x = minAngle_x;
    bone->maxAngle_x = maxAngle_x;
    bone->angle_y = angle_y;
    bone->minAngle_y = minAngle_y;
    bone->maxAngle_y = maxAngle_y;
    bone->angle_z = angle_z;
    bone->minAngle_z = minAngle_z;
    bone->maxAngle_z = maxAngle_z;
    bone->xdim = xdim;
    bone->ydim = ydim;
    bone->zdim = zdim;
    bone->children = children;
    bone->color = color;
}

/* First bone added must be root */
void makeBones() {
    bones = std::vector<struct Bone *>();
    struct Bone *torso = new struct Bone;
    struct Bone *leftArm = new struct Bone;
    struct Bone *rightArm = new struct Bone;
    struct Bone *leftLeg = new struct Bone;
    struct Bone *rightLeg = new struct Bone;
    struct Bone *head = new struct Bone;
    struct Bone *leftEar = new struct Bone;
    struct Bone *rightEar = new struct Bone;

    std::vector<struct Bone *> children;
    children.push_back(leftArm);
    children.push_back(rightArm);
    children.push_back(head);
    children.push_back(leftLeg);
    children.push_back(rightLeg);
    // torso
    float torsoLen = 0.4;
    makeBone(torso, 0.5, 0.25, 0.0, torsoLen, 0.1, 0.1, 0.0, 0.0, 360.0, 
            0.0, 0.0, 360.0, 90.0, 0.0, 360.0, 
            children, RGBColor(229/255.f, 218/255.f, 42/255.f));
    // head
    children.clear();
    children.push_back(leftEar);
    children.push_back(rightEar);
    makeBone(head, 0.0, 0.0, 0.0, 0.15, 0.15, 0.15, 0.0, 0.0, 0.0, 
            0.0, 0.0, 0.0, 0.0, -30.0, 30.0, 
            children, RGBColor(229/255.f, 228/255.f, 52/255.f));
    // arms
    children.clear();
    makeBone(leftArm, 0.0, 0.0, 0.0, 0.2, 0.05, 0.05, 0.0, 0.0, 0.0, 
            0.0, 0.0, 0.0, 120.0, 0.0, 180.0, 
            children, RGBColor(229/255.f, 218/255.f, 42/255.f));
    makeBone(rightArm, 0.0, 0.0, 0.0, 0.2, 0.05, 0.05, 0.0, 0.0, 0.0, 
            0.0, 0.0, 0.0, -120.0, -18.0, 0.0, 
            children, RGBColor(229/255.f, 218/255.f, 42/255.f));
    // legs
    makeBone(leftLeg, -1*torsoLen, 0.0, 0.0, 0.2, 0.05, 0.05, 0.0, 0.0, 0.0, 
            0.0, 0.0, 0.0, 160.0, 90.0, 180.0, 
            children, RGBColor(0.0, 1.0, 0.0));
    makeBone(rightLeg, -1*torsoLen, 0.0, 0.0, 0.2, 0.05, 0.05, 0.0, 0.0, 0.0, 
            0.0, 0.0, 0.0, -160.0, -90.0, -180.0, 
            children, RGBColor(0.0, 0.0, 1.0));
    // ears
    makeBone(leftEar, 0.0, 0.0, 0.0, 0.2, 0.05, 0.05, 0.0, 0.0, 0.0, 
            0.0, 0.0, 0.0, 30.0, 15.0, 45.0, 
            children, RGBColor(229/255.f, 228/255.f, 52/255.f));
    makeBone(rightEar, 0.0, 0.0, 0.0, 0.2, 0.05, 0.05, 0.0, 0.0, 0.0, 
            0.0, 0.0, 0.0, -30.0, -45.0, -15.0, 
            children, RGBColor(229/255.f, 228/255.f, 52/255.f));


    bones.push_back(torso);
    bones.push_back(leftArm);
    bones.push_back(rightArm);
    bones.push_back(leftLeg);
    bones.push_back(rightLeg);
    bones.push_back(head);
    bones.push_back(leftEar);
    bones.push_back(rightEar);
}

/* recursively draws each bone, then its children */
void drawBone(struct Bone *bone, bool isRoot) {
    // rotate entire structure
    // float newAngle_z = bone->angle_z + rotDelta;
    // if (newAngle_z >= bone->minAngle_z && newAngle_z <= bone->maxAngle_z) {
    //     bone->angle_z = newAngle_z;
    // }
    // // translate entire structure
    // if(isRoot) {
    //     bone->x0 += transDelta;
    //     bone->y0 += transDelta;
    // }
    glPushMatrix();
    glTranslatef(bone->x0, bone->y0, bone->z0);
    glRotatef(bone->angle_x, 1.0, 0.0, 0.0);
    glRotatef(bone->angle_y, 0.0, 1.0, 0.0);
    glRotatef(bone->angle_z, 0.0, 0.0, 1.0);

    // // makes bones as lines
    // glBegin(GL_LINES);
    //     glVertex2f(0.0, 0.0);
    //     glVertex2f(bone->xdim, 0.0);
    // glEnd();

    // makes bones as cuboids
    glColor3f(bone->color.r, bone->color.g, bone->color.b);
    glBegin(GL_QUADS);
        glVertex3f(0.0, -0.5*bone->ydim, -0.5*bone->zdim);
        glVertex3f(0.0, 0.5*bone->ydim, -0.5*bone->zdim);
        glVertex3f(bone->xdim, 0.5*bone->ydim, -0.5*bone->zdim);
        glVertex3f(bone->xdim, -0.5*bone->ydim, -0.5*bone->zdim);

        glVertex3f(0.0, -0.5*bone->ydim, 0.5*bone->zdim);
        glVertex3f(0.0, 0.5*bone->ydim, 0.5*bone->zdim);
        glVertex3f(bone->xdim, 0.5*bone->ydim, 0.5*bone->zdim);
        glVertex3f(bone->xdim, -0.5*bone->ydim, 0.5*bone->zdim);

        glVertex3f(0.0, 0.5*bone->ydim, -0.5*bone->zdim);
        glVertex3f(0.0, 0.5*bone->ydim, 0.5*bone->zdim);
        glVertex3f(bone->xdim, 0.5*bone->ydim, 0.5*bone->zdim);
        glVertex3f(bone->xdim, 0.5*bone->ydim, -0.5*bone->zdim);

        glVertex3f(0.0, -0.5*bone->ydim, -0.5*bone->zdim);
        glVertex3f(0.0, -0.5*bone->ydim, 0.5*bone->zdim);
        glVertex3f(bone->xdim, -0.5*bone->ydim, 0.5*bone->zdim);
        glVertex3f(bone->xdim, -0.5*bone->ydim, -0.5*bone->zdim);

        glVertex3f(0.0, 0.5*bone->ydim, -0.5*bone->zdim);
        glVertex3f(0.0, 0.5*bone->ydim, 0.5*bone->zdim);
        glVertex3f(0.0, -0.5*bone->ydim, 0.5*bone->zdim);
        glVertex3f(0.0, -0.5*bone->ydim, -0.5*bone->zdim);

        glVertex3f(bone->xdim, 0.5*bone->ydim, -0.5*bone->zdim);
        glVertex3f(bone->xdim, 0.5*bone->ydim, 0.5*bone->zdim);
        glVertex3f(bone->xdim, -0.5*bone->ydim, 0.5*bone->zdim);
        glVertex3f(bone->xdim, -0.5*bone->ydim, -0.5*bone->zdim);
    glEnd();
    glTranslatef(bone->xdim, 0.0, 0.0);

    for(int i = 0; i < bone->children.size(); i++) {
        drawBone(bone->children[i], false);
    }
    glPopMatrix();
}

/* Makes skeleton walk: legs move and body sways as Pikachu waddles. */
void walk() {
    float t0 = 0.0;
    float t1 = 1.0;
    float t2 = 2.0;
    float t0LegAngle = -20.0;
    float t1LegAngle = 20.0;
    float t2LegAngle = -20.0;

    float newAngle = 0.0;
    if (currTime <= t0+dt) {
        newAngle = t0LegAngle;
    } else if (currTime >= t1-(dt/2) && currTime <= t1+(dt/2)) {
        newAngle = t1LegAngle;
    } else if (currTime >= t2) {
        newAngle = t2LegAngle;
        currTime = t0;
    } else {
        if (currTime < t1) {
            newAngle = ((currTime/(t1-t0))*(t1LegAngle-t0LegAngle)) + t0LegAngle;
        } else if (currTime < t2) {
            newAngle = (((currTime-t1)/(t2-t1))*(t2LegAngle-t1LegAngle)) + t1LegAngle;
        }
    }
    bones[LEFTLEG]->angle_y = newAngle;
    bones[RIGHTLEG]->angle_y = -1*newAngle;

    float t0BodyAngle = 2.0;
    float t1BodyAngle = -2.0;
    float t2BodyAngle = 2.0;
    float deltaAngle = 90.0;

    if (currTime <= t0+dt) {
        deltaAngle += t0BodyAngle - currBodyRotation;
    } else if (currTime >= t2) {
        deltaAngle += t2BodyAngle - currBodyRotation;
        currTime = t0;
    } else {
        float newAngle = 0.0;
        if (currTime < t1) {
            newAngle = ((currTime/(t1-t0))*(t1BodyAngle-t0BodyAngle)) + t0BodyAngle;
        } else if (currTime < t2) {
            newAngle = (((currTime-t1)/(t2-t1))*(t2BodyAngle-t1BodyAngle)) + t1BodyAngle;
        }
        deltaAngle += newAngle - currBodyRotation;
    }
    currBodyRotation += deltaAngle;
    bones[TORSO]->angle_z = currBodyRotation;

    currTime += dt;
    glutPostRedisplay();
}

/* Pikachu walks on all four limbs. */
void walkOnAllFours() {

}

/* Moves in direction of walking. */
void goForward() {
    glTranslatef(0.0, 0.0, transDelta);
    glutPostRedisplay();
}

/* Head sways from side to side. */
void shakeHead() {
    float t0 = 0.0;
    float t1 = 1.0;
    float t2 = 2.0;
    float t0HeadAngle = 5.0;
    float t1HeadAngle = -5.0;
    float t2HeadAngle = 5.0;
    float deltaAngle = 0.0;

    if (currTime <= t0+dt) {
        deltaAngle += t0HeadAngle - currHeadRotation;
    } else if (currTime >= t1-(dt/2) && currTime <= t1+(dt/2)) {
        deltaAngle += t1HeadAngle - currHeadRotation;
    }else if (currTime >= t2) {
        deltaAngle += t2HeadAngle - currHeadRotation;
        currTime = t0;
    } else {
        float newAngle = 0.0;
        if (currTime <= t1) {
            newAngle = ((currTime/(t1-t0))*(t1HeadAngle-t0HeadAngle)) + t0HeadAngle;
        } else if (currTime < t2) {
            newAngle = (((currTime-t1)/(t2-t1))*(t2HeadAngle-t1HeadAngle)) + t1HeadAngle;
        }
        deltaAngle += newAngle - currHeadRotation;
    }
    currHeadRotation += deltaAngle;
    bones[HEAD]->angle_z = currHeadRotation;
    if (!isWalking) currTime += dt;
    glutPostRedisplay();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.f,0.f,0.f);
    if (isWalking) walk();
    if (goingForward) goForward();
    if (headShaking) shakeHead();
    if (walkingAllFours) walkOnAllFours();
    drawBone(bones[0], true);
    glFlush();
    glutSwapBuffers();
}

// void timerFunc(int v) {
//     walk();
//     currTime += 0.01;
//     glutPostRedisplay();
//     glutTimerFunc(1, timerFunc, v);
// }

void keyFunc(unsigned char key, int x, int y) {
    if (key == 'd') { // rotate left leg anticlockwise around z axis
        struct Bone * bone = bones[5];
        float newAngle_z = bone->angle_z + 5*rotDelta;
        if (newAngle_z >= bone->minAngle_z && newAngle_z <= bone->maxAngle_z) {
            bone->angle_z = newAngle_z;
        }
        glutPostRedisplay();
    } else if (key == 'w') { // start/stop walking
        isWalking = !isWalking;
        currTime = 0.0;
        glutPostRedisplay();
    } else if (key == 'r') { // rotate scene by 90 degrees around y axis
        glTranslatef(0.5, 0.0, 0.0);
        glRotatef(90.0, 0.0, 1.0, 0.0);
        glutPostRedisplay();
    } else if (key == 'e') { // rotate scene by 45 degrees around y axis
        glTranslatef(0.3, 0.0, 0.0);
        glRotatef(45.0, 0.0, 1.0, 0.0);
        glutPostRedisplay();
    } else if (key == 'm') { // move/stop moving forward
        goingForward = !goingForward;
        glutPostRedisplay();
    } else if (key == 'h') { // move/stop moving head
        headShaking = !headShaking;
        glutPostRedisplay();
    }
}

void mouseFunc(int button, int state, int x, int y) {
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        isClicked = true;
        X = x;
        Y = y;
    }
}

void motionFunc(int curr_x, int curr_y) {
    if(isClicked) {
        float dx = float(X) - curr_x;
        float dy = float(Y) - curr_y;
        glRotatef(dx, 1.0, 0.0, 0.0);
        glRotatef(dy, 0.0, 1.0, 0.0);
    }
    glutPostRedisplay();
    X = curr_x;
    Y = curr_y;
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
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(500, 500); 
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Kinematics");
    init();
    glutKeyboardFunc(keyFunc);
    glutMouseFunc(mouseFunc);
    glutMotionFunc(motionFunc);
    glutDisplayFunc(display);
    // glutTimerFunc(100, timerFunc, 0);
    glutMainLoop();
    return 0;
}