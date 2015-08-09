// #include "main.h"
#include "alm_main.h"
#include "parser.h"

std::string curr_tex;
GLuint textureID;

// set up the texture settings
void init(std::string& filename) {
	
	if (filename == "") return;

	if (filename == curr_tex) return;           // attempt to improve efficiency, if desired text
	else curr_tex = filename;                   // is already set, don't change it

	SimpleImage texture(filename);
	
	int w = texture.width();
	int h = texture.height();

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_FLOAT, texture.data());
	
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, textureID);
}

// given a face and an initialied parser object draws the face
void DrawTriangle(Face& f, Parser& p) {
	
	// init texture settings if texture provided
	if (p.text == 1) init(p.mat_map[f.mat_id].texture);
	
	glBegin(GL_TRIANGLES);

	// setting space for vertex, texture, and normal
	Vertex *v = (Vertex*)malloc(sizeof(Vertex));
	Texture *t = (Texture*)malloc(sizeof(Texture));
	Normal *n = (Normal*)malloc(sizeof(Normal));

	// if vertex normals are not provided, calculate surface normal
	if (p.norm == 0) {
		n = &(p.cal_norm(p.obj_vertices[f.indices[0].v_ind], p.obj_vertices[f.indices[1].v_ind], p.obj_vertices[f.indices[2].v_ind]));
		glNormal3f(n->n_x, n->n_y, n->n_z);
	}

	// case: vertex texture and vertex normal are both provided
	if (p.text == 1 && p.norm == 1) {
		
		// accessing the vertex, texture, and normal at given face index
		// and using the values stored in the structs
		for (int i = 0; i < 3; i++) {
			v = &p.obj_vertices[f.indices[i].v_ind];
			t = &p.obj_textures[f.indices[i].vt_ind];
			n = &p.obj_normals[f.indices[i].vn_ind];

			glTexCoord2f(t->text_1, 1 - t->text_2);
			glNormal3f(n->n_x, n->n_y, n->n_z);
			glVertex3f(v->x_val, v->y_val, v->z_val);
		}

	}

	// case: vertex texture provided, but no vertex normals
	else if (p.text && p.norm == 0) {

		// accessing the vertex, texture, and normal at given face index
		// and using the values stored in the structs
		for (int i = 0; i < 3; i++) {
			v = &p.obj_vertices[f.indices[i].v_ind];
			t = &p.obj_textures[f.indices[i].vt_ind];
			glTexCoord2f(t->text_1, 1 - t->text_2);
			glVertex3f(v->x_val, v->y_val, v->z_val);
		}

	}

	// case: neither texture nor normal provided
	else {

		// accessing the vertex, texture, and normal at given face index
		// and using the values stored in the structs
		for (int i = 0; i < 3; i++) {
			v = &p.obj_vertices[f.indices[i].v_ind];
			glVertex3f(v->x_val, v->y_val, v->z_val);
		}

	}

	v = NULL;  t = NULL;  n = NULL;

	glEnd();
}