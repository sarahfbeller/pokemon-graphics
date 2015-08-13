
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
#include "draw_triangle.h"
#include "parser.h"
// #include "SimpleTexture.h"

// =================== Element Shaders ====================== //
    SimpleShaderProgram *shader1;
    SimpleShaderProgram *wallShader;
    SimpleShaderProgram *groundShader;
    SimpleShaderProgram *skyShader;

    std::string vertexShader = "kernels/character.vert";
    std::string fragmentShader = "kernels/character.frag";

    std::string wallVertexShader = "kernels/walls.vert";
    std::string wallFragmentShader = "kernels/walls.frag";

    std::string groundVertexShader = "kernels/ground.vert";
    std::string groundFragmentShader = "kernels/ground.frag";

    std::string skyVertexShader = "kernels/sky.vert";
    std::string skyFragmentShader = "kernels/sky.frag";

// ==================== Globals ============================= //
Parser *p; 
int Button;
int State;
int X;
int Y;

float x_position        = 0.0;
float z_position        = 0.0;
float vert_camera_pos   = 1.0;
float angle             = 0.f;
float facing_angle      = 0.f;

float ground_level  = 0.f;
float quad_height   = 8.0f;
float quad_size     = 50.0f;

float lx = 0.1f, lz = -1.0f, ly = -0.15f;
float x = -0.1f, z = 1.0f, y = 0.15f;
static SimpleImage texPNG;

std::string ground_file_name = "meshes/Textures/stone.jpg";
static SimpleImage groundIMG;
GLuint groundImgID;

std::string sky_file_name = "meshes/Textures/sky2.jpg";
static SimpleImage skyIMG;
GLuint skyImgID;

struct Bone {
    float x0, y0, z0; // starting position
    float xdim, ydim, zdim; // x,y,z dimensions of bone
    float angle_x, minAngle_x, maxAngle_x; // initial angle and limits of motion
    float angle_y, minAngle_y, maxAngle_y;
    float angle_z, minAngle_z, maxAngle_z;
    std::vector<struct Bone *> children;
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

// globals for animation
float dt = 0.01;
float currTime = 0.0;
bool isWalking = false;
bool goingForward = false;
bool headShaking = false;
float currBodyRotation = 0.0;
float currHeadRotation = 0.0;

// ====================== Draw Scene Helpers ================= //

/* recursively draws each bone, then its children */
void drawBone(struct Bone *bone) {
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
    // groundShader->Bind();
    // mtl_init(groundIMG);
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
    // groundShader->UnBind();
    glPopMatrix();

}

void drawSky(){
    glPushMatrix();
    // skyShader->Bind();
    // mtl_init(sky_file_name);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, skyImgID);

    glBegin(GL_QUADS);
        glTexCoord2f(0.f, 0.f);
        glVertex3f(-quad_size, quad_height, -quad_size);
        glTexCoord2f(0.f, 1.f);
        glVertex3f(quad_size, quad_height, -quad_size);
        glTexCoord2f(1.f, 1.f);
        glVertex3f(quad_size, quad_height, quad_size);
        glTexCoord2f(1.f, 0.f);
        glVertex3f(-quad_size, quad_height, quad_size);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

void drawWalls(){
    wallShader->Bind();
    glBegin(GL_QUADS);
        glVertex3f(-quad_size, quad_height, -quad_size);
        glVertex3f(-quad_size, quad_height, quad_size);
        glVertex3f(-quad_size, ground_level, quad_size);
        glVertex3f(-quad_size, ground_level, -quad_size);

        glVertex3f(quad_size, ground_level, -quad_size);
        glVertex3f(quad_size, ground_level, quad_size);
        glVertex3f(quad_size, quad_height, quad_size);
        glVertex3f(quad_size, quad_height, -quad_size);

        glVertex3f(-quad_size, ground_level, -quad_size);
        glVertex3f(quad_size, ground_level, -quad_size);
        glVertex3f(quad_size, quad_height, -quad_size);
        glVertex3f(-quad_size, quad_height, -quad_size);

        glVertex3f(-quad_size, quad_height, quad_size);
        glVertex3f(quad_size, quad_height, quad_size);
        glVertex3f(quad_size, ground_level, quad_size);
        glVertex3f(-quad_size, ground_level, quad_size);
    glEnd();
    wallShader->UnBind();
}

void drawCharacter(){
    glPushMatrix();
    // facing direction
    glTranslatef(-x_position, 0, -z_position);
    glRotatef(facing_angle, 0, 1, 0);
    glTranslatef(x_position, 0, z_position);
    

    for(int f = 0; f < (p->obj_faces).size(); f++){
        Face cur_face = p->obj_faces.at(f);
        std::vector <Index> indices = cur_face.indices;

        if(p->text && p->mat_map[cur_face.mat_id].texture != ""){
            mtl_init(p->mat_map[cur_face.mat_id].texture); // for cow
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

                // Sarah gets animation code integrated into this file
                // Sarah gets bones into floats offset_x, y, z
                // Sarah makes function to turn offsets into 3x3 matrix
                // Erin figures out the below transform functions
                    // to take in bone and matrix and transform vertex.
                // Later Sarah figures out how to set bone for each vertex

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
    // gluLookAt(  x, y, z, x + lx, y + ly, z + lz, 0.0f, 1.0f, 0.0f);
    gluLookAt(0.0f, vert_camera_pos, z, 0.0f, 0.0f + ly, 0.0f + lz, 0.0f, 1.0f, 0.0f);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    drawGround();
    drawWalls();
    drawSky();
    drawBone(bones[0]);
    drawCharacter();
}


// =================== Setup Functions ===================== //
void SetupLighting() {
    GLfloat light_position[] = { 0.0, 20.0, 0.0, 0.0 };
    GLfloat white_light[] = { .8, .8, .8, 1.0 };
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
            std::vector<struct Bone *> children) {
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
            0.0, 0.0, 360.0, 90.0, 0.0, 360.0, children);

    // head
    children.clear();
    children.push_back(leftEar);
    children.push_back(rightEar);
    makeBone(head, 0.0, 0.0, 0.0, 0.15, 0.15, 0.15, 0.0, 0.0, 0.0, 
            0.0, 0.0, 0.0, 0.0, -30.0, 30.0, children);
    // arms
    children.clear();
    makeBone(leftArm, 0.0, 0.0, 0.0, 0.2, 0.05, 0.05, 0.0, 0.0, 0.0, 
            0.0, 0.0, 0.0, 120.0, 0.0, 180.0, children);
    makeBone(rightArm, 0.0, 0.0, 0.0, 0.2, 0.05, 0.05, 0.0, 0.0, 0.0, 
            0.0, 0.0, 0.0, -120.0, -18.0, 0.0, children);
    // legs
    makeBone(leftLeg, -1*torsoLen, 0.0, 0.0, 0.2, 0.05, 0.05, 0.0, 0.0, 0.0, 
            0.0, 0.0, 0.0, 160.0, 90.0, 180.0, children);
    makeBone(rightLeg, -1*torsoLen, 0.0, 0.0, 0.2, 0.05, 0.05, 0.0, 0.0, 0.0, 
            0.0, 0.0, 0.0, -160.0, -90.0, -180.0, children);
    // ears
    makeBone(leftEar, 0.0, 0.0, 0.0, 0.2, 0.05, 0.05, 0.0, 0.0, 0.0, 
            0.0, 0.0, 0.0, 30.0, 15.0, 45.0, children);
    makeBone(rightEar, 0.0, 0.0, 0.0, 0.2, 0.05, 0.05, 0.0, 0.0, 0.0, 
            0.0, 0.0, 0.0, -30.0, -45.0, -15.0, children);

    bones.push_back(torso);
    bones.push_back(leftArm);
    bones.push_back(rightArm);
    bones.push_back(leftLeg);
    bones.push_back(rightLeg);
    bones.push_back(head);
    bones.push_back(leftEar);
    bones.push_back(rightEar);
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
    int gw = groundIMG.width();
    int gh = groundIMG.height();
    glGenTextures(1, &groundImgID);
    glBindTexture(GL_TEXTURE_2D, groundImgID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, gw, gh, 0, GL_RGB, GL_FLOAT, groundIMG.data());
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

    skyIMG = SimpleImage(sky_file_name);
    int sw = skyIMG.width();
    int sh = skyIMG.height();
    glGenTextures(2, &skyImgID);
    glBindTexture(GL_TEXTURE_2D, skyImgID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sw, sh, 0, GL_RGB, GL_FLOAT, skyIMG.data());
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    SetupCamera();
    SetupLighting();
    glEnable(GL_DEPTH_TEST);
}

// =================== Callback Functions ================== //
void DisplayCallback(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.f, 0.f, -10.f);

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
        z += .40;
        lz -= .40;
        ly -= .04;
        break;
    case 'o':
        z -= .20;
        lz += .20;
        ly += .02;
        break;

    // Change camera view
    case 'l':
        angle += 5.f;
        break;
    case 'j':
        angle -= 5.f;
        break;
    case 'i':
        vert_camera_pos += .1f;
        break;
    case ',':
        vert_camera_pos -= .1f;
        break;    
    case 'm':
        facing_angle -= 5.f;
        break;
    case '.':
        facing_angle += 5.f;

    // Move Character
    case 's':
        x_position -= .1f;
        break;
    case 'f':
        x_position += .1f;
        break;
    case 'e':
        z_position += .1f;
        break;
    case 'x':
        z_position -= .1f;
        break;    

    // Reset Character Position
    case 'a':
        x_position = 1.0;
        z_position = 1.0;
    default:
        break;
    }

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

    glutDisplayFunc(DisplayCallback);
    glutMouseFunc(MouseCallback);
    glutMotionFunc(TransformCallback);
    glutReshapeFunc(ReshapeCallback);
    glutKeyboardFunc(KeyCallback);
    glutIdleFunc(DisplayCallback);

    glutMainLoop();
    return 0;
}
