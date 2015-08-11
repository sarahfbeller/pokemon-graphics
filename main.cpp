
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

#include "parser.h"
#include "draw_triangle.h"

// file locations
std::string vertexShader;
std::string fragmentShader;

SimpleShaderProgram *shader;
Parser *p; 
int Button;
int State;
int X;
int Y;

void mouse(int button, int state, int x, int y) {

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

void transform(int curr_x, int curr_y) {

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

void Draw() {
	//init();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

    for(int i = 0; i < p->obj_faces.size(); i++)
        DrawTriangle(p->obj_faces[i], *p);
	// for each (Face f in p->obj_faces) {
	// 	DrawTriangle(f, *p);
	// }

}

void DrawWithShader(){

    shader->Bind();
	
	Draw();
	
    shader->UnBind();
}

void DisplayCallback(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	
    //glMatrixMode(GL_MODELVIEW);
   // glLoadIdentity();
   // glTranslatef(0.f, 0.f, -10.f); 

    Draw();
    glutSwapBuffers();
	glFlush();
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
    default:
        break;
    }

    glutPostRedisplay();
}

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

void Setup()
{	
    shader = new SimpleShaderProgram();
    shader->LoadVertexShader(vertexShader);
    shader->LoadFragmentShader(fragmentShader);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}


int main(int argc, char** argv){
    if(!(argc == 3)){
        printf("usage: ./hw5 <vertex shader> <fragment shader> \n");
        return 0;
    }

    vertexShader   = std::string(argv[1]);
    fragmentShader = std::string(argv[2]);

    // Initialize GLUT.
    glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(20, 20);
    glutInitWindowSize(512, 512);
    glutCreateWindow("CS148 Assignment 5");

	SetupCamera();
	SetupLighting();

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
	p = new Parser();
	// p->load_file("meshes/Ninetales/Ninetales.obj");
    p->load_file("meshes/Eevee.obj");

    glutDisplayFunc(DisplayCallback);
	glutMouseFunc(mouse);
	glutMotionFunc(transform);
   // glutReshapeFunc(ReshapeCallback);
    glutKeyboardFunc(KeyCallback);
    glutIdleFunc(DisplayCallback);

    glutMainLoop();
    return 0;
}
