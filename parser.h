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

	bool norm;     // true if vertex normals provided
	bool text;     // true if texture coords provided

	// key value pairs: material_id <-> material (stores material properties, including texture filenames)
	std::map<std::string, Material> mat_map;

	// given three vertices, calculates the surface normal
	Normal *cal_norm(Vertex v1, Vertex v2, Vertex v3) {
		Vertex v = Vertex(v3.x_val - v1.x_val, v3.y_val - v1.y_val, v3.z_val - v1.z_val);
		Vertex u = Vertex(v2.x_val - v1.x_val, v2.y_val - v1.y_val, v2.z_val - v1.z_val);
		Normal *result_alloc = (Normal *)malloc(sizeof(Normal));

		Normal result = Normal(u.y_val*v.z_val - u.z_val*v.y_val, u.z_val*v.x_val - u.x_val*v.z_val, u.x_val*v.y_val - u.y_val*v.x_val);
		
		float divisor = sqrt(result.n_x*result.n_x + result.n_y*result.n_y + result.n_z*result.n_z);

		result.n_x = result.n_z / divisor; result.n_y = result.n_y / divisor; result.n_z = result.n_z / divisor; 
		memcpy(result_alloc, &result, sizeof(Normal));
		return result_alloc;
	}

	// parses through the provided .obj file and fills the vertex, face, texture, and normal vectors
	bool load_file(const std::string& filename) {
		norm = false;
		text = false;
		std::ifstream ifs (filename.c_str(), std::ifstream::in);

		char c;
		float x, y, z;
		int i;
		std::string next_line = "";
		std::string word = "";
		std::string curr_mat = "";

		while (getline(ifs, next_line)) {
			if(next_line[0] == '#')
				continue;
			char *str = new char[next_line.length() + 1];
			strcpy(str, next_line.c_str());
			char* arguments = strtok (str, " ");

			if (strcmp(arguments,"mtllib") == 0) {
				arguments = strtok (NULL, " ");
				mat_map = read_mtl(arguments);
			}

			else if (strcmp(arguments, "usemtl") == 0) {
				arguments = strtok (NULL, " ");
				curr_mat = arguments;
			}

			else if (strcmp(arguments, "v") == 0) {
				arguments = strtok (NULL, " ");
				x = strtof(arguments, NULL);  
				arguments = strtok (NULL, " ");
				y = strtof(arguments, NULL);  
				arguments = strtok (NULL, " ");
				z = strtof(arguments, NULL);  
				obj_vertices.push_back(Vertex(x, y, z));
			}

			else if (strcmp(arguments, "vt") == 0) {
				arguments = strtok (NULL, " ");
				x = strtof(arguments, NULL);  
				arguments = strtok (NULL, " ");
				y = strtof(arguments, NULL);  
				obj_textures.push_back(Texture(x, y));
			}

			else if (strcmp(arguments, "vn") == 0) {
				arguments = strtok (NULL, " ");
				x = strtof(arguments, NULL);  
				arguments = strtok (NULL, " ");
				y = strtof(arguments, NULL);  
				arguments = strtok (NULL, " ");
				z = strtof(arguments, NULL);  
				obj_normals.push_back(Normal(x, y, z));
			}

			else if (strcmp(arguments, "f") == 0) {
				Face *f = new Face();
				f->mat_id = curr_mat;
				Index *ind = new Index();
				int counter = 0;
	
				// will definitely be x, y, z
				for(int i = 0; i < 3; i++){
					arguments = strtok(NULL, " ");

					if((char*)strchr(arguments, '/') == NULL){
						ind->v_ind = strtof(arguments, NULL) - 1;
					} else {
						int pos = strchr(arguments, '/') - arguments;
						char vert[16];
						memcpy(vert, arguments, pos);      
						ind->v_ind = strtof(vert, NULL) - 1;
						arguments += pos + 1;
						if((char*)strchr(arguments, '/') == NULL){
							ind->vt_ind = strtof(arguments, NULL) - 1;
						} else {
							int pos = strchr(arguments, '/') - arguments;
							char vert[16];
							memcpy(vert, arguments, pos);      
							ind->vt_ind = strtof(vert, NULL) - 1;
							ind->vn_ind = strtof(&arguments[pos + 1], NULL) - 1;
						}
					}
					f->indices.push_back(*ind);
					ind = new Index();
				}
				obj_faces.push_back(*f);

				delete f;
				delete ind;
			}
			delete [] str;

		}

		ifs.close();
		if (obj_textures.size() > 0) text = true;
		if (obj_normals.size() > 0) norm = true;
		return true;
	}
	
};

#endif