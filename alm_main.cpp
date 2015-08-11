
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
float quad_size     = 30.0f;

float lx = 0.1f, lz = -1.0f, ly = -0.15f;
float x = -0.1f, z = 1.0f, y = 0.15f;
static SimpleImage texPNG;


// SimpleTexture * texture;


// ====================== Draw Scene Helpers ================= //
void drawGround(){
    groundShader->Bind();
    glBegin(GL_QUADS);
        glVertex3f(-quad_size, ground_level, quad_size);
        glVertex3f(quad_size, ground_level, quad_size);
        glVertex3f(quad_size, ground_level, -quad_size);
        glVertex3f(-quad_size, ground_level, -quad_size);
    glEnd();
    groundShader->UnBind();
}

void drawSky(){
    skyShader->Bind();
    glBegin(GL_QUADS);
        glVertex3f(-quad_size, quad_height, -quad_size);
        glVertex3f(quad_size, quad_height, -quad_size);
        glVertex3f(quad_size, quad_height, quad_size);
        glVertex3f(-quad_size, quad_height, quad_size);
    glEnd();
    skyShader->UnBind();
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
    glTranslatef(-x_position, 0, -z_position);
    glRotatef(facing_angle, 0, 1, 0);
    glTranslatef(x_position, 0, z_position);
    
    // shader1->Bind();
    // shader1->SetUniform("x_position", x_position);
    // shader1->SetUniform("z_position", z_position);
    // shader1->SetUniform("facing_angle", facing_angle);

    for(int f = 0; f < (p->obj_faces).size(); f++){
        Face cur_face = p->obj_faces.at(f);
        std::vector <Index> indices = cur_face.indices;

        if(p->text && p->mat_map[cur_face.mat_id].texture != "")
            mtl_init(p->mat_map[cur_face.mat_id].texture);

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
            }
        glEnd();

    }
    if(p->text)
        glDisable(GL_TEXTURE_2D);

    // shader1->UnBind();  
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

void SetupWrapper(){
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
    }

    else if (state == GLUT_DOWN) {
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
        
    }

    else if (Button == 2) {
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
