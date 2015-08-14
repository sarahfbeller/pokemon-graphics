
#include "main.h"
#ifdef WIN32
#define ssize_t SSIZE_T
#endif

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <cmath>

// ====================== Draw Scene Helpers ================= //
// set up the texture settings
void mtl_init (std::string& filename) {
    if (filename == "") return;
    if (filename == curr_tex) 
        return;           // attempt to improve efficiency, if desired text
    else 
        curr_tex = filename;                   // is already set, don't change it

    texture = SimpleImage(filename);
    
    int w = texture.width();
    int h = texture.height();

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_FLOAT, texture.data());
    
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

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

// Draws the ground from the texture file specified. 
// Assumes that the quad is square and that the size has been set in the .h file
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

// Creates a sphere for the sky that encapsulates the entire quad.
// Colors sky in shader file.
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

// If selected, renders the quad (all walls at once) from the .obj file
// in coordination with the .mtl file from Maya.
// Very slow, but cool!
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

// If selected (default), draws walls as glRects rather than as
// rectangular prisms.
void drawWalls(){
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);

    // Memchu side
    glBindTexture(GL_TEXTURE_2D, memchuImgID);
    glBegin(GL_QUADS);
        glTexCoord2f(.99f, 0.01f);
        glVertex3f(-quad_size, quad_height, -quad_size);
        glTexCoord2f(0.01f, 0.01f);
        glVertex3f(-quad_size, quad_height, quad_size);
        glTexCoord2f(0.01f, .99f);
        glVertex3f(-quad_size, ground_level, quad_size);
        glTexCoord2f(.99f, .99f);
        glVertex3f(-quad_size, ground_level, -quad_size);
    glEnd();

    // Back side, towards Palm Drive
    glBindTexture(GL_TEXTURE_2D, backImgID);
    glBegin(GL_QUADS);
        glTexCoord2f(.99f, .99f);
        glVertex3f(quad_size, ground_level, -quad_size);
        glTexCoord2f(0.01f, .99f);
        glVertex3f(quad_size, ground_level, quad_size);
        glTexCoord2f(0.01f, 0.01f);
        glVertex3f(quad_size, quad_height, quad_size);
        glTexCoord2f(.99f, 0.01f);
        glVertex3f(quad_size, quad_height, -quad_size);
    glEnd();

    // Two sides of the quad
    glBindTexture(GL_TEXTURE_2D, sidesImgID);
    glBegin(GL_QUADS);
        glTexCoord2f(.99f, .99f);
        glVertex3f(-quad_size, ground_level, -quad_size);
        glTexCoord2f(0.01f, .99f);
        glVertex3f(quad_size, ground_level, -quad_size);
        glTexCoord2f(0.01f, 0.01f);
        glVertex3f(quad_size, quad_height, -quad_size);
        glTexCoord2f(.99f, 0.01f);
        glVertex3f(-quad_size, quad_height, -quad_size);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, sidesImgID);
    glBegin(GL_QUADS);
        glTexCoord2f(.99f, 0.01f);
        glVertex3f(-quad_size, quad_height, quad_size);
        glTexCoord2f(0.01f, 0.01f);
        glVertex3f(quad_size, quad_height, quad_size);
        glTexCoord2f(0.01f, .99f);
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
    Bone *b = bones[TORSO];

    // convert degrees in bone angle to radians
    float newDeg_z = (b->angle_z-90)*pi/180;

    float cosval = cos(newDeg_z);
    float sinval = sin(newDeg_z);

    // rotation matrix about the axis of rotation calculated earlier of the bone angle
    float rot_mat[3][3] = { {cosval, -sinval, 0},
                            {sinval, cosval, 0},
                            {0, 0, 1} };

    // loop throug the faces
    for(int f = 0; f < (p->obj_faces).size(); f++){
        Face cur_face = p->obj_faces.at(f);
        std::vector <Index> indices = cur_face.indices;

        // check if textures are provided in material file
        if(p->text && p->mat_map[cur_face.mat_id].texture != ""){
            mtl_init(p->mat_map[cur_face.mat_id].texture); 
        } else {
            character_shader->Bind();
            character_shader->SetUniform("x_position", x_position);
            character_shader->SetUniform("z_position", z_position);
            character_shader->SetUniform("facing_angle", facing_angle);
        }

        // draw face
        glBegin(GL_TRIANGLES);
            for (int i = 0; i < 3; i++){
                Vertex * v = &p->obj_vertices[indices[i].v_ind];

                // check if texture coordinates are provided
                if (p->text) {
                    Texture * t = &p->obj_textures[indices[i].vt_ind];
                    glTexCoord2f(t->text_1, 1 - t->text_2);
                }

                // check if normals are provided, otherwise calculate it
                if(p->norm){
                    Normal * n = &p->obj_normals[indices[i].vn_ind];
                    glNormal3f(n->n_x, n->n_y, n->n_z);    
                } else {
                    Normal * n = (p->cal_norm(p->obj_vertices[indices[0].v_ind], p->obj_vertices[indices[1].v_ind], p->obj_vertices[indices[2].v_ind]));
                    glNormal3f(n->n_x, n->n_y, n->n_z);                    
                }
                
                // check if currently being animated
                if (isWalking) {
                    float new_coords[3] = {0,0,0};  // coords to be used after rotation

                    // vector about the origin
                    float o_vec[3] = { ((v->x_val) - b->x0), ((v->y_val) - b->y0), ((v->z_val) - b->z0) };
                    
                    // matrix multiplication
                    for (int i = 0; i < 3; i++) {
                        float val = 0;
                        for (int j = 0; j < 3; j++) {
                            val += rot_mat[i][j] * o_vec[j];
                        }
                        new_coords[i] = val;
                    }
                    
                    // draw vertex
                    glVertex3f(new_coords[0] + b->x0 + x_position, new_coords[1] + b->y0, new_coords[2] + b->z0 + z_position);
                    
                } else {
                    glVertex3f(v->x_val + x_position,v->y_val, v->z_val + z_position);
                }
            }
        glEnd();
    }
    character_shader->UnBind();  
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

    // INSTRUCTIONS: choose either drawWalls or drawScene, but not both!
    drawWalls();
    // drawScene();

    // Skeleton is invisible by default, but can be made visible if desired
    drawBone(bones[0]);
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

// First bone added must be root.
// Currently makes spine - see 3dPikachu.cpp for entire skeleton structure.
void makeBones() {
    bones = std::vector<struct Bone *>();
    struct Bone *torso = new struct Bone;

    std::vector<struct Bone *> children;
    // torso
    float torsoLen = 2.0;
    makeBone(torso, x_position, 0, z_position, torsoLen, 0.5, 0.5, 0.0, 0.0, 360.0, 
            0.0, 0.0, 360.0, 90.0, 0.0, 360.0, children);

    bones.push_back(torso);
}

void SetupWrapper(){
    makeBones();

    character_shader = new SimpleShaderProgram();
    character_shader->LoadVertexShader(vertexShader);
    character_shader->LoadFragmentShader(fragmentShader);

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

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    SetupCamera();
    SetupLighting();
    glEnable(GL_DEPTH_TEST);
}

// =================== Animation Functions ================= //

/* Makes character sway back and forth perpendicular to direction of walking. */
void sway() {
    float deltaAngle = 0.0;

    if (currTime <= t0+dt) {
        deltaAngle += t0BodyAngle;
    } else if (currTime >= t2) {
        //deltaAngle += t2BodyAngle;
        currTime = t0;
        bones[TORSO]->angle_z = 90.f;
        deltaAngle = 0;
    } else {
        float newAngle = 0.0;
        if (currTime < t1) {
            newAngle = ((currTime/(t1-t0))*(t1BodyAngle-t0BodyAngle)) + t0BodyAngle;
        } else if (currTime < t2) {
            newAngle = (((currTime-t1)/(t2-t1))*(t2BodyAngle-t1BodyAngle)) + t1BodyAngle;
        }
        deltaAngle += newAngle;
    }
    bones[TORSO]->angle_z += deltaAngle;
    currTime += dt;
    glutPostRedisplay();
}

// =================== Callback Functions ================== //
void DisplayCallback(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.f, 0.f, -10.f);
    if (isWalking) sway();
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

// All of the controls for manipulating the camera and the character
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

    // Character movement
        // Character rotation
    case 'f':
        facing_angle -= 8.f;
        bones[TORSO]->angle_y = facing_angle;
        break;
    case 's':
        facing_angle += 8.f;
        bones[TORSO]->angle_y = facing_angle;
        break;
        // Character movement (s: forward, x: backwards)
    case 'e':
        z_position += .5f * cos(pi * facing_angle/180.f);
        bones[TORSO]->z0 = z_position;
        x_position += .5f * sin(pi * facing_angle/180.f);
        bones[TORSO]->x0 = x_position;
        break;
    case 'x':
        z_position -= .5f * cos(pi * facing_angle/180.f);
        bones[TORSO]->x0 = z_position;
        x_position -= .5f * sin(pi * facing_angle/180.f);
        bones[TORSO]->x0 = x_position;
        break;    
        // Character animation: begins swaying motion
    case 'w':
        isWalking = !isWalking;
        currTime = 0.0;
        bones[TORSO]->angle_z = 90.0;
        break;

    // Reset Character Position
    case 'a':
        x_position = 1.0;
        bones[TORSO]->x0 = x_position;
        z_position = 1.0;
        bones[TORSO]->z0 = z_position;
    default:
        break;
    }

    // Check for boundedness -- offset is given so character does not
    // run into or through walls.
    z_position = fmax(z_position, -quad_size + 4.f);
    z_position = fmin(z_position, quad_size - 4.f);
    bones[TORSO]->z0 = z_position;

    x_position = fmax(x_position, -quad_size + 4.f);
    x_position = fmin(x_position, quad_size - 4.f);
    bones[TORSO]->x0 = x_position;
    glutPostRedisplay();
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

    // Initialize GLEW.
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

    // If the Maya quad rendering option is selected (in drawingWrapper), 
    // this is necessary
    q = new Parser();
    q->load_file("meshes/quad_all.obj");

    glutDisplayFunc(DisplayCallback);
    glutMotionFunc(TransformCallback);
    glutReshapeFunc(ReshapeCallback);
    glutKeyboardFunc(KeyCallback);
    glutIdleFunc(DisplayCallback);
    glutMainLoop();
    return 0;
}