#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <map>
#include "mtl_parser.h"

#ifndef  __PARSER_H__
#define __PARSER_H__

// holds normal vector coords
struct Normal {
	float n_x;
	float n_y;
	float n_z;

	Normal(float x, float y, float z) {
		n_x = x; n_y = y; n_z = z;
	}
};

// holds texture coords
struct Texture {
	float text_1;
	float text_2;

	Texture(float a, float b) { text_1 = a; text_2 = b; }
};

// holds vertex coords
struct Vertex {
	float x_val;
	float y_val;
	float z_val;

	Vertex(float x, float y, float z) { x_val = x; y_val = y; z_val = z; }
	Vertex(float x, float y, float z, float nx, float ny, float nz) { x_val = x; y_val = y; z_val = z; }
};

// holds vertex, texture, and normal indices for face
struct Index {
	int v_ind;
	int vt_ind;
	int vn_ind;
};

// holds "vector" of indices, each Index corresponds to a vertex/texture/normal combo
struct Face {
	std::vector < Index > indices;
	std::string mat_id;
};

// class with vectors of vertices, faces, textures, and normals
class Parser{
public:
	std::vector<Vertex> obj_vertices;
	std::vector<Face> obj_faces;
	std::vector<Texture> obj_textures;
	std::vector<Normal> obj_normals;

	// bool norm = false;     // true if vertex normals provided
	// bool text = false;     // true if texture coords provided
	bool norm;     // true if vertex normals provided
	bool text;     // true if texture coords provided

	// key value pairs: material_id <-> material (stores material properties, including texture filenames)
	std::map<std::string, Material> mat_map;

	// given three vertices, calculates the surface normal
	Normal cal_norm(Vertex v1, Vertex v2, Vertex v3) {
		Vertex v = Vertex(v3.x_val - v1.x_val, v3.y_val - v1.y_val, v3.z_val - v1.z_val);
		Vertex u = Vertex(v2.x_val - v1.x_val, v2.y_val - v1.y_val, v2.z_val - v1.z_val);

		Normal result = Normal(u.y_val*v.z_val - u.z_val*v.y_val, u.z_val*v.x_val - u.x_val*v.z_val, u.x_val*v.y_val - u.y_val*v.x_val);
		
		float divisor = sqrt(result.n_x*result.n_x + result.n_y*result.n_y + result.n_z*result.n_z);

		result.n_x = result.n_z / divisor; result.n_y = result.n_y / divisor; result.n_z = result.n_z / divisor; 
		return result;
	}

	// parses through the provided .obj file and fills the vertex, face, texture, and normal vectors
	bool load_file(const std::string& filename) {
		norm = false;
		text = false;
		std::ifstream ifs (filename.c_str(), std::ifstream::in);

		printf("loading file: %s\n\n", filename.c_str());

		char c = ifs.get();
		float x, y, z;
		int i;
		std::string word = "";
		std::string curr_mat ="";

		printf("creating vertices, textures, faces vectors\n\n");

		while (ifs.good()) {

			while (c != ' ' && c != '\n' && c != '\t' && c != '#') {
				word += c;
				c = ifs.get();
			}

			if (word == "mtllib") {
				word = "";
				c = ifs.get();
				while (c != ' ' && c != '\n') {
					word += c;
					c = ifs.get();
				}
				printf("%s, \n", word.c_str());
				mat_map = read_mtl(word);
				word = "";
			}

			else if (word == "usemtl") {
				word = "";
				c = ifs.get();
				while (c != ' ' && c != '\n') {
					word += c;
					c = ifs.get();
				}
				curr_mat = word;
				word = "";
			}

			else if (word == "v") {
				ifs >> x;  ifs >> y; ifs >> z;
				obj_vertices.push_back(Vertex(x, y, z));
				word = "";
			}

			else if (word == "vt") {
				ifs >> x; ifs >> y;
				obj_textures.push_back(Texture(x, y));
				word = "";
			}

			else if (word == "vn") {
				ifs >> x; ifs >> y; ifs >> z;
				obj_normals.push_back(Normal(x, y, z));
				word = "";
			}

			else if (word == "f") {
				Face *f = new Face();
				f->mat_id = curr_mat;
				Index *ind = new Index();
				word = "";
				int counter = 0;
	
				while (c != '\n') {
					ifs >> i;
					c = ifs.get();

					if (counter == 0) ind->v_ind = i - 1;              // not very elegant...Find another implementation later
					else if (counter == 1) ind->vt_ind = i - 1;
					else if (counter == 2) ind->vn_ind = i - 1;

					if (c == ' ') {
						f->indices.push_back(*ind);
						ind = new Index();
						counter = 0;
					}

					else if (c == '/') counter++;
				}

				f->indices.push_back(*ind);
				obj_faces.push_back(*f);

				delete f;
				delete ind;

				word = "";
			}

			c = ifs.get();
			word = "";	

		}


		ifs.close();

		if (obj_textures.size() > 0) text = true;
		if (obj_normals.size() > 0) norm = true;

		return true;
	}
	

};

#endif