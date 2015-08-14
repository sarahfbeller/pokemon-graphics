
#include "alm_main.h"
#ifdef WIN32
#define ssize_t SSIZE_T
#endif

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <cmath>
// #include "SimpleTexture.h"


// ====================== Draw Scene Helpers ================= //

/* recursively draws each bone, then its children */
void drawBone(struct Bone *bone) {
    glPushMatrix();
    glTranslatef(bone->x0, bone->y0, bone->z0);
    glRotatef(bone->angle_x, 1.0, 0.0, 0.0);
    glRotatef(bone->angle_y, 0.0, 1.0, 0.0);
    glRotatef(bone->angle_z, 0.0, 0.0, 1.0);

    // makes bones as cuboids
    glColor3f(1.0,1.0,1.0);
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
        drawBone(bone->children[i]);
    }
    glPopMatrix();
}

void drawGround(){
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, groundImgID);
    glBegin(GL_QUADS);
        glTexCoord2f(1.f, 0.f);
        glVertex3f(-quad_size, ground_level, quad_size);
        glTexCoord2f(1.f, 1.f);
        glVertex3f(quad_size, ground_level, quad_size);
        glTexCoord2f(0.f, 1.f);
        glVertex3f(quad_size, ground_level, -quad_size);
        glTexCoord2f(0.f, 0.f);
        glVertex3f(-quad_size, ground_level, -quad_size);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void drawSky(){
    glPushMatrix();
    skyShader->Bind();
    glDisable(GL_CULL_FACE);
    GLUquadricObj* quadric = gluNewQuadric();
    gluQuadricDrawStyle(quadric,GLU_FILL);
    gluSphere(quadric,quad_size * 2,20,20);
    gluDeleteQuadric(quadric);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    skyShader->UnBind();
    glPopMatrix();
}

void drawScene(){
    glPushMatrix();    
    glScalef(2.0f, 1.0f, 2.0f);
    for(int f = 0; f < (q->obj_faces).size(); f++){
        Face cur_face = q->obj_faces.at(f);
        std::vector <Index> indices = cur_face.indices;

        if(q->text && q->mat_map[cur_face.mat_id].texture != "")
            mtl_init(q->mat_map[cur_face.mat_id].texture); 
 
        glBegin(GL_TRIANGLES);
            for (int i = 0; i < 3; i++){
                Vertex * v = &q->obj_vertices[indices[i].v_ind];
                if (q->text) {
                    Texture * t = &q->obj_textures[indices[i].vt_ind];
                    glTexCoord2f(t->text_1, 1 - t->text_2);
                }
                if(q->norm){
                    Normal * n = &q->obj_normals[indices[i].vn_ind];
                    glNormal3f(n->n_x, n->n_y, n->n_z);    
                } else {
                    Normal * n = (q->cal_norm(q->obj_vertices[indices[0].v_ind], q->obj_vertices[indices[1].v_ind], q->obj_vertices[indices[2].v_ind]));
                    glNormal3f(n->n_x, n->n_y, n->n_z);                    
                }
                glVertex3f(v->x_val + x_position, v->y_val + .02f, v->z_val + z_position);
            }
        glEnd();
    }
    glPopMatrix(); 
    glScalef(1.0f, 1.0f, 1.0f);
}

void drawWalls(){
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, memchuImgID);
    glBegin(GL_QUADS);
        glTexCoord2f(1.f, 0.f);
        glVertex3f(-quad_size, quad_height, -quad_size);
        glTexCoord2f(0.f, 0.f);
        glVertex3f(-quad_size, quad_height, quad_size);
        glTexCoord2f(0.f, 1.f);
        glVertex3f(-quad_size, ground_level, quad_size);
        glTexCoord2f(1.f, 1.f);
        glVertex3f(-quad_size, ground_level, -quad_size);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, backImgID);
    glBegin(GL_QUADS);
        glTexCoord2f(1.f, 1.f);
        glVertex3f(quad_size, ground_level, -quad_size);
        glTexCoord2f(0.f, 1.f);
        glVertex3f(quad_size, ground_level, quad_size);
        glTexCoord2f(0.f, 0.f);
        glVertex3f(quad_size, quad_height, quad_size);
        glTexCoord2f(1.f, 0.f);
        glVertex3f(quad_size, quad_height, -quad_size);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, sidesImgID);
    glBegin(GL_QUADS);
        glTexCoord2f(1.f, 1.f);
        glVertex3f(-quad_size, ground_level, -quad_size);
        glTexCoord2f(0.f, 1.f);
        glVertex3f(quad_size, ground_level, -quad_size);
        glTexCoord2f(0.f, 0.f);
        glVertex3f(quad_size, quad_height, -quad_size);
        glTexCoord2f(1.f, 0.f);
        glVertex3f(-quad_size, quad_height, -quad_size);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, sidesImgID);
    glBegin(GL_QUADS);
        glTexCoord2f(1.f, 0.f);
        glVertex3f(-quad_size, quad_height, quad_size);
        glTexCoord2f(0.f, 0.f);
        glVertex3f(quad_size, quad_height, quad_size);
        glTexCoord2f(0.f, 1.f);
        glVertex3f(quad_size, ground_level, quad_size);
        glTexCoord2f(1.f, 1.f);
        glVertex3f(-quad_size, ground_level, quad_size);
    glEnd();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}

void drawCharacter(){
    glPushMatrix();
    // facing direction
    glTranslatef(x_position, 0, z_position);
    glRotatef(facing_angle, 0, 1, 0);
    glTranslatef(-x_position, 0, -z_position);
    

    for(int f = 0; f < (p->obj_faces).size(); f++){
        Face cur_face = p->obj_faces.at(f);
        std::vector <Index> indices = cur_face.indices;

        if(p->text && p->mat_map[cur_face.mat_id].texture != ""){
            mtl_init(p->mat_map[cur_face.mat_id].texture); 
        } else {
            shader1->Bind();
            shader1->SetUniform("x_position", x_position);
            shader1->SetUniform("z_position", z_position);
            shader1->SetUniform("facing_angle", facing_angle);
        }

        glBegin(GL_TRIANGLES);
            for (int i = 0; i < 3; i++){
                Vertex * v = &p->obj_vertices[indices[i].v_ind];
                if (p->text) {
                    Texture * t = &p->obj_textures[indices[i].vt_ind];
                    glTexCoord2f(t->text_1, 1 - t->text_2);
                }
                if(p->norm){
                    Normal * n = &p->obj_normals[indices[i].vn_ind];
                    glNormal3f(n->n_x, n->n_y, n->n_z);    
                } else {
                    Normal * n = (p->cal_norm(p->obj_vertices[indices[0].v_ind], p->obj_vertices[indices[1].v_ind], p->obj_vertices[indices[2].v_ind]));
                    glNormal3f(n->n_x, n->n_y, n->n_z);                    
                }
                glVertex3f(v->x_val + x_position, v->y_val, v->z_val + z_position);

                // float x_transformed = transformX(v->x_val);
                // float y_transformed = transformY(v->y_val);
                // float z_transformed = transformZ(v->z_val);
                // glVertex3f(x_transformed + x_position, y_transformed, z_transformed + z_position);
            }
        glEnd();

    }
    // if(p->text){
    //     glDisable(GL_TEXTURE_2D);
    // } else {
        shader1->UnBind();  
    // }

    glPopMatrix();
}


// ========================= Drawing Wrapper ================== //
void DrawingWrapper(){
    glRotatef(angle, 0, 1, 0);
    gluLookAt(x, vert_camera_pos + 1.0f, z, x_position, ly, z_position + lz, 0.0f, 1.0f, 0.0f);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    drawGround();
    drawSky();
    drawWalls();
    // drawScene();
    // drawBone(bones[0]);
    drawCharacter();
}


// =================== Setup Functions ===================== //q
void SetupLighting() {
    GLfloat light_position[] = { 0.0, 20.0, 0.0, 0.0 };
    GLfloat white_light[] = { .8, .8, .8, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 60.0 };
    GLfloat lmodel_ambient[] = { 1.0, 1.0, 1.0, 1.0 };

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHTING);
}

void SetupCamera() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1, 1, -1, 1, 1, 20);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 10.0,
        0.0, 0.0, 1.0,
        0.0, 1.0, 0.0);
}

void makeBone(struct Bone *bone, float x0, float y0, float z0,
            float xdim, float ydim, float zdim,
            float angle_x, float minAngle_x, float maxAngle_x,
            float angle_y, float minAngle_y, float maxAngle_y,
            float angle_z, float minAngle_z, float maxAngle_z,
            std::vector<struct Bone *> children) 
{
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
}

/* First bone added must be root.
 * Currently makes torso, arms, legs, head, and ears. */
void makeBones() {
    bones = std::vector<struct Bone *>();
    struct Bone *torso = new struct Bone;
    // struct Bone *leftArm = new struct Bone;
    // struct Bone *rightArm = new struct Bone;
    // struct Bone *leftLeg = new struct Bone;
    // struct Bone *rightLeg = new struct Bone;
    // struct Bone *head = new struct Bone;
    // struct Bone *leftEar = new struct Bone;
    // struct Bone *rightEar = new struct Bone;

    std::vector<struct Bone *> children;
    // children.push_back(leftArm);
    // children.push_back(rightArm);
    // children.push_back(head);
    // children.push_back(leftLeg);
    // children.push_back(rightLeg);
    // torso
    float torsoLen = 2.0;
    makeBone(torso, 0.5, 0.25, 0.0, torsoLen, 0.5, 0.5, 0.0, 0.0, 360.0, 
            0.0, 0.0, 360.0, 90.0, 0.0, 360.0, children);

    // head
    // children.clear();
    // children.push_back(leftEar);
    // children.push_back(rightEar);
    // makeBone(head, 0.0, 0.0, 0.0, 0.6, 0.6, 0.6, 0.0, 0.0, 0.0, 
    //         0.0, 0.0, 0.0, 0.0, -30.0, 30.0, children);
    // // arms
    // children.clear();
    // makeBone(leftArm, 0.0, 0.0, 0.0, 0.8, 0.2, 0.2, 0.0, 0.0, 0.0, 
    //         0.0, 0.0, 0.0, 120.0, 0.0, 180.0, children);
    // makeBone(rightArm, 0.0, 0.0, 0.0, 0.2, 0.05, 0.05, 0.0, 0.0, 0.0, 
    //         0.0, 0.0, 0.0, -120.0, -18.0, 0.0, children);
    // // legs
    // makeBone(leftLeg, -1*torsoLen, 0.0, 0.0, 0.8, 0.2, 0.2, 0.0, 0.0, 0.0, 
    //         0.0, 0.0, 0.0, 160.0, 90.0, 180.0, children);
    // makeBone(rightLeg, -1*torsoLen, 0.0, 0.0, 0.8, 0.2, 0.2, 0.0, 0.0, 0.0, 
    //         0.0, 0.0, 0.0, -160.0, -90.0, -180.0, children);
    // // ears
    // makeBone(leftEar, 0.0, 0.0, 0.0, 0.8, 0.2, 0.2, 0.0, 0.0, 0.0, 
    //         0.0, 0.0, 0.0, 30.0, 15.0, 45.0, children);
    // makeBone(rightEar, 0.0, 0.0, 0.0, 0.8, 0.2, 0.2, 0.0, 0.0, 0.0, 
    //         0.0, 0.0, 0.0, -30.0, -45.0, -15.0, children);

    bones.push_back(torso);
    // bones.push_back(leftArm);
    // bones.push_back(rightArm);
    // bones.push_back(leftLeg);
    // bones.push_back(rightLeg);
    // bones.push_back(head);
    // bones.push_back(leftEar);
    // bones.push_back(rightEar);
}

void SetupWrapper(){
    makeBones();

    shader1 = new SimpleShaderProgram();
    shader1->LoadVertexShader(vertexShader);
    shader1->LoadFragmentShader(fragmentShader);

    wallShader = new SimpleShaderProgram();
    wallShader->LoadVertexShader(wallVertexShader);
    wallShader->LoadFragmentShader(wallFragmentShader);

    groundShader = new SimpleShaderProgram();
    groundShader->LoadVertexShader(groundVertexShader);
    groundShader->LoadFragmentShader(groundFragmentShader);   

    skyShader = new SimpleShaderProgram();
    skyShader->LoadVertexShader(skyVertexShader);
    skyShader->LoadFragmentShader(skyFragmentShader);     

    glEnable(GL_TEXTURE_2D);

    groundIMG = SimpleImage(ground_file_name);
        int w = groundIMG.width();
        int h = groundIMG.height();
        glGenTextures(1, &groundImgID);
        glBindTexture(GL_TEXTURE_2D, groundImgID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_FLOAT, groundIMG.data());
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

    sidesIMG = SimpleImage(sides_file_name);
        w = sidesIMG.width();
        h = sidesIMG.height();
        glGenTextures(1, &sidesImgID);
        glBindTexture(GL_TEXTURE_2D, sidesImgID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_FLOAT, sidesIMG.data());
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

    std::string memchu_file_name = "meshes/Textures/memchu.jpg";
    memchuIMG = SimpleImage(memchu_file_name);
        w = memchuIMG.width();
        h = memchuIMG.height();
        glGenTextures(1, &memchuImgID);
        glBindTexture(GL_TEXTURE_2D, memchuImgID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_FLOAT, memchuIMG.data());
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

    std::string back_file_name = "meshes/Textures/back_quad.jpg";
    backIMG = SimpleImage(back_file_name);
        w = backIMG.width();
        h = backIMG.height();
        glGenTextures(1, &backImgID);
        glBindTexture(GL_TEXTURE_2D, backImgID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_FLOAT, backIMG.data());
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);


    // makeBones();
    // std::cout<<bones.size()<<std::endl;

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    SetupCamera();
    SetupLighting();
    glEnable(GL_DEPTH_TEST);
}

// =================== Animation Functions ================= //
/* Makes skeleton walk: legs move and body and head sway as Pikachu waddles. */
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

    float t0HeadAngle = 5.0;
    float t1HeadAngle = -5.0;
    float t2HeadAngle = 5.0;
    deltaAngle = 0.0;

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

    currTime += dt;
    glutPostRedisplay();
}

// =================== Callback Functions ================== //
void DisplayCallback(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.f, 0.f, -10.f);
    if (isWalking) walk();
    DrawingWrapper();
    glutSwapBuffers();
}

void ReshapeCallback(int w, int h){
    glViewport(0, 0, w, h);

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective(30.0f, (float)w/(float)h, 0.1f, 100000.f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void KeyCallback(unsigned char key, int x, int y){
    switch(key) {
    case 'q':
        exit(0);

    // Zoom in/out
    case 'u':
        z += .60;
        lz -= .60;
        ly -= .06;
        break;
    case 'o':
        z -= .40;
        lz += .40;
        ly += .04;
        break;

    // Change camera view
    case 'l':
        angle += 10.f;
        break;
    case 'j':
        angle -= 10.f;
        break;
    case 'i':
        vert_camera_pos += .2f;
        break;
    case ',':
        vert_camera_pos -= .2f;
        break;    
    case 'm':
        facing_angle -= 8.f;
        break;
    case '.':
        facing_angle += 8.f;

    // Move Character
    case 's':
        x_position -= .2f;
        break;
    case 'f':
        x_position += .2f;
        break;
    case 'e':
        z_position += .5f * cos(pi * facing_angle/180.f);
        x_position += .5f * sin(pi * facing_angle/180.f);
        break;
    case 'x':
        z_position -= .5f * cos(pi * facing_angle/180.f);
        x_position -= .5f * sin(pi * facing_angle/180.f);
        break;    
    case 'w':
        isWalking = !isWalking;
        currTime = 0.0;
        break;

    // Reset Character Position
    case 'a':
        x_position = 1.0;
        z_position = 1.0;
    default:
        break;
    }
    z_position = fmax(z_position, -quad_size + 4.f);
    z_position = fmin(z_position, quad_size - 4.f);

    x_position = fmax(x_position, -quad_size + 4.f);
    x_position = fmin(x_position, quad_size - 4.f);
    glutPostRedisplay();
}

void MouseCallback(int button, int state, int x, int y) {

    if (button == GLUT_LEFT_BUTTON) Button = 1;
    else if (button == GLUT_RIGHT_BUTTON) Button = 2;

    if (state == GLUT_UP) {
        Button = 0;
        //glSaveModelView();
    } else if (state == GLUT_DOWN) {
        X = x;
        Y = y;
        glPushMatrix();
    }
}

void TransformCallback(int curr_x, int curr_y) {
    int dx = X - curr_x;
    int dy = Y - curr_y;
    if (Button == 1) {
        //calculating delta0 and delta1
        float delta0 = (dx);
        float delta1 = -(dy);
        glRotatef(delta1, 1, 0, 0);
        glRotatef(delta0, 0, 1, 0);
    } else if (Button == 2) {
        //zooming based on y-motion
        float scale_val = 1 + 0.005 * (float)dy;
        glScalef(scale_val, scale_val, scale_val);
    }
    
    glutPostRedisplay();
    X = curr_x;
    Y = curr_y;
}


// ========================= Main =========================== //
int main(int argc, char** argv){
    if(!(argc == 2)){
        printf("usage: ./3dPokemon <mesh> \n");
        return 0;
    }

    std::string input_file = "";
    input_file  = argv[1];

    // Initialize GLUT.
    glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(640, 480);
    glutCreateWindow("Pokemon");

    //
    // Initialize GLEW.
    //
    #if !defined(__APPLE__) && !defined(__linux__)
        glewInit();
        if(!GLEW_VERSION_2_0) {
            printf("Your graphics card or graphics driver does\n"
                   "\tnot support OpenGL 2.0, trying ARB extensions\n");
            if(!GLEW_ARB_vertex_shader || !GLEW_ARB_fragment_shader) {
                printf("ARB extensions don't work either.\n");
                printf("\tYou can try updating your graphics drivers.\n"
                       "\tIf that does not work, you will have to find\n");
                printf("\ta machine with a newer graphics card.\n");
                exit(1);
            }
        }
    #endif

    SetupWrapper();

    p = new Parser();
    p->load_file(input_file);

    q = new Parser();
    q->load_file("meshes/quad_all.obj");

    glutDisplayFunc(DisplayCallback);
    glutMouseFunc(MouseCallback);
    glutMotionFunc(TransformCallback);
    glutReshapeFunc(ReshapeCallback);
    glutKeyboardFunc(KeyCallback);
    glutIdleFunc(DisplayCallback);

    glutMainLoop();
    return 0;
}
