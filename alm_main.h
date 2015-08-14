// main.h
#ifndef __MAIN_H__
#define __MAIN_H__

/*
  This file is used to pull in all the requirements
  for running a program with the GLEW library for
  loadng OpenGL extensions.

  Depending on how GLEW gets set up for your system,
  you may need to edit this file.
*/

/* Include-order dependency!
*
* GLEW must be included before the standard GL.h header.
*/
#ifdef __APPLE__
#define GLEW_VERSION_2_0 1
#elif defined(__linux__)
#define GLEW_VERSION_2_0 1
#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include <GL/glext.h>
#else
#define GLEW_STATIC 1
#include "GL/glew.h"
#endif

#include <string.h>
#include <stdexcept>
#include <cmath>
//
// Forward declarations
//
class SimpleImage;

//
// headers.
//
#include "SimpleImage.h"
#include "SimpleShaderProgram.h"
#include "findGLUT.h"
#include "draw_triangle.h"
#include "parser.h"


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
Parser *q;
int Button;
int State;
int X;
int Y;

float x_position        = 0.2;
float z_position        = 0.2;
float vert_camera_pos   = 3.0;
float angle             = 0.f;
float facing_angle      = 0.f;
float pi                = 3.14579;

float ground_level  = 0.f;
float quad_height   = 18.0f;
float quad_size     = 60.0f;

float lx = 0.1f, lz = -1.0f, ly = -0.15f;
float x = -0.1f, z = 1.0f, y = 0.15f;
static SimpleImage texPNG;

std::string ground_file_name = "meshes/Textures/stone1.jpg";
static SimpleImage groundIMG;
GLuint groundImgID;

std::string sky_file_name = "meshes/Textures/sky2.jpg";
static SimpleImage skyIMG;
GLuint skyImgID;

std::string sides_file_name = "meshes/Textures/left_quad.jpg";
static SimpleImage sidesIMG;
GLuint sidesImgID;

static SimpleImage memchuIMG;
GLuint memchuImgID;

static SimpleImage backIMG;
GLuint backImgID;

static SimpleImage sceneIMG;
GLuint sceneImgID;


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
float currBodyRotation = 0.0;
float currHeadRotation = 0.0;

#endif // __MAIN_H__
