
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
#include "file_io.h"

std::string vertexShader;
std::string fragmentShader;

std::string wallVertexShader = "kernels/walls.vert";
std::string wallFragmentShader = "kernels/walls.frag";

std::string groundVertexShader = "kernels/ground.vert";
std::string groundFragmentShader = "kernels/ground.frag";

std::string skyVertexShader = "kernels/sky.vert";
std::string skyFragmentShader = "kernels/sky.frag";


SimpleShaderProgram *shader1;
SimpleShaderProgram *wallShader;
SimpleShaderProgram *groundShader;
SimpleShaderProgram *skyShader;

float x_distortion = 1.0;
float y_distortion = 1.0;
float z_distortion = 1.0;

float ground_level  = -.6f;
float quad_height   = 4.0f;
float quad_size     = 30.0f;

float facing_angle     = 0;


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
    shader1->Bind();
    shader1->SetUniform("x_distortion", x_distortion);
    shader1->SetUniform("y_distortion", y_distortion);
    shader1->SetUniform("z_distortion", z_distortion);
    shader1->SetUniform("facing_angle", facing_angle);

    for ( int i = 0; i < FACES.size(); i ++){
        Vertex a = ((Triangle_face)FACES.at(i)).a;
        Vertex b = ((Triangle_face)FACES.at(i)).b;
        Vertex c = ((Triangle_face)FACES.at(i)).c;
        Vertex u = Vertex(b.x - a.x, b.y - a.y, b.z - a.z);
        Vertex v = Vertex(c.x - a.x, c.y - a.y, c.z - a.z);

        Vertex cross    = Vertex(   u.y * v.z - u.z * v.y,
                                    u.z * v.x - u.x * v.z,
                                    u.x * v.y - u.y * v.x);
        float mag       = sqrt(     cross.x * cross.x 
                                    + cross.y * cross.y 
                                    + cross.z * cross.z);
        Vertex norm     = Vertex(   cross.x / mag, 
                                    cross.y / mag, 
                                    cross.z / mag);

        glBegin(GL_TRIANGLES);
            glNormal3f( norm.x, norm.y, norm.z);
            glVertex3f( ((Triangle_face)FACES.at(i)).a.x, 
                        ((Triangle_face)FACES.at(i)).a.y, 
                        ((Triangle_face)FACES.at(i)).a.z);
            glVertex3f( ((Triangle_face)FACES.at(i)).b.x, 
                        ((Triangle_face)FACES.at(i)).b.y, 
                        ((Triangle_face)FACES.at(i)).b.z);
            glVertex3f( ((Triangle_face)FACES.at(i)).c.x, 
                        ((Triangle_face)FACES.at(i)).c.y, 
                        ((Triangle_face)FACES.at(i)).c.z);
        glEnd();        

    }   

    shader1->UnBind();



    for ( int i = 0; i < FACES.size(); i ++){
        Vertex a = ((Triangle_face)FACES.at(i)).a;
        Vertex b = ((Triangle_face)FACES.at(i)).b;
        Vertex c = ((Triangle_face)FACES.at(i)).c;
        Vertex u = Vertex(b.x - a.x, b.y - a.y, b.z - a.z);
        Vertex v = Vertex(c.x - a.x, c.y - a.y, c.z - a.z);

        Vertex cross    = Vertex(   u.y * v.z - u.z * v.y,
                                    u.z * v.x - u.x * v.z,
                                    u.x * v.y - u.y * v.x);
        float mag       = sqrt(     cross.x * cross.x 
                                    + cross.y * cross.y 
                                    + cross.z * cross.z);
        Vertex norm     = Vertex(   cross.x / mag, 
                                    cross.y / mag, 
                                    cross.z / mag);

        glBegin(GL_TRIANGLES);
            glNormal3f( norm.x, norm.y, norm.z);
            glVertex3f( ((Triangle_face)FACES.at(i)).a.x + 3, 
                        ((Triangle_face)FACES.at(i)).a.y, 
                        ((Triangle_face)FACES.at(i)).a.z);
            glVertex3f( ((Triangle_face)FACES.at(i)).b.x + 3, 
                        ((Triangle_face)FACES.at(i)).b.y, 
                        ((Triangle_face)FACES.at(i)).b.z);
            glVertex3f( ((Triangle_face)FACES.at(i)).c.x + 3, 
                        ((Triangle_face)FACES.at(i)).c.y, 
                        ((Triangle_face)FACES.at(i)).c.z);
        glEnd();


    }   
}


void DrawWithShader(){
    glColor3f(1.0, 0.0, 0.0);
    glLoadIdentity();             /* clear the matrix */
    glRotatef(angle, 0, 1, 0);
    // gluLookAt(  x, y, z, x + lx, y + ly, z + lz, 0.0f, 1.0f, 0.0f);
    gluLookAt(2.0f, 0.0f, z, 0.0f, 0.0f + ly, 0.0f + lz, 0.0f, 1.0f, 0.0f);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    drawGround();
    drawWalls();
    drawSky();
    drawCharacter();
}

void DisplayCallback(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.f, 0.f, -10.f);

    DrawWithShader();
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

void KeyCallback(unsigned char key, int x, int y)
{
    switch(key) {
    case 'q':
        exit(0);
    case 'f':
        z += .40;
        lz -= .40;
        ly -= .04;
        break;
    case 'j':
        z -= .20;
        lz += .20;
        ly += .02;
        break;
    case 's':
        angle += 5.f;
        break;
    case 'l':
        angle -= 5.f;
        break;
    case 'd':
        x_distortion -= .1f;
        break;
    case 'k':
        x_distortion += .1f;
        break;
    case 'i':
        y_distortion += .1f;
        break;
    case ',':
        y_distortion -= .1f;
        break;    
    case 'e':
        z_distortion += .1f;
        break;
    case 'x':
        z_distortion -= .1f;
        break;    
    case 'a':
        x_distortion = 1.0;
        y_distortion = 1.0;
        z_distortion = 1.0;
    default:
        break;
    }

    glutPostRedisplay();
}

void Setup()
{
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


    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 60.0 };
    GLfloat white_light[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat lmodel_ambient[] = { 1.0, 1.0, 1.0, 1.0 };

    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glEnable(GL_DEPTH_TEST);

}


int main(int argc, char** argv){
    if(!(argc == 4)){
        printf("usage: ./hw5 <vertex shader1> <fragment shader1> <mesh> \n");
        return 0;
    }

    vertexShader   = std::string(argv[1]);
    fragmentShader = std::string(argv[2]);
    std::string input_file = "";
    input_file  = argv[3];

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

    Setup();

    load_file(input_file);

    glutDisplayFunc(DisplayCallback);
    glutReshapeFunc(ReshapeCallback);
    glutKeyboardFunc(KeyCallback);
    glutIdleFunc(DisplayCallback);

    glutMainLoop();
    return 0;
}
